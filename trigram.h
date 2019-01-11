#pragma once

#include <string>
#include <set>
#include <QString>
#include <QFileInfo>

class FilesTrigram {

public:

	FilesTrigram(std::string file_name);
	~FilesTrigram();

public:
	static const size_t BUFFER_LINE_SIZE = 1'000;

	struct cmp {
		bool operator()(FilesTrigram const& lhs, FilesTrigram const& rhs) const {
			size_t size_lhs = QFileInfo(lhs.get_qstring_name()).size();
			size_t size_rhs = QFileInfo(rhs.get_qstring_name()).size();
			if (size_lhs == size_rhs) {
				return lhs.get_string_name() < rhs.get_string_name();
			} else {
				return size_lhs < size_rhs;
			}

		}
	};


	bool hasTrigrams(std::set<uint32_t> text_trigrams);

	static void splitStringToTrigram(std::string s, std::set<uint32_t>& trSet);

private:
	std::string _file_name;
	std::set<uint32_t> trigrams;

public:
	std::string get_string_name() {
		return _file_name;
	}

	std::string const get_string_name() const {
		return _file_name;
	}

	QString get_qstring_name() {
		return QString::fromStdString(_file_name);
	}

	QString const get_qstring_name() const {
		return QString::fromStdString(_file_name);
	}
};