#ifndef SCRIBE_WRAPPER_H
#define SCRIBE_WRAPPER_H

#include "common.h"

#define CONN_FATAL (-1)
#define CONN_OK (0)
#define CONN_TRANSIENT (1)
#define PERIODIC_FLUSH (5*60)

class scribeWrapper {
	public:
		scribeWrapper(const std::string& host, const unsigned int& port, const unsigned int& timeout, const std::string& category, const bool& debug, const std::string& logfile);
		virtual ~scribeWrapper();
		int send(const std::string& message);
		bool open();
		bool isOpen();
		void flushCounters();
		void close();
	private:
		std::string getCurrentTime();
		void getUid(std::string& uid);
		std::string debug_message();
		void periodicFlush();
	protected:
		std::string host;
		unsigned int port;
		unsigned int timeout;	
		std::string category;
		time_t last_check;

		bool debug;
		std::string uid;
		std::string starttime;
		std::string mode;
		scribeLog* log;
		const std::string logfile;
	
		std::map<std::string, unsigned long> counters;

		boost::shared_ptr<apache::thrift::transport::TSocket> socket;
		boost::shared_ptr<apache::thrift::transport::TFramedTransport> transport;
		boost::shared_ptr<apache::thrift::protocol::TBinaryProtocol> protocol;
		boost::shared_ptr<scribe::thrift::scribeClient> resendClient;
};

#endif
