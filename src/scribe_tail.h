#ifndef SCRIBE_TAIL_H
#define SCRIBE_TAIL_H 

#include "common.h"
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>
#include <sys/stat.h>

class scribeTail {
	public:
		scribeTail(const std::string& filename, const int& sleep_var, const int& reopen_count);
		virtual ~scribeTail(){};	
		void open();
		bool getLine(std::string& message);	
		void pop();
	protected:
		void stillGood();
		struct stat pstat;
		struct stat cstat;
		std::string filename;
		int sleep_var;
		int reopen_count;
		boost::iostreams::filtering_istream in;
};

#endif
