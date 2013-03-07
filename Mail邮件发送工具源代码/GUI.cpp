#include "GUI.h"

const unsigned long Text_Color = 0xE4E4E4;
const unsigned long Back_Color = 0xFF8080;
const char szMainCaption[] ="E_Mail 0.1";

HINSTANCE g_hInst;
HICON g_Icon;

static RECT rcCaption= {0};

#define WM_ONTRAYICON   WM_USER + 5
#define WM_ONTRAYICON1   WM_USER + 6

SendEmail g_Send;

void PaintCaption(HDC h_DC,HICON h_Ico,const char* szCaption,RECT rect)
{
    LOGBRUSH _logbrush;
    _logbrush.lbStyle = BS_SOLID;
    _logbrush.lbHatch = 0;
    _logbrush.lbColor = 0x9E6A54;
    FillRect(h_DC,&rect,CreateSolidBrush(Back_Color));
    HBRUSH brush = CreateBrushIndirect(&_logbrush);
    FrameRect(h_DC,&rect,brush);
    DeleteObject(brush);
    SetTextColor(h_DC,Text_Color);
    SetBkMode(h_DC,TRANSPARENT);
    rect.left += ((rect.right-rect.left)/3+20);
    rect.top = 2;
    rect.bottom -= 2;
    HFONT h_font = CreateFont(-12,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,
                              OUT_STRING_PRECIS,CLIP_CHARACTER_PRECIS,DRAFT_QUALITY,
                              DEFAULT_PITCH|FF_DONTCARE,"Tahoma");
    SelectObject(h_DC, h_font);
    if(h_Ico!=NULL)
    {
        DrawIconEx(h_DC,2,2,h_Ico,16,16,0,0,DI_NORMAL);
        rect.left += 20;
    }
    DrawText(h_DC,szCaption,-1,&rect,DT_SINGLELINE|DT_VCENTER);
    DeleteObject(h_font);
}
void DrawButton(PDRAWITEMSTRUCT pdis)
{
    char szText[MAX_PATH] = {0};
    FillRect(pdis->hDC,&pdis->rcItem,(HBRUSH)CreateSolidBrush(Back_Color));

    SetTextColor(pdis->hDC,Text_Color);
    SetBkMode(pdis->hDC,TRANSPARENT);

    DrawEdge(pdis->hDC,&pdis->rcItem,BDR_RAISEDOUTER,BF_RECT);
    GetWindowText(pdis->hwndItem,szText,sizeof(szText));
    DrawText(pdis->hDC,szText,-1,&pdis->rcItem,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

    if((pdis->itemState&ODS_SELECTED))
    {
        SetTextColor(pdis->hDC,0x00ddff);
        DrawText(pdis->hDC,szText,-1,&pdis->rcItem,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
        DrawEdge(pdis->hDC,&pdis->rcItem,BDR_SUNKENOUTER,BF_RECT);
    }
}
void DynamicShow(HWND hwndDlg)
{
    RECT Rt;
    int x, y, i;
    HRGN h_Rgn;
    ShowWindow(hwndDlg, SW_HIDE);
    GetWindowRect(hwndDlg,&Rt);
    x= (Rt.right - Rt.left)/2;
    y= (Rt.bottom - Rt.top)/2;
    for(i = 0; i<(Rt.right/2); i++)
    {
        h_Rgn = CreateRectRgn(x-i,y-i, x+i,y+i);
        SetWindowRgn(hwndDlg,h_Rgn,TRUE);
        ShowWindow(hwndDlg,SW_SHOW);
        DeleteObject(h_Rgn);
    }
}
void AddSysTrayIcon(HWND hWnd,UINT uCallBackMsg)
{
    NOTIFYICONDATA nid = {0};
    nid.cbSize = sizeof(nid);
    nid.uID = 1000;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.hIcon = g_Icon;
    strcpy(nid.szTip,szMainCaption);
    nid.hWnd = hWnd;
    nid.uCallbackMessage = uCallBackMsg;
    Shell_NotifyIcon(NIM_ADD, &nid);
}
void RemoveSysTrayIcon(HWND hWnd,UINT uCallBackMsg)
{
    NOTIFYICONDATA nid = {0};
    nid.cbSize = sizeof(nid);
    nid.uID = 1000;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.hIcon = g_Icon;
    strcpy(nid.szTip,szMainCaption);
    nid.hWnd = hWnd;
    nid.uCallbackMessage = uCallBackMsg;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}
void OnTrayIcon(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    switch(lParam)
    {
    case WM_LBUTTONDBLCLK:
        RemoveSysTrayIcon(hWnd,WM_ONTRAYICON);
        DialogBox(g_hInst,MAKEINTRESOURCE(IDD_EMAIL),NULL,(DLGPROC)DialogProc);
        EndDialog(hWnd,0);
        break;
    }
}
void OnTrayIcon1(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    switch(lParam)
    {
    case WM_LBUTTONDBLCLK:
        ShowWindow(hWnd,SW_SHOW);
        RemoveSysTrayIcon(hWnd,WM_ONTRAYICON);
        break;
    }
}
bool OnVerification(HWND hWnd)
{
    char username[MAX_PATH]= {0};
    char password[MAX_PATH]= {0};
    GetDlgItemText(hWnd,2003,username,MAX_PATH);
    GetDlgItemText(hWnd,2004,password,MAX_PATH);
    g_Send.Verification(username,password);
    g_Send.IsDisconnect(true);
    return g_Send.IsExistError();
}
void InitEditTextControl(HWND hWnd)
{
    SetWindowText(GetDlgItem(hWnd,4002),"");
    SetWindowText(GetDlgItem(hWnd,4004),"");
    SetWindowText(GetDlgItem(hWnd,4005),"");
    SetWindowText(GetDlgItem(hWnd,4007),"");
    int ItemCount = SendMessage(GetDlgItem(hWnd,4009),LB_GETCOUNT,0,0);
    for(int i = 0; i<ItemCount; i++)
    {
        SendMessage(GetDlgItem(hWnd,4009),LB_DELETESTRING,(WPARAM)i,0);
    }
}
void OnSend(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    char* pszTempBufPtr = NULL;
    char pszRecvEmailBuf[MAX_PATH] = {0};
    char pszCopyEmailBuf[MAX_PATH] = {0};
    char pszSubjectBuf[MAX_PATH] = {0};
    char pszTextBuf[8192] = {0};
    int len = GetDlgItemText(hWnd,4002,pszRecvEmailBuf,MAX_PATH);
    if(len <= 0)
    {
        SetWindowText(GetDlgItem(hWnd,IDC_STATUS),"receiver no empty.");
        return;
    }
    else
    {
#ifndef NDEBUG
        MessageBox(NULL,NULL,"m_listsReceiver11",MB_OK);
#endif
        pszTempBufPtr = pszRecvEmailBuf;
        while(pszTempBufPtr < &pszRecvEmailBuf[len])
        {
            char* pszEnd = find(pszTempBufPtr,&pszRecvEmailBuf[len],';');
            if(pszEnd!=NULL)
            {
                char buf[MAX_PATH] = {0};
                strncpy(buf,pszTempBufPtr,pszEnd-pszTempBufPtr);
                g_Send.AddReceiver(buf);
                pszTempBufPtr = pszEnd + 1;
            }
            else
            {
                g_Send.AddReceiver(pszTempBufPtr);
                pszTempBufPtr = &pszRecvEmailBuf[len];

            }
        }
#ifndef NDEBUG
        MessageBox(NULL,NULL,"m_listsReceiver12",MB_OK);
#endif
        len = GetDlgItemText(hWnd,4004,pszCopyEmailBuf,MAX_PATH);
        pszTempBufPtr = pszCopyEmailBuf;
        while(pszTempBufPtr < &pszCopyEmailBuf[len])
        {
            char* pszEnd = find(pszTempBufPtr,&pszCopyEmailBuf[len],';');
            if(pszEnd!=NULL)
            {
                char buf[MAX_PATH] = {0};
                strncpy(buf,pszTempBufPtr,pszEnd-pszTempBufPtr);
                g_Send.AddCopyTo(buf);
                pszTempBufPtr = pszEnd + 1;
            }
            else
            {
                g_Send.AddCopyTo(pszTempBufPtr);
                pszTempBufPtr = &pszCopyEmailBuf[len];

            }
        }
    }
#ifndef NDEBUG
    MessageBox(NULL,NULL,"m_listsReceiver13",MB_OK);
#endif
    len = GetDlgItemText(hWnd,4005,pszSubjectBuf,MAX_PATH);
    if(len <= 0)
    {
        SetWindowText(GetDlgItem(hWnd,IDC_STATUS),"subject no empty.");
        return;
    }
    else
    {
        g_Send.AddSubject(pszSubjectBuf);
    }
#ifndef NDEBUG
    MessageBox(NULL,NULL,"m_listsReceiver14",MB_OK);
#endif
    len = GetDlgItemText(hWnd,4007,pszTextBuf,8192);
    if(len <= 0)
    {
        SetWindowText(GetDlgItem(hWnd,IDC_STATUS),"content no empty.");
        return;
    }
    else
    {
        g_Send.AddContent(pszTextBuf);
    }
#ifndef NDEBUG
    MessageBox(NULL,NULL,"m_listsReceiver15",MB_OK);
#endif
    if(!g_Send.SendEmailMessage())
    {
        SetWindowText(GetDlgItem(hWnd,IDC_STATUS),"send failed.");
    }
    else
    {
        SetWindowText(GetDlgItem(hWnd,IDC_STATUS),"send OK.");
    }
    InitEditTextControl(hWnd);
}
void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
    case 2001:
        if(!OnVerification(hWnd))
        {
            AddSysTrayIcon(hWnd,WM_ONTRAYICON);
            EnableWindow(GetDlgItem(hWnd,2001),FALSE);
            EnableWindow(GetDlgItem(hWnd,2003),FALSE);
            EnableWindow(GetDlgItem(hWnd,2004),FALSE);
            ShowWindow(hWnd,SW_HIDE);
        }
        else
        {
            SetWindowText(GetDlgItem(hWnd,IDC_LOGMAIN),g_Send.GetLastError());
            SetWindowText(GetDlgItem(hWnd,2003),"");
            SetWindowText(GetDlgItem(hWnd,2004),"");
        }
        break;
    case 2002:
        EndDialog(hWnd,0);
        break;
    case 2005:
        OnSend(hWnd,wParam,lParam);
        break;
    case 2006:
        ShowWindow(hWnd,SW_HIDE);
        AddSysTrayIcon(hWnd,WM_ONTRAYICON1);
        break;
    }
}
void OnLbuttondown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    POINT pt = {0};
    pt.x=LOWORD(lParam);
    pt.y=HIWORD(lParam);
    if(PtInRect(&rcCaption,pt))
        PostMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION,0);
}
void OnCtlcolor(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if(GetDlgItem(hWnd,IDC_STATUS)==(HWND)lParam)
    {
        SetTextColor((HDC)wParam,0xFFFFFFFF);
    }
    else
    {
        SetTextColor((HDC)wParam,Text_Color);
    }
    SetBkMode((HDC)wParam,TRANSPARENT);
}
void OnDropFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    char sPath[MAX_PATH]= {0};
    HDROP hDrop = ( HDROP )wParam;
    unsigned int nFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
    for(unsigned int i=0; i<nFile; i++)
    {
        DragQueryFile(hDrop,i, sPath, sizeof(sPath));
        if(GetFileAttributes(sPath)!=FILE_ATTRIBUTE_DIRECTORY)
        {
            g_Send.AddAttachment(sPath);
            SendMessage(GetDlgItem(hWnd,4009),LB_ADDSTRING,0,(LPARAM)sPath);
        }
        else
        {
            SetWindowText(GetDlgItem(hWnd,IDC_STATUS),"no support dir");
        }
    }
    DragFinish(hDrop);
}
LRESULT CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC dc;

    PAINTSTRUCT ps;
    PDRAWITEMSTRUCT pdis;
    switch(uMsg)
    {
    case WM_INITDIALOG:
        g_Icon = LoadIcon(g_hInst,(LPCTSTR)IDI_EMAIL);
        SetWindowText(hwndDlg,szMainCaption);
        SendMessage(hwndDlg,WM_SETICON,TRUE,(WPARAM)g_Icon);
        GetClientRect(hwndDlg,&rcCaption);
        rcCaption.bottom=rcCaption.top+20;
        DynamicShow(hwndDlg);
        return TRUE;
    case WM_CLOSE:
        EndDialog(hwndDlg,0);
        return TRUE;
    case WM_COMMAND:
        OnCommand(hwndDlg,wParam,lParam);
    case WM_LBUTTONDOWN:
        OnLbuttondown(hwndDlg,wParam,lParam);
        break;
    case WM_PAINT:
        dc = BeginPaint(hwndDlg,(LPPAINTSTRUCT)&ps);
        PaintCaption(dc,g_Icon,szMainCaption,rcCaption);
        EndPaint(hwndDlg,&ps);
        return TRUE;
    case WM_DRAWITEM:
        pdis = PDRAWITEMSTRUCT(lParam);
        DrawButton(pdis);
        return TRUE;
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
        OnCtlcolor(hwndDlg,wParam,lParam);
        return (LRESULT)CreateSolidBrush(Back_Color);
    case WM_ONTRAYICON:
        OnTrayIcon(hwndDlg,wParam,lParam);
        break;
    case WM_ONTRAYICON1:
        OnTrayIcon1(hwndDlg,wParam,lParam);
        break;
    case WM_DROPFILES:
        OnDropFile(hwndDlg,wParam,lParam);
    default:
        break;
    }
    return FALSE;
}
int RunDialog(HINSTANCE hInstance)
{
    g_hInst = hInstance;
    return DialogBox(hInstance,MAKEINTRESOURCE(Main),NULL,(DLGPROC)DialogProc);
}
