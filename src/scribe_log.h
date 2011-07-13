#ifndef SCRIBE_LOG_H
#define SCRIBE_LOG_H

#include "common.h"
#include <fstream>

class scribeLog {
	public:
		static scribeLog* getInstance(const std::string& filename);
		void write(const std::string& message);
		~scribeLog();
	private:
		static scribeLog* instance;
		scribeLog(const std::string& filename);
		void initialize();
		std::ofstream out;
		const std::string filename;
};

#endif
