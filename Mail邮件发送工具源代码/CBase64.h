#ifndef CBASE64_H
#define CBASE64_H
#include "_CBase64.h"

class CBase64
{
public:
    size_t GetBufferLength(int iInLen);
    size_t Encoder(const char* pszIn, int nInLen, char* pszOut,int* nOutLen);
    size_t Decoder(unsigned char* dst,const unsigned char* src,size_t srclen);
};

#endif // CBASE64_H
