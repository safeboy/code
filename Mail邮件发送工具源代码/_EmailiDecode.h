#ifndef _EMAILDECODE_H
#define _EMAILDECODE_H

#include <cassert>
#include <ctime>
#include "CBase64.h"
#include "CMemory.h"

#define NDEBUG

struct EMAIL
{
    void* pDataDest;
    long ulDataDestLen;
    char* pszOriginalSender;
    long ulOriginalSender;
    char* pszSender;
    long ulSenderLen;
    char* pszReceiver;
    long ulReceiverLen;
    char* pszCopiedTo;
    long ulCopiedToLen;
    char* pszTimeDate;
    long ulTimeDateLen;
    char* pszSubject;
    long ulSubjectLen;
    char* pszContent;
    long ulContentLen;
};
typedef struct EMAIL* PEMAIL;

#endif // _EMAILDECODE_H
