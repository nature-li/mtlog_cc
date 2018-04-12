// log.h

#ifndef AD_LOG_H_
#define AD_LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <mutex>
#include <unistd.h>
#include <fstream>
#include "stream.h"
#include "ad_menu.h"

namespace mtad {
namespace adlog {

class Record;
class Filter;
class Backend;
class Sink;

class AdLog {
public:
	AdLog();
	virtual ~AdLog();

	static AdLog* Instance();

	bool Init(LogEnv env, const std::string& target,
			const std::string& file_name, long file_size = 100 * 1024 * 1024,
			int queue_size = 100 * 1024, int max_file_count = -1);

	bool Start(std::string file_name, std::string dir, long max_file_size,
			int max_queue_size,
			const std::map<int, std::string>& file_level_name,
			int max_file_count);

	void Stop();

	void SetMinLevel(LogSev level);

	bool Filt(LogSev level);

	bool DirectPushLog(Record* record);

	void SetAsync(bool async);

private:
	void DeleteExpireLog();

private:
	static AdLog* instance_;
	static std::mutex mutex_;
	std::shared_ptr<Filter> filter_;
	std::shared_ptr<Backend> writter_;
	std::shared_ptr<Sink> sink_;
	int running_flag_;
	bool async_;

public:
	static LogEnv env_;
};

class Wrapper: public Stream {
public:
	Wrapper(LogSev level, const char* file, int line, const char* func,
			const std::string& pv_id, const std::string& keyword);

	~Wrapper();

	Stream& stream();

private:
	void Prefix(LogSev level, const char* file, int line, const char* func,
			const std::string& pv_id, const std::string& keyword);

private:
	Record* record_;
};

}
}

#define LOG(level, PV_ID, KEYWORD) \
  if (true) \
    mtad::adlog::Wrapper(mtad::adlog::level, __FILE__, __LINE__, __FUNCTION__, PV_ID, KEYWORD).stream()

// Must specify: pvid keyword
#define LOG_PVWD_TRACE(PV_ID, KEYWORD) LOG(trace, PV_ID, KEYWORD)
#define LOG_PVWD_DEBUG(PV_ID, KEYWORD) LOG(debug, PV_ID, KEYWORD)
#define LOG_PVWD_INFO(PV_ID, KEYWORD) LOG(info, PV_ID, KEYWORD)
#define LOG_PVWD_WARN(PV_ID, KEYWORD) LOG(warn, PV_ID, KEYWORD)
#define LOG_PVWD_ERROR(PV_ID, KEYWORD) LOG(error, PV_ID, KEYWORD)
#define LOG_PVWD_FATAL(PV_ID, KEYWORD) LOG(fatal, PV_ID, KEYWORD)
#define LOG_PVWD_REPORT(PV_ID, KEYWORD) LOG(report, PV_ID, KEYWORD)

// Must specify: pvid
#define LOG_PVID_TRACE(PV_ID) LOG(trace, PV_ID, "normal")
#define LOG_PVID_DEBUG(PV_ID) LOG(debug, PV_ID, "normal")
#define LOG_PVID_INFO(PV_ID) LOG(info, PV_ID, "normal")
#define LOG_PVID_WARN(PV_ID) LOG(warn, PV_ID, "normal")
#define LOG_PVID_ERROR(PV_ID) LOG(error, PV_ID, "normal")
#define LOG_PVID_FATAL(PV_ID) LOG(fatal, PV_ID, "normal")
#define LOG_PVID_REPORT(PV_ID) LOG(report, PV_ID, "normal")

// Sepcify nothing
#define LOG_TRACE LOG(trace, "", "normal")
#define LOG_DEBUG LOG(debug, "", "normal")
#define LOG_INFO LOG(info, "", "normal")
#define LOG_WARN LOG(warn, "", "normal")
#define LOG_ERROR LOG(error, "", "normal")
#define LOG_FATAL LOG(fatal, "", "normal")
#define LOG_REPORT LOG(report, "", "normal")

// Set log level
#define SET_LOG_LEVEL(LVL) mtad::adlog::AdLog::Instance()->SetMinLevel(mtad::adlog::LVL);

// Set log async
#define SET_LOG_ASYNC(OPTION) mtad::adlog::AdLog::Instance()->SetAsync(OPTION);

// Init log
#define MT_LOG_INIT(ENV, TARGET, ARGS...) mtad::adlog::AdLog::Instance()->Init(mtad::adlog::ENV, TARGET, ARGS);

// Stop log
#define MT_LOG_STOP() mtad::adlog::AdLog::Instance()->Stop();

// Init log

#endif /* AD_LOG_H_ */
