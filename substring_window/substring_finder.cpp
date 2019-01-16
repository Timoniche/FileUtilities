#include "substring_finder.h"
#include "Boyer_Moore.h"
#include "my_functions.h"

#include <QFileInfo>
#include <QDirIterator>
#include <QThread>
#include <array>
#include <fstream>

namespace {

	struct cancellation_exception : std::exception {
		const char *what() const noexcept override {
			return "Process interrupted";
		}
	};

}

void substring_finder::process_impl(std::string const& pattern, std::set<FilesTrigram, FilesTrigram::cmp> const * files_trigrams) {
	try {
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

		std::set<uint32_t> pattern_trigrams;
		FilesTrigram::splitStringToTrigram(pattern, pattern_trigrams);
		if (!files_trigrams->empty()) {
			for (auto file : *files_trigrams) {
				cancellation_point();
				auto *ANS = new std::vector< std::pair<int, std::vector<int>> >();
				std::vector<int> toMerge;
				if (!file.hasTrigrams(pattern_trigrams)) {
					//show no occurrences message <?>
					emit increase_bar(1);
					continue;
				}
				std::ifstream stream(file.get_string_name());
				std::string cur_line;
				QString name = file.get_qstring_name();
				if (!stream.is_open()) {
                    emit push_to_log(QString("Can't open ") + name);
					//throw std::runtime_error("Can't open one file");
				}
				int row = 0;
				while (!stream.eof()) {
					cancellation_point();
					row++;
					std::getline(stream, cur_line);
					if (cur_line.size() < pattern.size()) continue;
					Boyer_Moore finder(cur_line, pattern);
					toMerge = finder.occurrences;
					if (!finder.occurrences.empty()) {
						ANS->push_back({row, toMerge});
					}
				}
				emit increase_bar(1);
				emit update_tree({file.get_string_name(), *ANS});
			}
		}
		else {
			//show empty message
		}


	}
	catch (std::exception &ex) {
		emit error(ex.what());
	}
	emit finished();
}

void substring_finder::process(std::string const& pattern, std::set<FilesTrigram, FilesTrigram::cmp> const * files_trigrams) {
	try {
		//process_impl(pattern, _filesTrigrams);
		process_impl(pattern, files_trigrams);
	}
	catch (std::exception &ex) {
		emit error(ex.what());
	}

}
