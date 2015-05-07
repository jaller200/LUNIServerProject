#include "EmailSender.h"
#include "FileList.h"
#include <stdio.h>

void main(void)
{
	printf("A C++ class used to send email, such as for servers.\n");
	printf("TLS support (such as for Gmail) requires OPEN_SSL_CLIENT_SUPPORT to be defined\nin RakNetDefines.h.\n");
	printf("Difficulty: Beginner\n\n");

    FileList fileList;
	EmailSender emailSender;
	const char *quote = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
//	const char base64Map[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//	char output[1024];
//	emailSender.Base64Encoding(quote, strlen(quote), output, base64Map);
//	printf("%s", output);

	char mailServer[128], senderUsername[128], receiver[128], password[128];
	printf("Tests sending email.\n");
	printf("Enter mail server: ");
	gets(mailServer);
	if (mailServer[0]==0)
		strcpy(mailServer, "smtp.gmail.com");
	printf("Enter email account username: ");
	gets(senderUsername);
	if (senderUsername[0]==0)
		strcpy(senderUsername, "mygmailaccount@gmail.com");
	printf("Enter receiver email address: ");
	gets(receiver);
	if (receiver[0]==0)
		strcpy(receiver, "destination@host.com");
	printf("Enter password needed to send: ");
	gets(password);
	strcpy(password, "mygmailpassword");

	fileList.AddFile("quote.txt", quote, (const unsigned int) strlen(quote), 0, 0);
	const char *sendResult=emailSender.Send(mailServer,
		25,
		senderUsername,
		receiver,
		senderUsername,
		receiver,
		"Test subject.",
		"Test attachment body :).\n.\n..\n.\n(Should be .,.,..,.)\r\n.\r\n.\r\n..\r\n.\r\n(Should be .,.,..,.)12345\r\n.\r\n",
		&fileList,
		true,
		password);
	if (sendResult!=0)
		printf("Send Failed! %s", sendResult);
	else
		printf("Success (probably).\n");
	printf("Press enter to quit.\n");
	char buff[256];
	gets(buff);
}
