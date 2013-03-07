#ifndef SENDEMAIL_H
#define SENDEMAIL_H
#include "SmtpEngine.h"

class SendEmail:private SmtpEngine
{
    void InitVariable();
public:
    void IsDisconnect(bool bIsClose);
    void Verification(const string& sUser,const string& sPass);
    void AddSubject(const string& sText);
    void AddContent(const string& sText);
    void AddAttachment(const string& sPath);
    void AddReceiver(const string& sUser);
    void AddCopyTo(const string& sCopy);
    bool SendEmailMessage();
    bool IsExistError();
    char* GetLastError();
private:
    string m_sLastError;
    string m_sUser;
    string m_sPass;
    string m_sSubject;
    string m_sContent;
    list<string> m_listsFile;
    list<string> m_listsReceiver;
    list<string> m_listsCopy;
};

#endif // SENDEMAIL_H
