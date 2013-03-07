#include "SendEmail.h"
void SendEmail::InitVariable()
{
    this->m_sSubject.clear();
    this->m_sContent.clear();
    this->m_listsFile.clear();
    this->m_listsReceiver.clear();
    this->m_listsCopy.clear();
}
void SendEmail::IsDisconnect(bool bIsClose)
{
    if(bIsClose)
    {
        this->Disconnect();
    }
}
void SendEmail::Verification(const string& sUser,const string& sPass)
{
    this->m_sLastError.clear();
    if(sUser.length()<=0 || sPass.length()<=0)
    {
        this->m_sLastError.clear();
        this->m_sLastError += "NAME Or PASS no is empty.";
        return;
    }
    else
    {
        this->m_sUser.clear();
        this->m_sPass.clear();
        this->m_sUser += sUser;
        this->m_sPass += sPass;
        if(sUser.find("@163.com")!=string::npos)
        {
            if(!this->SendHandShake("smtp.163.com"))
            {
                this->m_sLastError.clear();
                this->m_sLastError += "connect e_mail server failed.";
                return;
            }
        }
        if(!this->SendPrivateKey(sUser.c_str(),sPass.c_str()))
        {
            this->m_sLastError.clear();
            this->m_sLastError += "NAME or PASS verification failed.";
            return;
        }
    }
}
void SendEmail::AddSubject(const string& sText)
{
    m_sSubject.clear();
    m_sSubject += sText;
}
void SendEmail::AddContent(const string& sText)
{
    m_sContent.clear();
    m_sContent += sText;
}
void SendEmail::AddAttachment(const string& sPath)
{
    m_listsFile.push_back(sPath);
}
void SendEmail::AddReceiver(const string& sUser)
{
    m_listsReceiver.push_back(sUser);
}
void SendEmail::AddCopyTo(const string& sCopy)
{
    m_listsCopy.push_back(sCopy);
}
bool SendEmail::SendEmailMessage()
{
    bool IsFile = false;
    string sUser = this->m_sUser;
    string sPass = this->m_sPass;
    this->Verification(sUser,sPass);
    if(this->IsExistError())
    {
#ifndef NDEBUG
        MessageBox(NULL,"Verification",this->GetLastError(),MB_OK);
#endif
        return false;
    }
    if(!this->SendMailFrom("E_Mail",this->m_sUser.c_str()))
    {
#ifndef NDEBUG
        MessageBox(NULL,"SendMailFrom","1234",MB_OK);
#endif
        return false;
    }
    list<string>::iterator it;
#ifndef NDEBUG
    MessageBox(NULL,NULL,"m_listsReceiver",MB_OK);
#endif
    for(it = m_listsReceiver.begin(); it!=m_listsReceiver.end(); it++)
    {
        string sBuf = *it;
        this->AddRcptRecipient(sBuf,sBuf,SmtpEngine::TO);
    }
#ifndef NDEBUG
    MessageBox(NULL,NULL,"m_listsCopy",MB_OK);
#endif
    for(it = m_listsCopy.begin(); it!=m_listsCopy.end(); it++)
    {
        string sBuf = *it;
        this->AddRcptRecipient(sBuf,sBuf,SmtpEngine::CC);
    }
#ifndef NDEBUG
    MessageBox(NULL,NULL,"SendRcptRecipient",MB_OK);
#endif
    if(!this->SendRcptRecipient())
    {
        return false;
    }
    else
    {
        if(m_listsFile.size())
        {
            IsFile = true;
        }
        else
        {
            IsFile = false;
        }
    }
#ifndef NDEBUG
    MessageBox(NULL,NULL,"SendMailHeader",MB_OK);
#endif
    if(!this->SendMailHeader(m_sSubject,IsFile))
    {
        return false;
    }
#ifndef NDEBUG
    MessageBox(NULL,NULL,"SendMessage",MB_OK);
#endif
    this->SendMessage(m_sContent);
    for(it = m_listsFile.begin(); it!=m_listsFile.end(); it++)
    {
        string sBuf = *it;
        this->SendAttachment(sBuf);
    }
#ifndef NDEBUG
    MessageBox(NULL,NULL,"SendAttachmentFinally",MB_OK);
#endif
    if(!this->SendAttachmentFinally(IsFile) || !this->SendComplete())
    {
        return false;
    }
#ifndef NDEBUG
    MessageBox(NULL,NULL,"Send OK",MB_OK);
#endif
    this->InitVariable();
    return true;
}
bool SendEmail::IsExistError()
{
    return (m_sLastError.length()>0);
}
char* SendEmail::GetLastError()
{
    return (const_cast<char*>(m_sLastError.c_str()));
}
