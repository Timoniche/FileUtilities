#include "copies_finder.h"
#include "binary_tree.h"

#include <QFileInfo>
#include <QDirIterator>
#include <QThread>
#include <array>

namespace {

    struct cancellation_exception : std::exception {
        const char *what() const noexcept override {
            return "Stop pressed";
        }
    };

}

void copies_finder::process_impl() {
    try {
        binary_tree Tree(this);
        auto cancellation_point = [thread = QThread::currentThread(), this]() {
            if (thread->isInterruptionRequested()) {
                throw cancellation_exception();
            }
			
			if (_pause_required) {
				_continue.lock();
				_pause_Manager.wait(&_continue);
				_continue.unlock();
			}

        };
		
        QDirIterator it(_dir, QDir::Hidden | QDir::Files, QDirIterator::Subdirectories);
        std::vector<std::string> files_path;
        int count_of_files = 0;
        while (it.hasNext()) {
            files_path.push_back(it.next().toStdString());
            count_of_files++;
        }

        emit set_max_bar(count_of_files);

        //grouping files by size
        std::map<size_t, std::vector<std::string>> map;

        for (const auto &path : files_path) {
            cancellation_point();
            size_t size = static_cast<size_t>((QFileInfo(QString::fromStdString(path))).size());
            if (size != 0) {
                auto iter = map.find(size);
                if (iter == map.end()) {
                    std::vector<std::string> new_group;
                    new_group.push_back(path);
                    map.insert({size, new_group});
                } else {
                    iter->second.push_back(path);
                }
            } else {
                emit increase_bar(1);
            }
        }

        qRegisterMetaType<MyArray>("MyArray");
        qRegisterMetaType<clock_t>("clock_t");

        std::exception_ptr ex_ptr;

        for (auto u : map) {
            cancellation_point();

            if (u.second.size() <= 1) {
                emit increase_bar(static_cast<int>(u.second.size()));
                continue;
            }

            auto *ANS = new std::vector<std::vector<std::string>>();

            //not to open so many streams at one time
            std::map<std::vector<char>, std::vector<int>> mapa;
            for (int i = 0; i < u.second.size(); ++i) {
                std::array<char, 4> buf_from{};
                auto *buf = new std::vector<char>();
                std::ifstream stream_initial(u.second[i], std::ios::binary);
                if (!stream_initial.is_open()) {
                    throw std::runtime_error("Can't open one file");
                }
                stream_initial.read(buf_from.data(), buf_from.size());
                long gcount = stream_initial.gcount();
                for (size_t j = 0; j < gcount; ++j) {
                    buf->push_back(buf_from[j]);
                }
                auto iterator = mapa.find(*buf);
                if (iterator == mapa.end()) {
                    auto *new_group = new std::vector<int>();
                    new_group->push_back(i);
                    mapa.insert({*buf, *new_group});
                } else {
                    iterator->second.push_back(i);
                }
                stream_initial.close();
            }

            size_t item_size = 0;
            std::vector<std::ifstream> streams(u.second.size());
            for (auto &paths_numbers : mapa) {
                cancellation_point();
                for (auto &path_number : paths_numbers.second) {
                    std::ifstream fin(u.second[path_number], std::ios::binary);
                    if (!fin.is_open()) {
                        throw std::runtime_error("Can't open one more stream");
                    }
                    streams[path_number] = std::move(fin);
                }
                std::vector<std::vector<int>> ans;
                Tree.build_final_tree(streams, &paths_numbers.second, ans, 0);

                item_size += ans.size();
                for (auto &path_number : paths_numbers.second) {
                    if (streams[path_number].is_open()) {
                        streams[path_number].close();
                    }
                }
                std::vector<std::vector<std::string>> ans_to_merge;
                for (const auto &p : ans) {
                    std::vector<std::string> temp;
                    for (auto p2 : p) {
                        temp.push_back(u.second[p2]);
                    }
                    ans_to_merge.push_back(std::move(temp));
                }
                ANS->insert(ANS->end(), std::make_move_iterator(ans_to_merge.begin()),
                            std::make_move_iterator(ans_to_merge.end()));

            }
            if (!ANS->empty()) {
                emit update_tree(*ANS);
            }
            emit increase_bar(static_cast<int>(u.second.size()));
            cancellation_point();
        }
    } catch (std::exception &ex) {
        emit error(ex.what());
    }

    emit finished();
}

void copies_finder::process() {
    try {
        process_impl();
    } catch (std::exception &ex) {
        emit error(ex.what());
    }

}
