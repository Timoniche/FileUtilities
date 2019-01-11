#ifndef FILECOPIES_BINARY_TREE_H
#define FILECOPIES_BINARY_TREE_H


#include <vector>
#include "copies_finder.h"

#include <fstream>
#include <map>
#include <memory>
#include <QByteArray>

struct binary_tree {

	binary_tree(copies_finder* ref) : worker_from(ref) {}

    friend copies_finder;

    bool is_two_files_equal(int file_1, int file_2, std::vector<std::ifstream> &streams);

    void build_final_tree(std::vector<std::ifstream> &streams, std::vector<int> *fNumbers,
                          std::vector<std::vector<int>> &ans, unsigned long two_pow_len);

private:
	copies_finder* worker_from;
};

#endif //FILECOPIES_BINARY_TREE_H
