#ifndef EMAILDECODE_H
#define EMAILDECODE_H

#include "_EmailiDecode.h"

class EmailiDecode:private CMemory
{
    bool IsAsciiByte(void* src);//1 byte
    bool IsWideByte(void* src);//2 byte
    bool IsMultiByte(void* src);//2 byte
    void InitVariable();
public:
    EmailiDecode();
    void GetEmail(void* src,size_t len,PEMAIL* Email);
    void Complete();
private:
    PEMAIL m_Email;
public:
    virtual ~EmailiDecode();
};


#endif // EMAILDECODE_H
