// Mostly from http://www.codeproject.com/internet/SendTo.asp and
// Also see http://www.codeguru.com/cpp/i-n/network/messaging/article.php/c5417/

#ifndef __SENDFILETO_H__
#define __SENDFILETO_H__

#include <windows.h>
#include <mapi.h>


class CSendFileTo
{
public:
	bool SendMail(HWND hWndParent, const char *strAttachmentFilePath, const char *strAttachmentFileName,const char *strSubject, const char *strBody, const char *strRecipient);
};

#endif