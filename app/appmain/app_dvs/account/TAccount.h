#ifndef __TACCOUNT_H__
#define __TACCOUNT_H__

class TAccount
{
public:
    static TAccount* getInstance();
    bool    auth_check(const char* name, const char* pwd);
private:
    TAccount();
    ~TAccount();
private:
    static TAccount*    m_instance;
};

#endif