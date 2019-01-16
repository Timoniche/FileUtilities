#ifndef BOYER_MOORE_BOYER_MOORE_H
#define BOYER_MOORE_BOYER_MOORE_H

#include "substring_finder.h"

#include <string>
#include <vector>

class Boyer_Moore {

public:
    Boyer_Moore(const std::string &text, const std::string &pattern);

private:
    std::string text;
    int n;
    std::string pattern;
    int m;

public:
    std::vector<int> occurrences;
private:
    std::vector<int> suffShift;
    std::vector<int> z;
private:
    void fillSuffShift();

    void findOccurrences();
};

#endif //BOYER_MOORE_BOYER_MOORE_H
