#ifndef POP3ENGINE_H
#define POP3ENGINE_H
#include "_Pop3Engine.h"

class Pop3Engine
{
    enum RESPONSE {CONNECTION_CHECK,USER_CHECK,PASS_CHECK,QUIT_CHECK,STAT_CHECK,LIST_CHECK,RETR_CHECK};
    class Pop3Socket
    {
    public:
        Pop3Socket();
        bool  Create();
        bool  Connect(const char* lpszHost, int nPort = 110);
        bool  Send(char* lpszBuf, int nBuf);
        void  Close();
        int   Receive(char* lpszBuf, int nBuf);
    protected:
        bool   Connect(const struct sockaddr* lpSockAddr, int nSockAddrLen);
        SOCKET m_hSocket;
    };
    bool ReadResponseOK(RESPONSE index);
    bool Disconnect();
public:
    Pop3Engine();
    bool StartConnectionServer(const char* pszHostName, int nPort=110);
    bool SendUsername(const string& sName);
    bool SendPassword(const string& sPass);
    bool GetUnreadMessageNumber();
    bool GetUnreadMessageList();
    bool GetSpecificMessage(int index);
    bool IsErrorEmpty() const;
    string GetErrorMessage() const;
private:
    Pop3Socket m_POP3;
    string m_ErrorMessage;
public:
    virtual ~Pop3Engine();
};

#endif // POP3ENGINE_H
