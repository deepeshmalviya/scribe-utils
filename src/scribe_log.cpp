#include <iostream>
#include "common.h"
#include "scribe_log.h"

scribeLog* scribeLog::instance = NULL;

scribeLog::scribeLog(const std::string& filename)
	: filename(filename) {
	initialize();
}

scribeLog::~scribeLog() {
	out.close();
}

scribeLog* scribeLog::getInstance(const std::string& filename) {
	if(instance == NULL) {
		instance = new scribeLog(filename);
	}
	return instance;
}

void scribeLog::initialize() {
	out.open(filename.c_str(), std::ofstream::app);
}

void scribeLog::write(const std::string& message) {
	time_t rawtime;
	time(&rawtime);
	std::string msg = asctime(gmtime(&rawtime));
	msg += "\t" + message;
	msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
	out << msg.c_str() << std::endl;
}
