#include "EmaiDecode.h"

EmailiDecode::EmailiDecode()
{
    this->InitVariable();
    this->GetMemory(1024*8,1024);
}
bool EmailiDecode::IsAsciiByte(void* src)
{
    unsigned char* psrc = static_cast<unsigned char*>(src);
    if(*psrc>=0&&*psrc<=0x7F)
    {
        return true;
    }
    return false;
}
bool EmailiDecode::IsWideByte(void* src)
{
    wchar_t* psrc = static_cast<wchar_t*>(src);
    if((psrc[0]>=0&&psrc[0]<=0x7F)&&!psrc[1])
    {
        return true;
    }
    return false;
}
bool EmailiDecode::IsMultiByte(void* src)
{
    wchar_t* psrc = static_cast<wchar_t*>(src);
    if((psrc[0]>=0x80&&psrc[0]<=0xFF)&&!(psrc[1]>=0x80&&psrc[1]<=0xFF))
    {
        return true;
    }
    return false;
}
void EmailiDecode::InitVariable()
{
    this->m_Email = NULL;
}
void EmailiDecode::GetEmail(void* src,size_t len,PEMAIL* Email)
{
    this->m_Email = static_cast<PEMAIL>(this->Malloc(sizeof(struct EMAIL)));
    *Email = this->m_Email;
}
void EmailiDecode::Complete()
{
    assert(this->m_Email!=NULL);

    this->Delete(m_Email);
    this->InitVariable();
}
EmailiDecode::~EmailiDecode()
{
    this->InitVariable();
}
