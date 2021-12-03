#include "tlog.h"

TLog* TLog::m_instance = NULL;

static void _log_writedumpCallBack(const char* data, int size)
{
    LOG(INFO) << "[writedumpCallBack]" << data << endl;
}


TLog* TLog::getInstance()
{
    if(!m_instance)
    {
        m_instance = new TLog();
    }

    return m_instance;
}

TLog::TLog()
{


}

TLog::~TLog()
{

}

bool TLog::log_init()
{
    const char* parameters = "APPMAIN";

    FLAGS_logtostderr = true;
    FLAGS_stop_logging_if_full_disk = true;
    FLAGS_alsologtostderr = true;

    google::InitGoogleLogging(parameters);

     //google::LogToStderr();
    google::InstallFailureSignalHandler();
    google::InstallFailureWriter(_log_writedumpCallBack);
    //google::InstallFailureFunction();
    //SetStderrLogging(GLOG_INFO);

    return true;
}