#pragma once

#include <string>
#include <set>
#include <QString>
#include <QFileInfo>

class FilesTrigram {

public:

	FilesTrigram();
    FilesTrigram(std::string const & file_name);
    FilesTrigram(std::string const & file_name, std::atomic_bool const* indexing);
	~FilesTrigram();

public:
    static const size_t BUFFER_LINE_SIZE = 1000;

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


	bool hasTrigrams(std::set<uint32_t> const & text_trigrams);

	static void splitStringToTrigram(std::string const & s, std::set<uint32_t>& trSet);

public:
	bool isValid = true;

private:
	std::string _file_name;
	std::set<uint32_t> trigrams;
    std::atomic_bool const* _indexing;
	
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
