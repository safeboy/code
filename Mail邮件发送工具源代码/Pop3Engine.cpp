#include "Pop3Engine.h"

Pop3Engine::Pop3Engine()
{
    WSADATA WSAData = { 0 };
    if(0!=WSAStartup(0x202,&WSAData))
    {
        throw fprintf(stderr,"WSAStartup failed.");
    }
}
bool Pop3Engine::ReadResponseOK(RESPONSE index)
{
    char inBuf[8192] = {0};
    if(this->m_POP3.Receive(inBuf,8192))
    {
//#ifndef NDEBUG
        cout << "Pop3Engine::ReadResponseOK->\n" << inBuf << endl;
//#endif
        m_ErrorMessage.clear();
        switch(index)
        {
        case CONNECTION_CHECK:
            if (strnicmp(inBuf,"-ERR", 4) == 0)
            {
                m_ErrorMessage = "Error (POP3): connection failure.";
                return false;
            }
            break;
        case USER_CHECK:
            if (strnicmp(inBuf,"-ERR", 4) == 0)
            {
                m_ErrorMessage = "Error (POP3): wrong user name.";
                return false;
            }
            break;
        case PASS_CHECK:
            if (strnicmp(inBuf,"-ERR", 4) == 0)
            {
                m_ErrorMessage = "Error (POP3): wrong password.";
                return false;
            }
            break;
        case QUIT_CHECK:
            if (strnicmp(inBuf,"-ERR", 4) == 0)
            {
                m_ErrorMessage = "Error (POP3): error occured during QUIT.";
                return false;
            }
            break;
        case STAT_CHECK:
            if (strnicmp(inBuf,"-ERR", 4) == 0)
            {
                m_ErrorMessage = "Error (POP3): error occured during STAT.";
                return false;
            }
            else
            {

            }
            break;
        case LIST_CHECK:
            if (strnicmp(inBuf,"-ERR", 4) == 0)
            {
                m_ErrorMessage = "Error (POP3): error occured during LIST.";
                return false;
            }
            else
            {

            }
            break;
        case RETR_CHECK:
            if (strnicmp(inBuf,"-ERR", 4) == 0)
            {
                m_ErrorMessage = "Error (POP3): error occured during RETR.";
                return false;
            }
            else
            {

            }
            break;
        default:
            break;
        }
        return true;
    }
    return false;
}
bool Pop3Engine::Disconnect()
{
    string sBuf("QUIT\r\n");
    if(!this->m_POP3.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return false;
    }
    if(!this->ReadResponseOK(QUIT_CHECK))
    {
        return false;
    }
    return true;
}
Pop3Engine::Pop3Socket::Pop3Socket()
{
    m_hSocket = INVALID_SOCKET;
}
bool Pop3Engine::Pop3Socket::Create()
{
    m_hSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    return (m_hSocket != INVALID_SOCKET);
}
bool Pop3Engine::Pop3Socket::Connect(const char* lpszHost, int nPort)
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
bool Pop3Engine::Pop3Socket::Connect(const struct sockaddr* lpSockAddr, int nSockAddrLen)
{
    return (connect(m_hSocket, lpSockAddr, nSockAddrLen) != SOCKET_ERROR);
}
bool Pop3Engine::Pop3Socket::Send(char* lpszBuf, int nBuf)
{
    assert(m_hSocket != INVALID_SOCKET);
    return (send(m_hSocket, lpszBuf, nBuf, 0) != SOCKET_ERROR);
}
int Pop3Engine::Pop3Socket::Receive(char* lpszBuf, int nBuf)
{
    assert(m_hSocket != INVALID_SOCKET);
    return recv(m_hSocket, lpszBuf, nBuf, 0);
}
void Pop3Engine::Pop3Socket::Close()
{
    if (m_hSocket != INVALID_SOCKET)
    {
        assert(SOCKET_ERROR != closesocket(m_hSocket));
        m_hSocket = INVALID_SOCKET;
    }
}
bool Pop3Engine::StartConnectionServer(const char* pszHostName, int nPort)
{
    if(!this->m_POP3.Create())
    {
        return false;
    }
    if(!this->m_POP3.Connect(pszHostName,nPort))
    {
        return false;
    }
    if(!this->ReadResponseOK(CONNECTION_CHECK))
    {
        return false;
    }
    return true;
}
bool Pop3Engine::SendUsername(const string& sName)
{
    string sBuf("USER ");
    sBuf += sName;
    sBuf += EOL;
    if(!this->m_POP3.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return false;
    }
    if(!this->ReadResponseOK(USER_CHECK))
    {
        return false;
    }
    return true;
}
bool Pop3Engine::SendPassword(const string& sPass)
{
    string sBuf("PASS ");
    sBuf += sPass;
    sBuf += EOL;
    if(!this->m_POP3.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return false;
    }
    if(!this->ReadResponseOK(PASS_CHECK))
    {
        return false;
    }
    return true;
}
bool Pop3Engine::GetUnreadMessageNumber()
{
    string sBuf("STAT\r\n");
    if(!this->m_POP3.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return false;
    }
    if(!this->ReadResponseOK(STAT_CHECK))
    {
        return false;
    }
    return true;
}
bool Pop3Engine::GetUnreadMessageList()
{
    string sBuf("LIST\r\n");
    if(!this->m_POP3.Send(const_cast<char*>(sBuf.c_str()),sBuf.length()))
    {
        return false;
    }
    if(!this->ReadResponseOK(LIST_CHECK))
    {
        return false;
    }
    return true;
}
bool Pop3Engine::GetSpecificMessage(int index)
{
    char inBuf[512] = {0};
    int length = snprintf(inBuf,512,"RETR %d\r\n",index);
    if(!this->m_POP3.Send(inBuf,length))
    {
        return false;
    }
    if(!this->ReadResponseOK(RETR_CHECK))
    {
        return false;
    }
    return true;
}
bool Pop3Engine::IsErrorEmpty() const
{
    return (m_ErrorMessage.length()<=0);
}
string Pop3Engine::GetErrorMessage() const
{
    return m_ErrorMessage;
}
Pop3Engine::~Pop3Engine()
{
    this->Disconnect();
    this->m_POP3.Close();
    if(0!=WSACleanup())
    {
        throw fprintf(stderr,"WSACleanup failed.");
    }
}
