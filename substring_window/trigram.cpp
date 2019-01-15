#include "trigram.h"

#include <fstream>

FilesTrigram::FilesTrigram() = default;

FilesTrigram::FilesTrigram(std::string const & file_name) :
    _file_name(file_name)
{
    std::ifstream stream(_file_name);
    if (!stream.is_open()) {
        isValid = false;
        //cancel <-- throw std::runtime_error("Can't open " + _file_name);
        //todo: --> throw to log from std::function;
    }
    else {
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
                }
                else {
                    splitStringToTrigram(cur_line, trigrams);
                }
            }
        }
    }
}

FilesTrigram::FilesTrigram(std::string const & file_name, std::atomic_bool const* indexing) :
    _file_name(file_name),
    _indexing(indexing)
{
	std::ifstream stream(_file_name);
	if (!stream.is_open()) {
		isValid = false;
		//cancel <-- throw std::runtime_error("Can't open " + _file_name);
		//todo: --> throw to log from std::function;
	}
	else {
		std::string cur_line;
		while (!stream.eof()) {
            if (!_indexing) {
                isValid = false;
                return;
            }
			std::getline(stream, cur_line);
			if (cur_line.size() < 3) {
				continue;
			}
			else {
				if (cur_line.size() > BUFFER_LINE_SIZE) {
					//throw exc
					//increase BUFFER_LINE_SIZE in preferences
					//some data could be lost - continue?
				}
				else {
					splitStringToTrigram(cur_line, trigrams);
				}
			}
		}
	}
}

FilesTrigram::~FilesTrigram() = default;

//Pre: size [3; BUFFER_LINE_SIZE]
void FilesTrigram::splitStringToTrigram(std::string const & s, std::set<uint32_t>& trSet) {
	//string to utf8?
	//QString qs = QString::fromStdString(s);
	//qs.toUtf8().constData();
	//now without validation
    uint32_t curTrigram = static_cast<uint32_t>(static_cast<uint8_t>(s[0]) << 8) | (static_cast<uint8_t>(s[1]));
	for (size_t i = 2; i < s.length(); i++) {
		curTrigram &= 0xFFFF; // clear first (head) 16 bits
		curTrigram <<= 8;
		curTrigram |= static_cast<uint8_t>(s[i]);
		trSet.insert(curTrigram);
	}
}

bool FilesTrigram::hasTrigrams(std::set<uint32_t> const & text_trigrams) {
	for (auto u : text_trigrams) {
		if (trigrams.find(u) == trigrams.end()) {
			return false;
		}
	}
	return true;
}
