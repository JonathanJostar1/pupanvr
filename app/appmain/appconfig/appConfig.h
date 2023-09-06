#ifndef APPCONFIG_H
#define APPCONFIG_H

#include<iostream>

#include<fstream>
#include<map>
#include<string>


using namespace std;

class AppConfig
{
public:
    typedef enum{
        MODE_WRITE = 0,
        MODE_READ = 1,
    }FunConfigMode;
    /*关键字内容解析过程*/
    typedef bool (*pFunConfigParseProcess)(AppConfig* appConfig, const string& key, string &value, int mode);
    AppConfig(const string& fileName);
    virtual ~AppConfig();
    /*save config to file*/
    virtual bool save();
    /*load config from file!*/
    virtual bool load();
protected:
    /*config default*/
    virtual void AppDefaultConfig() = 0;

    virtual bool    registerConfig(const string& key, pFunConfigParseProcess funConfigParseProcess);
    /*通用的字符串值处理过程*/
    virtual bool    stringValueProcess(AppConfig* appConfig, const string& key, string &value, int mode);
    /*注册通用的字符串值处理*/
    virtual bool    registerStringValueProcess(const string& key, string* value);

    static const string AppConfigPath;
protected:
    virtual bool    loadFromFile();
    virtual bool    saveToFile();
private:
    /*配置文件名称*/
    string      m_fileName;
    /*字段对应的处理过程*/
    map<string, pFunConfigParseProcess> m_keyParseFunList;
public:
    /*通用的一个"字段=值"处理记录,这个QMAP的值记录具体字段的地址，注意释放的问题*/
    map<string, string*> m_keyValueRecordList;
};

#endif // APPCONFIG_H
