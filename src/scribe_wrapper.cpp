#include "common.h"
#include "scribe_log.cpp"
#include "scribe_wrapper.h"
#include <sstream>

using std::string;
using boost::shared_ptr;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift;
using namespace scribe::thrift;
using namespace scribe;

scribeWrapper::scribeWrapper(const string& host="127.0.0.1", const unsigned int& port=1463, const unsigned int& timeout=30, const std::string& category="default", const bool& debug=false, const std::string& logfile="/tmp/scribewrapper.log") 
	: host(host), port(port), timeout(timeout), category(category), debug(debug), logfile(logfile) {
	time(&last_check);

	counters["msg"] = 0;
	counters["ok"] = 0;
	counters["try_again"] = 0;
	counters["failed"] = 0;	
	
	log = scribeLog::getInstance(logfile);	
	log->write("Starting Scribe Client for \"" + category + "\" category");

	if(debug) {
		log->write("Debug mode is enabled");
		mode = "BEGIN";		
		getUid(uid);
		starttime = getCurrentTime();
	}
	open();
}

scribeWrapper::~scribeWrapper() {
}

std::string scribeWrapper::getCurrentTime() {
	// Generate start time
	time_t rawtime;
	time(&rawtime);	
	return asctime(gmtime(&rawtime));
}

void scribeWrapper::getUid(std::string& uid) {
	std::stringstream out;
	srand(time(NULL));
	out << &uid << rand();
	out >> uid;
}

bool scribeWrapper::open() {
	try {
		socket = shared_ptr<TSocket>(new TSocket(host, port));
		if(!socket) {
			throw std::runtime_error("Error creating socket");
		}
		socket->setConnTimeout(timeout);
		socket->setRecvTimeout(timeout);
		socket->setSendTimeout(timeout);
		socket->setLinger(0, 0);

		transport = shared_ptr<TFramedTransport>(new TFramedTransport(socket));
		if(!transport) {
			throw std::runtime_error("Error creating transport");
		} 

		protocol = shared_ptr<TBinaryProtocol>(new TBinaryProtocol(transport));
		if(!transport) {
			throw std::runtime_error("Error creating protocol");
		} 

		resendClient = shared_ptr<scribe::thrift::scribeClient>(new scribeClient(protocol));
		protocol->setStrict(false, false);
		if(!resendClient) {
			throw std::runtime_error("Error creating client");
		}

		transport->open();			
	} catch(const TTransportException& ttx) {
		log->write("Scribe Client Initialization Failed due to TransportException");
		log->write(ttx.what());
		return false;
	} catch(const std::exception& stx) {
		log->write("Scribe Client Initialization Failed due to Exception");
		log->write(stx.what());
		return false;
	}
	log->write("Scribe Client Initialization Successful, listening for messages");
	return true;
}

bool scribeWrapper::isOpen() {
	return transport->isOpen();
}

std::string scribeWrapper::debug_message() {
	std::stringstream out;
	std::string msg;
	out << mode << ">>>" << uid << ">>>" << starttime << ">>>" << counters["msg"] << ">>>" << getCurrentTime() << ">>>";	
	out >> msg;
	msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
	return msg;
}

int scribeWrapper::send(const std::string& message) {
	if(!isOpen()) {
		if(!open()) {
			return (CONN_FATAL);
		}
	}
	periodicFlush();	

	counters["msg"]++;

	LogEntry entry;
	std::string msg;
	entry.category = category;
	if(debug) {
		msg += debug_message();
	}
	msg += message + "\n"; //Special handling to include new lines
	entry.message = msg;
	std::vector<LogEntry> messages;
	messages.push_back(entry);
	ResultCode result = TRY_LATER;
	try {
		result = resendClient->Log(messages);
		if(debug) { mode = "STEP"; }
		if(result == OK) {
			counters["ok"]++;
			return (CONN_OK);
		} else if(result == TRY_LATER) {
			counters["try_again"]++;
			return (CONN_TRANSIENT);
		}
	} catch(const TTransportException& ttx) {
		log->write("Message sending failed due to TransportException");
		log->write(ttx.what());
	} catch(std::exception& stx) {
		log->write("Message sending failed due to Exception");
		log->write(stx.what());
	}
	counters["failed"]++;
	return (CONN_FATAL);
}

void scribeWrapper::periodicFlush() {
	time_t current_check;
	time(&current_check);
	if(current_check - last_check > (PERIODIC_FLUSH)) {
		flushCounters();
		last_check = current_check;
	}
}

void scribeWrapper::flushCounters() {
	std::stringstream out;
	out << "Category: " << category << "\tCounters - msg: " << counters["msg"] << "\tok: " << counters["ok"] << "\ttry_again: " << counters["try_again"] << "\tunknown: " << counters["failed"];
	log->write(out.str());
}

void scribeWrapper::close() {
	try {
		if(debug) {
			mode = "END";
			std::stringstream out;
			out << "msg=\"" << counters["msg"] << "\";;;ok=\"" << counters["ok"] << "\";;;try_again=\"" << counters["try_again"] << "\";;;unknown=\"" << counters["failed"] << "\"";
			send(out.str());
		}
		transport->close();
	} catch(const TTransportException& ttx) {
		log->write(ttx.what());
	}
	flushCounters();
	log->write("Closing Scribe Client...");
}
