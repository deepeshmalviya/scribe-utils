#include "common.h"
#include "scribe_wrapper.cpp"
#include "scribe_tail.h"

using std::string;
using boost::shared_ptr;

scribeTail::scribeTail(const std::string& filename, const int& sleep_var=1, const int& reopen_count=1)
	: filename(filename), sleep_var(sleep_var), reopen_count(reopen_count) {	
	boost::iostreams::file_source file(filename.c_str(), std::ios::in);
	in.push(file);	
	stat(filename.c_str(), &pstat);
	cstat = pstat;
}

void scribeTail::open() {	
	boost::iostreams::seek(in, std::ios_base::beg, BOOST_IOS::end);
}

void scribeTail::stillGood() {
	stat(filename.c_str(), &cstat);	
	if(cstat.st_ino != pstat.st_ino) {
		in.reset();
		boost::iostreams::file_source file(filename.c_str());
		in.push(file);
	} else if(cstat.st_size < pstat.st_size) {
		boost::iostreams::seek(in, 0, BOOST_IOS::beg);
	}	
}

bool scribeTail::getLine(std::string& message) {
	while(true) {
		stillGood();
		std::getline(in, message);
		if(in.eof()) {
			in.clear();
		}
		return true;		
	}
}

void scribeTail::pop() {
	in.pop();
}

int main(int argc, char **argv) {
	std::string host; 
	unsigned long port;
	unsigned int timeout; 
	bool debug;
	std::string log;
	std::string category; 
	std::string message;
	std::string filename;
	
	try {
		boost::program_options::options_description desc("Usage: scribe_tail [OPTION]");
		desc.add_options()
			("help", "Print help")
			("host", boost::program_options::value<std::string>(&host)->default_value("127.0.0.1"), "Scribe hostname")
			("port", boost::program_options::value<unsigned long>(&port)->default_value(1463), "Scribe port")
			("timeout", boost::program_options::value<unsigned int>(&timeout)->default_value(30), "timeout")
			("debug", boost::program_options::value<bool>(&debug)->default_value(false), "Enable debug mode")		
			("log", boost::program_options::value<std::string>(&log)->default_value("/tmp/scribewrapper.log"), "Path to scribe log file")		
			("category", boost::program_options::value<std::string>(&category)->default_value("default"), "Scribe category")		
			("file", boost::program_options::value<std::string>(&filename), "Log file to listen")		
		;
		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);
		
		if(vm.count("help")) {
			std::cout << desc << std::endl;
			return 1;
		}
		if (!vm.count("file")) {
		    std::cout << "Log file to listen is missing" << std::endl;
		    std::cout << std::endl << desc << std::endl;
		    return 1;
		}
	} catch(boost::program_options::error& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	scribeTail tail(filename);
	scribeWrapper client(host, port, timeout, category, debug, log);
	
	tail.open();
	while(tail.getLine(message)) {
		boost::algorithm::trim(message);
		if(message.empty()) {
			continue;
		}
		if(client.send(message) == -1) {
			std::cout << "Error! Please check logs" << std::endl;
			return 1;
		}		
	}	
	tail.pop();
	client.close();
	return 0; 
}
