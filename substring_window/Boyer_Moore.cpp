#include "Boyer_Moore.h"
#include <algorithm>

//todo: compare with std boyer_moore_searcher
Boyer_Moore::Boyer_Moore(const std::string &text, const std::string &pattern) : text(text),
	pattern(pattern) {
	n = static_cast<int>(text.size());
	m = static_cast<int>(pattern.size());
	fillSuffShift();
	findOccurrences();
}

void Boyer_Moore::fillSuffShift() {
	suffShift.assign(m + 1, m);
	z.assign(m, 0);

	//z function initialization for reverse pattern
	//first index consider 0
	for (int j = 1, maxZidx = 0, maxZ = 0; j < m; ++j) {
		if (j <= maxZ) {
			z[j] = std::min(maxZ - j + 1, z[j - maxZidx]);
		}
		while (j + z[j] < m && pattern[m - 1 - z[j]] ==
			pattern[m - 1 - (j + z[j])]) {
			z[j]++;
		}
		if (j + z[j] - 1 > maxZ) {
			maxZidx = j;
			maxZ = j + z[j] - 1;
		}
	}
	for (int j = m - 1; j > 0; --j) {
		suffShift[m - z[j]] = j;
	}
	for (int j = 1, r = 0; j <= m - 1; ++j) {
		if (j + z[j] == m) {
			for (; r <= j; r++) {
				if (suffShift[r] == m) {
					suffShift[r] = j;
				}
			}
		}
	}
}

void Boyer_Moore::findOccurrences() {
	int bound = 0;
	int j = 0;
	for (int i = 0; i <= n - m; i += suffShift[j + 1]) {
		for (j = m - 1; j >= bound && pattern[j] == text[i + j]; j--);
		if (j < bound) {
			occurrences.push_back(i);
			bound = m - suffShift[0];
			j = -1;
		}
		else {
			bound = 0;
		}
	}
}