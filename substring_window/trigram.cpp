#include "trigram.h"

#include <fstream>

FilesTrigram::FilesTrigram(std::string file_name) : _file_name(file_name) {
	std::ifstream stream(_file_name);
	if (!stream.is_open()) {
		throw std::runtime_error("Can't open one file");
	}
	std::string cur_line;
	while (!stream.eof()) {
		std::getline(stream, cur_line);
		if (cur_line.size() < 3) {
			continue;
		}
		else {
			if (cur_line.size() > BUFFER_LINE_SIZE) {
				//throw exc
				//increase BUFFER_LINE_SIZE in preferences
				//some data could be lost - continue?
			} else {
				splitStringToTrigram(cur_line, trigrams);
			}
		}
	}
}

FilesTrigram::~FilesTrigram() = default;

//Pre: size [3; BUFFER_LINE_SIZE]
void FilesTrigram::splitStringToTrigram(std::string s, std::set<uint32_t>& trSet) {
	//string to utf8?
	uint32_t buff[BUFFER_LINE_SIZE];
	//QString qs = QString::fromStdString(s);
	//qs.toUtf8().constData();
	for (int i = 0; i < s.length(); i++) {
		buff[i] = static_cast<uint8_t>(s[i]);
	}
	//now without validation
	uint32_t curTrigram = (buff[0] << 8) | (buff[1]);
	for (int i = 2; i < s.length(); i++) {
		curTrigram &= 0xFFFF; // clear first (head) 16 bits
		curTrigram <<= 8;
		curTrigram |= buff[i];
		trSet.insert(curTrigram);
	}
}

bool FilesTrigram::hasTrigrams(std::set<uint32_t> text_trigrams) {
	for (auto u : text_trigrams) {
		if (trigrams.find(u) == trigrams.end()) {
			return false;
		}
	}
	return true;
}