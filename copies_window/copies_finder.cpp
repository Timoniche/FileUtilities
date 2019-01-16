#include "copies_finder.h"
#include "binary_tree.h"

#include <QFileInfo>
#include <QDirIterator>
#include <QThread>
#include <array>
#include <QCryptographicHash>

namespace {

    struct cancellation_exception : std::exception {
        const char *what() const noexcept override {
            return "Process interrupted";
        }
    };

}

void copies_finder::process_impl() {
    try {
        //binary_tree Tree(this);
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

        auto get_sha256hash = [&](std::string const &path) {
            std::array<char, 8192> buffer{};
            std::ifstream fin(path, std::ios::binary);
            if (!fin.is_open()) {
                emit log(QString(tr("Can't open one file ")) + QString::fromStdString(path));
            }
            QCryptographicHash hash(QCryptographicHash::Sha256);
            int gcount = 0;
            do {
                cancellation_point();
                fin.read(buffer.data(), buffer.size());
                gcount = static_cast<int>(fin.gcount());
                hash.addData(buffer.data(), gcount);
            } while (gcount > 0);
            return hash.result();
        };

        QDirIterator it(_dir, QDir::Hidden | QDir::Files, QDirIterator::Subdirectories);
        std::vector<std::string> files_path;
        int count_of_files = 0;
        while (it.hasNext()) {
            cancellation_point();
            files_path.push_back(it.next().toStdString());
            count_of_files++;
        }

        emit set_max_bar(count_of_files);

        std::map<size_t, std::vector<std::string>> grouping_files_by_size;
        for (const auto &path : files_path) {
            cancellation_point();
            size_t size = static_cast<size_t>((QFileInfo(QString::fromStdString(path))).size());
            if (size != 0) {
                auto iter = grouping_files_by_size.find(size);
                if (iter == grouping_files_by_size.end()) {
                    grouping_files_by_size.insert({size, {path}});
                } else {
                    iter->second.push_back(path);
                }
            } else {
                emit increase_bar(1);
            }
        }

        for (auto u : grouping_files_by_size) {
            cancellation_point();

            if (u.second.size() <= 1) {
                emit increase_bar(static_cast<int>(u.second.size()));
                continue;
            }

            std::vector<std::vector<std::string>> ANS;

            std::map<QByteArray, std::vector<int>> first_hundred_bytes_hash;
            QCryptographicHash hash(QCryptographicHash::Sha256);
            for (int i = 0; i < static_cast<int>(u.second.size()); ++i) {
                cancellation_point();
                std::array<char, 100> buf{};
                std::ifstream stream_initial(u.second[i], std::ios::binary);
                if (!stream_initial.is_open()) {
                    emit log(QString(tr("Can't open one file ")) + QString::fromStdString(u.second[i]));
                    continue;
                }
                stream_initial.read(buf.data(), buf.size());
                size_t gcount = static_cast<size_t>(stream_initial.gcount()); //NOLINT
                hash.addData(buf.data(), static_cast<int>(gcount));
                QByteArray res = hash.result();
                hash.reset();
                auto iterator = first_hundred_bytes_hash.find(res);
                if (iterator == first_hundred_bytes_hash.end()) {
                    first_hundred_bytes_hash.insert({res, {i}});
                } else {
                    iterator->second.push_back(i);
                }
                stream_initial.close();
            }

            std::map<QByteArray, std::vector<int>> result_map;
            for (auto &paths_numbers : first_hundred_bytes_hash) {
                if (paths_numbers.second.size() < 2) { continue; }
                cancellation_point();
                for (auto file_number : paths_numbers.second) {
                    auto byte_array = get_sha256hash(u.second[static_cast<size_t>(file_number)]);
                    auto iterator = result_map.find(byte_array);
                    if (iterator == result_map.end()) {
                        result_map.insert({byte_array, {file_number}});
                    } else {
                        iterator->second.push_back(file_number);
                    }
                }
            }

            for (auto &paths_numbers : result_map) {
                cancellation_point();
                if (paths_numbers.second.size() < 2) { continue; }
                std::vector<std::string> temp;
                for (auto file_number : paths_numbers.second) {
                    temp.push_back(u.second[file_number]);
                }
                ANS.push_back(std::move(temp));
            }

            if (!ANS.empty()) {
                emit update_tree(ANS);
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
