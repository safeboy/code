#ifndef SMTPENGINE_H
#define SMTPENGINE_H
#include "_SmtpEngine.h"


class SmtpEngine:private CMemory
{
    class SmtpSocket
    {
    public:
        SmtpSocket();
        bool  Create();
        bool  Connect(const char* lpszHost, int nPort = 25);
        bool  Send(char* lpszBuf, int nBuf);
        void  Close();
        int   Receive(char* lpszBuf, int nBuf);
    protected:
        bool   Connect(const struct sockaddr* lpSockAddr, int nSockAddrLen);
        SOCKET m_hSocket;
    };
    bool ReadResponse(const char* pszCode);
    bool SendVerify250Response(const string& sData);
    string GetMailHeader(const string& sSubject);
public:
    enum RECIPIENT_TYPE { TO, CC, BCC };
    SmtpEngine();
    bool Disconnect();
    bool SendHandShake(const char* pszHostName, int nPort=25);
    bool SendPrivateKey(const char* username,const char* password);
    bool SendMailFrom(const char* sFriendly,const char* sAddress);
    void AddRcptRecipient(const string& sFriendly, const string& sAddress,RECIPIENT_TYPE RecipientType = TO);
    bool SendRcptRecipient();
    bool SendMailHeader(const string& sSubject,bool IsAnnex);
    bool SendMessage(const string& sMsg);
    bool SendAttachment(const string& sPath);
    bool SendAttachmentFinally(bool IsAnnex);
    bool SendComplete();
private:
    SmtpSocket m_SMTP;
    string m_sFriendly;
    string m_sAddress;
    map<string,string> m_mapTO;
    map<string,string> m_mapCC;
    map<string,string> m_mapBCC;
public:
    virtual ~SmtpEngine();
};

#endif // SMTPENGINE_H
