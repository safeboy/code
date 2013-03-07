void _stdcall TestSmtpEngine()
{
    SmtpEngine Smtp;
    if(!Smtp.SendHandShake("smtp.163.com"))
    {
        fprintf(stderr,"SendHandShake failed.");
    }
    if(!Smtp.SendPrivateKey("dtworker@163.com","MyDengTao"))
    {
        fprintf(stderr,"SendPrivateKey failed.");
    }
    if(!Smtp.SendMailFrom("dtworker","dtworker@163.com"))
    {
        fprintf(stderr,"SendMailFrom failed.");
    }
    Smtp.AddRcptRecipient("dengtao","918831380@qq.com",SmtpEngine::TO);
    Smtp.AddRcptRecipient("dengtao1","458452846@qq.com",SmtpEngine::TO);
    if(!Smtp.SendRcptRecipient())
    {
        fprintf(stderr,"SendRcptRecipient failed.");
    }
    if(!Smtp.SendMailHeader("this is test.",true))
    {
        fprintf(stderr,"SendMailHeader failed.");
    }
    Smtp.SendMessage("this is test.");
    if(!Smtp.SendAttachment("test.txt"))
    {
        fprintf(stderr,"SendAttachment failed.");
    }
    if(!Smtp.SendAttachment("test1.txt")||!Smtp.SendAttachmentFinally(true))
    {
        fprintf(stderr,"SendAttachment failed.");
    }
    if(!Smtp.SendComplete())
    {
        fprintf(stderr,"SendComplete failed.");
    }
}

void TestPop3Engine()
{
    Pop3Engine Pop3;
    Pop3.StartConnectionServer("pop.163.com");
    if(!Pop3.IsErrorEmpty())
    {
        fprintf(stderr,"StartConnectionServer failed.");
    }
    Pop3.SendUsername("dtworker@163.com");
    if(!Pop3.IsErrorEmpty())
    {
        fprintf(stderr,"SendUsername failed.");
    }
    Pop3.SendPassword("MyDengTao");
    if(!Pop3.IsErrorEmpty())
    {
        fprintf(stderr,"SendPassword failed.");
    }
    Pop3.GetUnreadMessageNumber();
    if(!Pop3.IsErrorEmpty())
    {
        fprintf(stderr,"GetUnreadMessageNumber failed.");
    }
    Pop3.GetUnreadMessageList();
    if(!Pop3.IsErrorEmpty())
    {
        fprintf(stderr,"GetUnreadMessageNumber failed.");
    }
    Pop3.GetSpecificMessage(5);
    if(!Pop3.IsErrorEmpty())
    {
        fprintf(stderr,"GetSpecificMessage failed.");
    }
}
