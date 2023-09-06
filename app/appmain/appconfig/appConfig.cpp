#include <string>
#include "tlog.h"
#include "appConfig.h"




const string AppConfig::AppConfigPath = "/nfsroot/hi3536c/config";

AppConfig::AppConfig(const string& fileName)
{
    m_fileName = AppConfigPath + "/" + fileName;
}

AppConfig::~AppConfig()
{

}

bool AppConfig::save()
{
    bool ret = false;
    ret = this->saveToFile();

    return ret;      
}

bool AppConfig::load()
{
    bool ret = false;
    ret = this->loadFromFile();

    return ret;
}


bool AppConfig::loadFromFile()
{
    char buff[2048];
    fstream stream;

    string tmpStr = "";
    string key,value;
    unsigned int pos;

    pFunConfigParseProcess funConfigParseProcess;

    stream.open(m_fileName.c_str(), ios::in);

    if(!stream.is_open())
    {
        return false;
    }

    while(!stream.eof())
    {
        stream.getline(buff, sizeof(buff));
        if(stream.bad())
        {
            break;
        }

        tmpStr = buff;

        tmpStr.erase(0, tmpStr.find_first_not_of(" "));
        tmpStr.erase(tmpStr.find_last_not_of(" ") + 1);
        tmpStr.erase(0, tmpStr.find_first_not_of("\r\n"));
        tmpStr.erase(tmpStr.find_last_not_of("\r\n") + 1);
        if(tmpStr.empty())
        {
            //LOG(INFO) << "buff " << buff << endl;
            continue;
        }
        if(tmpStr[0] == '#')
        {
            LOG(INFO) << "buff " << buff << endl;
            continue;
        }

        pos = tmpStr.find('=', 0);
        if(pos == string::npos)
        {
            LOG(ERROR) << "AppConfig parse failure! fileName:" << m_fileName << " context:" << tmpStr << endl;
            continue;
        }

        key = tmpStr.substr(0, pos);
        value = tmpStr.substr(pos + 1, tmpStr.size() - pos - 1);
        if(key.empty())
        {
            continue;
        }

        string qkey = key, qvalue = value;

        map<string, pFunConfigParseProcess>::iterator iter = m_keyParseFunList.find(qkey);

        if(iter != m_keyParseFunList.end())
        {
            funConfigParseProcess = iter->second;
            if(funConfigParseProcess)
            {
                funConfigParseProcess(this, qkey, qvalue, AppConfig::MODE_READ);
                LOG(INFO) << "Load config " << key << "=" << value << endl;
            }
        }else{

            if(m_keyValueRecordList.find(qkey) != m_keyValueRecordList.end())
            {
                stringValueProcess(this, qkey, qvalue, AppConfig::MODE_READ);
            }
        }
    }

    stream.close();
    return true;
}

bool AppConfig::saveToFile()
{
    char buff[2048];
    fstream stream;
    string key = "",value = "";
    pFunConfigParseProcess funConfigParseProcess;

    stream.open(m_fileName.c_str(), ios::out);
    if(!stream.is_open())
    {
        LOG(ERROR) << "AppConfig::saveToFile open file:" << m_fileName << " failure!!!\n";
        return false;
    }

    map<string, string*>::iterator iter1;
    for(iter1 = m_keyValueRecordList.begin(); iter1 != m_keyValueRecordList.end(); iter1++)
    {
        key = iter1->first;
        if(m_keyParseFunList.find(key) != m_keyParseFunList.end())   /*判断一下指定的处理过程中有没有相同字段的处理，如果有，用指定的过程处理*/
        {
            continue;
        }

        string* V = m_keyValueRecordList[key];
        snprintf(buff, sizeof(buff), "%s=%s\r\n", key.c_str(), V->c_str());
        stream.write(buff, strlen(buff));
    }

    map<string, pFunConfigParseProcess>::iterator iter;
    for(iter = m_keyParseFunList.begin(); iter != m_keyParseFunList.end(); iter++)
    {
        key = iter->first;
        value = "";
        funConfigParseProcess = iter->second;
        if(funConfigParseProcess)
        {
            if(funConfigParseProcess(this, key, value, AppConfig::MODE_WRITE))
            {
                snprintf(buff, sizeof(buff), "%s=%s\r\n", key.c_str(), value.c_str());
                stream.write(buff, strlen(buff));
                LOG(INFO) << "Save config " << key << "=" << value << endl;
            }
        }
    }

    stream.close();
    return true;
}

bool AppConfig::stringValueProcess(AppConfig* appConfig, const string& key, string &value, int mode)
{
    string *v = NULL;
    if(m_keyValueRecordList.find(key) == m_keyValueRecordList.end())
    {
        return false;
    }

    v = m_keyValueRecordList[key];
    if(mode == AppConfig::MODE_READ)
    {
        *v = value;
    }else{
        value = *v;
    }

    return true;
}

bool AppConfig::registerConfig(const string& key, AppConfig::pFunConfigParseProcess funConfigParseProcess)
{
    map<string, pFunConfigParseProcess>::iterator iter = m_keyParseFunList.find(key);
    if(m_keyParseFunList.end() != iter)
    {
        LOG(ERROR)<<"AppConfig::registerConfig repeat registerConfig key:" << key << endl;
        return false;
    }

    m_keyParseFunList[key] = funConfigParseProcess;
    return true;
}

bool AppConfig::registerStringValueProcess(const string& key, string* value)
{
    m_keyValueRecordList[key] = value;
    return true;
}

