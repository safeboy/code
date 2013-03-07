#include "SmtpEngine.h"

SmtpEngine::SmtpEngine()
{
    WSADATA WSAData = { 0 };
    if(0!=WSAStartup(0x202,&WSAData))
    {
        throw fprintf(stderr,"WSAStartup failed.");
    }
    this->GetMemory(1024*8,1024);
}
SmtpEngine::SmtpSocket::SmtpSocket()
{
    m_hSocket = INVALID_SOCKET;
}
bool SmtpEngine::SmtpSocket::Create()
{
    m_hSocket = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
    return (m_hSocket != INVALID_SOCKET);
}
bool SmtpEngine::SmtpSocket::Connect(const char* lpszHost, int nPort)
{
    assert(m_hSocket != INVALID_SOCKET);

    struct sockaddr_in sockAddr = {0};
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons((u_short)nPort);
    sockAddr.sin_addr.s_addr = inet_addr(lpszHost);

    if (sockAddr.sin_addr.s_addr == INADDR_NONE)
    {
        LPHOSTENT lphost;
        lphost = gethostbyname(lpszHost);
        if (lphost != NULL)
            sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
        else
        {
            WSASetLastError(WSAEINVAL);
            return false;
        }
    }
    return Connect((struct sockaddr*)&sockAddr, sizeof(sockAddr));
}
bool SmtpEngine::SmtpSocket::Connect(const struct sockaddr* lpSockAddr, int nSockAddrLen)
{
    return (connect(m_hSocket, lpSockAddr, nSockAddrLen) != SOCKET_ERROR);
}
bool SmtpEngine::SmtpSocket::Send(char* lpszBuf, int nBuf)
{
    return (send(m_hSocket, lpszBuf, nBuf, 0) != SOCKET_ERROR);
}
int SmtpEngine::SmtpSocket::Receive(char* lpszBuf, int nBuf)
{
    return recv(m_hSocket, lpszBuf, nBuf, 0);
}
void SmtpEngine::SmtpSocket::Close()
{
    if (m_hSocket != INVALID_SOCKET)
    {
        assert(SOCKET_ERROR != closesocket(m_hSocket));
        m_hSocket = INVALID_SOCKET;
    }
}
bool SmtpEngine::Disconnect()
{
    string sBuf("QUIT\r\n");
    if(!this->m_SMTP.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return false;
    }
    if(!this->ReadResponse("221"))
    {
        return false;
    }
    this->m_SMTP.Close();
    return true;
}
bool SmtpEngine::ReadResponse(const char* pszCode)
{
    char buf[_MAX_PATH] = {0};
    if(this->m_SMTP.Receive(buf,_MAX_PATH))
    {
        return (strstr(buf,pszCode)!=NULL);
    }
    return false;
}
bool SmtpEngine::SendVerify250Response(const string& sData)
{
    if(!this->m_SMTP.Send(const_cast<char*>(sData.c_str()),sData.length()))
    {
        return false;
    }
    if(!this->ReadResponse("250"))
    {
        return false;
    }
    return true;
}
string SmtpEngine::GetMailHeader(const string& sSubject)
{
    string sBuf("");
    time_t rawtime;
    char buffer[80] = {0};
    time(&rawtime);
    strftime(buffer,80,"%a, %d %b %Y %H:%M:%S ",localtime(&rawtime));
    sBuf += "From: ";
    sBuf += this->m_sAddress;
    sBuf += EOL;
    map<string,string>::iterator it;
    sBuf += "To:";
    for(it = m_mapTO.begin(); it != m_mapTO.end(); it++)
    {
        if(it!=m_mapTO.begin())
        {
            sBuf += ",";
        }
        sBuf += (*it).first;
        sBuf += " ";
        sBuf += "<";
        sBuf += (*it).second;
        sBuf += ">";
    }
    sBuf += EOL;
    if(m_mapCC.size())
    {
        sBuf += "Cc: ";
        for(it = m_mapCC.begin(); it != m_mapCC.end(); it++)
        {
            if(it!=m_mapTO.begin())
            {
                sBuf += ",";
            }
            sBuf += (*it).first;
            sBuf += " ";
            sBuf += "<";
            sBuf += (*it).second;
            sBuf += ">";
        }
        sBuf += EOL;
    }
    sBuf += "Subject: ";
    sBuf += sSubject;
    sBuf += EOL;
    sBuf += "Date: ";
    sBuf += buffer;
    sBuf += EOL;
    sBuf += "X-Mailer: SmtpEngine0.1\r\n";
    return sBuf;
}
bool SmtpEngine::SendHandShake(const char* pszHostName,int nPort)
{
    if(!this->m_SMTP.Create())
    {
        return false;
    }
    if(!this->m_SMTP.Connect(pszHostName,nPort))
    {
        return false;
    }
    if(!this->ReadResponse("220"))
    {
        return false;
    }
    string sBuf("HELO ");
    sBuf+=pszHostName;
    sBuf+=EOL;
    return (this->SendVerify250Response(sBuf));
}
bool SmtpEngine::SendPrivateKey(const char* username,const char* password)
{
    char Encod64User[50] = {0},Encod64Password[50] = {0};
    string auth("AUTH login\r\n");
    if(!this->m_SMTP.Send(const_cast<char*>(auth.c_str()),auth.length()))
    {
        return false;
    }
    if(!this->ReadResponse("334"))
    {
        return true;
    }
    else
    {
        CBase64 Base64;
        int nOutSize = Base64.GetBufferLength(strlen(username));
        Base64.Encoder(username,strlen(username),Encod64User,&nOutSize);
        nOutSize = Base64.GetBufferLength(strlen(password));
        Base64.Encoder(password, strlen(password),Encod64Password,&nOutSize);
        auth.clear();
        auth += Encod64User;
        if(!this->m_SMTP.Send(const_cast<char*>(auth.c_str()),auth.length()))
        {
            return false;
        }
        if(!this->ReadResponse("334"))
        {
            return false;
        }
        auth.clear();
        auth += Encod64Password;
        if(!this->m_SMTP.Send(const_cast<char*>(auth.c_str()),auth.length()))
        {
            return false;
        }
        if(!this->ReadResponse("235"))
        {
            return false;
        }
        return true;
    }
}
bool SmtpEngine::SendMailFrom(const char* sFriendly,const char* sAddress)
{
    this->m_sFriendly.clear();
    this->m_sAddress.clear();
    this->m_sFriendly += sFriendly;
    this->m_sAddress += sAddress;
    string sBuf("MAIL FROM:<");
    sBuf+=sAddress;
    sBuf+=">";
    sBuf+=EOL;
    return (this->SendVerify250Response(sBuf));
}
void SmtpEngine::AddRcptRecipient(const string& sFriendly, const string& sAddress,RECIPIENT_TYPE RecipientType)
{
    switch (RecipientType)
    {
    case TO:
        m_mapTO[sFriendly] = sAddress;
        break;
    case CC:
        m_mapCC[sFriendly] = sAddress;
        break;
    case BCC:
        m_mapBCC[sFriendly] = sAddress;
        break;
    default:
        assert(0);
        break;
    }
}
bool SmtpEngine::SendRcptRecipient()
{
    string sBuf;
    map<string,string>::iterator it;
    for(it = m_mapTO.begin(); it != m_mapTO.end(); it++)
    {
        sBuf.clear();
        sBuf += "RCPT TO:<";
        sBuf += (*it).second;
        sBuf += ">\r\n";
        if(!this->SendVerify250Response(sBuf))
        {
            return false;
        }
    }
    for(it = m_mapCC.begin(); it != m_mapCC.end(); it++)
    {
        sBuf.clear();
        sBuf += "RCPT TO:<";
        sBuf += (*it).second;
        sBuf += ">\r\n";
        if(!this->SendVerify250Response(sBuf))
        {
            return false;
        }
    }
    for(it = m_mapBCC.begin(); it != m_mapBCC.end(); it++)
    {
        sBuf.clear();
        sBuf += "RCPT TO:<";
        sBuf += (*it).second;
        sBuf += ">\r\n";
        if(!this->SendVerify250Response(sBuf))
        {
            return false;
        }
    }
    return true;
}
bool SmtpEngine::SendMailHeader(const string& sSubject,bool IsAnnex)
{
    string sBuf("DATA\r\n");
    if(!this->m_SMTP.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return false;
    }
    if(!this->ReadResponse("354"))
    {
        return false;
    }
    sBuf.clear();
    sBuf += this->GetMailHeader(sSubject);
    if(IsAnnex)
    {
        sBuf += "MIME-Version: 1.0\r\nContent-type: multipart/mixed; boundary=\"#BOUNDARY#\"\r\n";
    }
    else
    {
        sBuf += "MIME-Version: 1.0\r\nContent-type: text/plain; charset=US-ASCII\r\n";
    }
    if(!this->m_SMTP.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return false;
    }
    if(IsAnnex)
    {
        sBuf.clear();
        sBuf += "\r\n--#BOUNDARY#\r\n";
        sBuf += "Content-Type: text/plain; charset=us-ascii\r\n";
        sBuf += "Content-Transfer-Encoding: quoted-printable\r\n";
        if(!this->m_SMTP.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
        {
            return false;
        }
    }
    sBuf.clear();
    sBuf += EOL;
    if(!this->m_SMTP.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return false;
    }
    return true;
}
bool SmtpEngine::SendMessage(const string& sMsg)
{
    if(!this->m_SMTP.Send(const_cast<char*>(sMsg.c_str()),sMsg.length()))
    {
        return false;
    }
    return true;
}
bool SmtpEngine::SendAttachment(const string& sPath)
{
    bool bIsTure = false;
    string sBuf;
    char sTitle[_MAX_PATH];
    char sFname[_MAX_FNAME];
    char sExt[_MAX_EXT];
    _splitpath(sPath.c_str(), NULL, NULL, sFname, sExt);
    _makepath(sTitle, NULL, NULL, sFname, sExt);
    sBuf += "\r\n\r\n--#BOUNDARY#\r\n";
    sBuf += "Content-Type: application/octet-stream; name=";
    sBuf += sTitle;
    sBuf += EOL;
    sBuf += "Content-Transfer-Encoding: base64\r\n";
    sBuf += "Content-Disposition: attachment; filename=";
    sBuf += sTitle;
    sBuf += EOL;
    sBuf += EOL;
    if(!this->m_SMTP.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return bIsTure;
    }
    ifstream infile;
    infile.open(sPath.c_str(),ios::binary);
    if(infile.is_open())
    {
        CBase64 Base64;
        infile.seekg(0, ios::end);
        long size = infile.tellg();
        infile.seekg (0, ios::beg);
        char* pszBuf = static_cast<char*>(this->Malloc(size+1));
        infile.read(pszBuf,size);
        int nOutSize = Base64.GetBufferLength(size);
        char* pszEncoded = static_cast<char*>(this->Malloc(nOutSize));
        Base64.Encoder(pszBuf,size,pszEncoded,&nOutSize);
        this->Delete(pszBuf);
        if(this->m_SMTP.Send(pszEncoded,nOutSize))
        {
            bIsTure = true;
        }
        this->Delete(pszEncoded);
        infile.close();
    }
    return bIsTure;
}
bool SmtpEngine::SendAttachmentFinally(bool IsAnnex)
{
    if(IsAnnex)
    {
        string sBuf("\r\n--#BOUNDARY#--");
        if (!this->m_SMTP.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
        {
            return false;
        }
    }
    return true;
}
bool SmtpEngine::SendComplete()
{
    string sBuf("\r\n.\r\n");
    if(!this->m_SMTP.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return false;
    }
    if(!this->ReadResponse("250"))
    {
        return false;
    }
    return true;
}
SmtpEngine::~SmtpEngine()
{
    this->Disconnect();
    if(0!=WSACleanup())
    {
        throw fprintf(stderr,"WSACleanup failed.");
    }
}
