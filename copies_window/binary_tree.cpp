#include <array>
#include "binary_tree.h"
#include <QCryptographicHash>

const int gap_point = 6;

#include <QThread>

namespace {
    struct cancellation_exception : std::exception {
        const char *what() const noexcept override {
            return "Process interrupted";
        }
    };
}

void binary_tree::build_final_tree(std::vector<std::ifstream> &streams, std::vector<int> *fNumbers,
                                   std::vector<std::vector<int>> &ans, unsigned long two_pow_len) {

    auto cancellation_point = [thread = QThread::currentThread(), this]() {
        if (thread->isInterruptionRequested()) {
            throw cancellation_exception();
        }
        if (worker_from->_pause_required) {
            worker_from->_continue.lock();
            worker_from->_pause_Manager.wait(&worker_from->_continue);
            worker_from->_continue.unlock();
        }
    };


    if (two_pow_len > 23) two_pow_len = 23;
    std::vector<char> buffer(1ull << two_pow_len);
    cancellation_point();
    if (two_pow_len < gap_point) {
        size_t gcount = 0;
        std::map<std::vector<char>, std::vector<int>> Map;
        for (int fNumber : *fNumbers) {
            auto index = static_cast<size_t>(fNumber);
            streams[index].read(buffer.data(), buffer.size());
            gcount = static_cast<size_t>(streams[index].gcount());
            auto iter = Map.find(buffer);
            if (iter == Map.end()) {
                std::vector<int> v(1, fNumber);
                Map.insert({buffer, std::move(v)});
            } else {
                iter->second.push_back(fNumber);
            }
        }
        if (gcount == buffer.size()) {
            for (auto u : Map) {

                if (u.second.size() < 2) {
                    continue;
                } else if (u.second.size() == 2) {
                    if (is_two_files_equal(u.second[0], u.second[1], streams)) {
                        ans.push_back(u.second);
                    }
                } else {
                    binary_tree::build_final_tree(streams, &u.second, ans, two_pow_len + 1);
                }
            }

        } else if (gcount > 0) {
            std::vector<std::vector<int>> to_merge;
            for (auto u : Map) {

                if (u.second.size() < 2) {
                    continue;
                } else if (u.second.size() == 2) {
                    if (is_two_files_equal(u.second[0], u.second[1], streams)) {
                        ans.push_back(u.second);
                    }
                } else {
                    to_merge.push_back(std::move(u.second));
                }
            }
            ans.insert(ans.end(), std::make_move_iterator(to_merge.begin()),
                       std::make_move_iterator(to_merge.end()));
        } else {
            ans.push_back(*fNumbers);
        }
    } else {
        size_t gcount = 0;
        QCryptographicHash sha(QCryptographicHash::Sha3_256);
        std::map<QByteArray, std::vector<int>> Map;

        for (int fNumber : *fNumbers) {

            auto index = static_cast<size_t>(fNumber);
            streams[index].read(buffer.data(), buffer.size());
            gcount = static_cast<size_t>(streams[index].gcount());

            sha.addData(buffer.data(), static_cast<int>(gcount));
            QByteArray res = sha.result();
            sha.reset();

            auto iter = Map.find(res);
            if (iter == Map.end()) {
                std::vector<int> v(1, fNumber);
                Map.insert({res, std::move(v)});
            } else {
                iter->second.push_back(fNumber);
            }
        }

        if (gcount == buffer.size()) {
            for (auto u : Map) {

                if (u.second.size() < 2) {
                    continue;
                } else if (u.second.size() == 2) {
                    if (is_two_files_equal(u.second[0], u.second[1], streams)) {
                        ans.push_back(u.second);
                    }
                } else {
                    binary_tree::build_final_tree(streams, &u.second, ans, two_pow_len + 1);
                }
            }

        } else if (gcount > 0) {
            std::vector<std::vector<int>> to_merge;
            for (auto u : Map) {

                if (u.second.size() < 2) {
                    continue;
                } else if (u.second.size() == 2) {
                    if (is_two_files_equal(u.second[0], u.second[1], streams)) {
                        ans.push_back(u.second);
                    }
                } else {
                    to_merge.push_back(std::move(u.second));
                }
            }
            ans.insert(ans.end(), std::make_move_iterator(to_merge.begin()),
                       std::make_move_iterator(to_merge.end()));
        } else {
            ans.push_back(*fNumbers);
        }


    }
}

bool binary_tree::is_two_files_equal(int file_1, int file_2, std::vector<std::ifstream> &streams) {
    auto cancellation_point = [thread = QThread::currentThread()]() {
        if (thread->isInterruptionRequested()) {
            throw cancellation_exception();
        }
    };

    cancellation_point();
    char value_1 = 0, value_2 = 0;
    while (streams[file_1] >> value_1) {

        streams[file_2] >> value_2;
        if (value_1 != value_2) {
            return false;
        }
    }
    return true;
}
