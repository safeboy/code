#include "CBase64.h"

unsigned char basis64[] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t CBase64::GetBufferLength(int iInLen)
{
    int nOutSize = (iInLen+2)/3*4;
    nOutSize += strlen("\r\n")*nOutSize/BASE64_MAXLINE + 3;
    return nOutSize;
}
size_t CBase64::Encoder(const char* pszIn, int nInLen, char* pszOut,int* nOutLen)
{
    int nInPos  = 0;
    int nOutPos = 0;
    int nLineLen = 0;

    for (int i=0; i<nInLen/3; ++i)
    {
        int c1 = pszIn[nInPos++] & 0xFF;
        int c2 = pszIn[nInPos++] & 0xFF;
        int c3 = pszIn[nInPos++] & 0xFF;

        pszOut[nOutPos++] = basis64[(c1 & 0xFC) >> 2];
        pszOut[nOutPos++] = basis64[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
        pszOut[nOutPos++] = basis64[((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6)];
        pszOut[nOutPos++] = basis64[c3 & 0x3F];
        nLineLen += 4;

        if (nLineLen >= BASE64_MAXLINE-3)
        {
            const char* cp = EOL;
            pszOut[nOutPos++] = *cp++;
            if (*cp)
                pszOut[nOutPos++] = *cp;
            nLineLen = 0;
        }
    }

    const char* cp;
    switch (nInLen % 3)
    {
    case 0:
    {
        cp = EOL;
        pszOut[nOutPos++] = *cp++;
        if (*cp)
            pszOut[nOutPos++] = *cp;
        break;
    }
    case 1:
    {
        int c1 = pszIn[nInPos] & 0xFF;
        pszOut[nOutPos++] = basis64[(c1 & 0xFC) >> 2];
        pszOut[nOutPos++] = basis64[((c1 & 0x03) << 4)];
        pszOut[nOutPos++] = '=';
        pszOut[nOutPos++] = '=';
        cp = EOL;
        pszOut[nOutPos++] = *cp++;
        if (*cp)
            pszOut[nOutPos++] = *cp;
        break;
    }
    case 2:
    {
        int c1 = pszIn[nInPos++] & 0xFF;
        int c2 = pszIn[nInPos] & 0xFF;
        pszOut[nOutPos++] = basis64[(c1 & 0xFC) >> 2];
        pszOut[nOutPos++] = basis64[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
        pszOut[nOutPos++] = basis64[((c2 & 0x0F) << 2)];
        pszOut[nOutPos++] = '=';
        cp = EOL;
        pszOut[nOutPos++] = *cp++;
        if (*cp)
            pszOut[nOutPos++] = *cp;
        break;
    }
    default:
    {
        assert(0);
        break;
    }
    }
    pszOut[nOutPos] = 0;
    *nOutLen = nOutPos;
    return nOutPos;
}
size_t CBase64::Decoder(unsigned char* dst,const unsigned char* src,size_t srclen)
{
    char cn = 0;
    unsigned char w1 = 0, w2 = 0, w3 = 0;
    unsigned long pos = 0;
    char isValid[256] = {0};
    char decodeValue[256] = {0};
    for(unsigned long i = 0; i < sizeof(basis64)-1; i++)
    {
        char c = basis64[i];
        isValid[(int)c]     = 1;
        decodeValue[(int)c] = i;
    }
    for (unsigned long i = 0; (i < srclen) && isValid[src[i]]; i++)
    {
        char bitChunk = decodeValue[src[i]];
        switch ( cn )
        {
        case 0:
            w1 = (bitChunk << 2);
            break;
        case 1:
            w1 |= (bitChunk >> 4);
            w2 |= (bitChunk << 4);
            break;
        case 2:
            w2 |= (bitChunk >> 2);
            w3 |= (bitChunk << 6);
            break;
        case 3:
            w3 |= bitChunk;
            break;
        }
        cn++;
        if (cn == 4)
        {
            dst[pos++] = w1;
            dst[pos++] = w2;
            dst[pos++] = w3;
            cn = w1 = w2 = w3 = 0;
        }
    }
    switch (cn)
    {
    case 2:
        dst[pos++] = w1;
        break;

    case 3:
        dst[pos++] = w1;
        dst[pos++] = w2;
        break;
    }
    return pos;
}
