// log.cpp

#include <unistd.h>
#include <stdlib.h>
#include "ad_log.h"
#include "record.h"
#include "backend.h"
#include "sink.h"
#include "filter.h"
#include "tool.h"

namespace mtad {
namespace adlog {

AdLog* AdLog::instance_ = nullptr;
std::mutex AdLog::mutex_;
LogEnv AdLog::env_ = invalid_env;

static void StopLogger(int status, void *argv) {
	AdLog::Instance()->Stop();
}

AdLog::AdLog() :
		running_flag_(0), async_(true) {
	// TODO Auto-generated constructor stub
}

AdLog::~AdLog() {
	// TODO Auto-generated destructor stub
}

AdLog* AdLog::Instance() {
	if (!instance_) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (!instance_) {
			instance_ = new AdLog();
		}
	}

	return instance_;
}

bool AdLog::Init(LogEnv env, const std::string& target,
		const std::string& file_name, long file_size, int queue_size,
		int max_file_count, bool async) {
	env_ = env;
	async_ = async;
	std::map<int, std::string> file_level_name;
	file_level_name[trace] = file_name + ".process.log";
	file_level_name[debug] = file_name + ".process.log";
	file_level_name[info] = file_name + ".process.log";
	file_level_name[warn] = file_name + ".process.log";
	file_level_name[error] = file_name + ".process.log";
	file_level_name[fatal] = file_name + ".process.log";
	file_level_name[report] = file_name + ".report.log";

	return Start(file_name, target, file_size, queue_size, file_level_name,
			max_file_count);
}

bool AdLog::Start(std::string file_name, std::string dir, long max_file_size,
		int max_queue_size, const std::map<int, std::string>& file_level_name,
		int max_file_count) {
	// Expand to full dir path
	if (dir.find("/") != 0) {
		char buf[1024];
		getcwd(buf, sizeof(buf));
		std::string full_path = buf;
		dir = full_path + "/" + dir;
	}

	filter_ = std::make_shared<Filter>();
	writter_ = std::make_shared<Backend>(dir, max_file_size, max_file_count,
			file_level_name);
	writter_->Init();
	sink_ = std::make_shared<Sink>(writter_.get(), max_queue_size, async_);
	sink_->Start();
	running_flag_ = 1;

	// Set exit function
	on_exit(StopLogger, NULL);
	return true;
}

void AdLog::Stop() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (running_flag_ == 0) {
		return;
	}
	running_flag_ = 0;

	if (sink_) {
		sink_->Stop();
	}

	if (writter_) {
		writter_->Stop();
	}
}

bool AdLog::DirectPushLog(Record* record) {
	sink_->PushLog(record);
	return true;
}

void AdLog::SetMinLevel(LogSev level) {
	filter_->SetLevel(level);
}

bool AdLog::Filt(LogSev level) {
	return filter_->Filt(level);
}

Wrapper::Wrapper(LogSev level, const char* file, int line, const char* func,
		const std::string& pv_id, const std::string& keyword) {
	// Check log level
	if (AdLog::Instance()->Filt(level)) {
		record_ = new Record();
		record_->level_ = level;
		Prefix(level, file, line, func, pv_id, keyword);
	} else {
		record_ = nullptr;
	}
}

Wrapper::~Wrapper() {
	if (record_) {
		record_->operator<<(Record::log_tail_);
		AdLog::Instance()->DirectPushLog(record_);
	}
}

Stream& Wrapper::stream() {
	if (record_) {
		return *record_;
	} else {
		return *this;
	}
}

void Wrapper::Prefix(LogSev level, const char* file, int line, const char* func,
		const std::string& pv_id, const std::string& keyword) {
	std::string micro_string = Tool::GetTimeMicroString();
	std::string& sev_string = Tool::LogSevToString(level);
	std::string& thread_id = Tool::GetThreadId();
	std::string& env_string = Tool::LogEnvToString(AdLog::env_);

	char prefix[1024];
	snprintf(prefix, 1024, Record::log_format_.c_str(), micro_string.c_str(),
			sev_string.c_str(), thread_id.c_str(), file, line, func,
			env_string.c_str(), pv_id.c_str(), keyword.c_str());
	record_->operator<<(prefix);
}

}
}
