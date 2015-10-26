#include <iconv.h>

#ifndef __SENDMAIL_H__
#define __SENDMAIL_H__

#define SMTP_PORT       25
#define MAX_LINE_SIZE   1024
#define MAX_NAME_SIZE   64


#define MAIL_HOST	"smtp.acidflash.org"
#define MAIL_SENDER	"olivka@acidflash.org"
#define MAIL_SENDER2	"Olivka"

//#define MAIL_HOST	"mail.tula.net"
//#define MAIL_SENDER	"olivka@tula.net"
//#define MAIL_SENDER2	"Olivka"

#define CDICT_SENDMAIL 	"/home/olivka/bot/dict/sendmail.alt"

/*
#define MAIL_HOST	"mail.smartru.com"
#define MAIL_SENDER	"eilyushin@smartru.com"
#define MAIL_SENDER2	"Bot"
*/

typedef struct tagSENDMAIL
{
	const char *lpszHost;          // host name or dotted IP address
	const char *lpszSender;        // sender userID (optional)
	const char *lpszSenderName;    // sender display name (optional)
	const char *lpszRecipient;     // recipient userID
	const char *lpszRecipientName; // recipient display name (optional)
	const char *lpszReplyTo;       // reply to userID (optional)
	const char *lpszReplyToName;   // reply to display name (optional)
	const char *lpszMessageID;     // message ID (optional)
	const char *lpszSubject;       // subject of message
	const char *lpszMessage;       // message text
} SENDMAIL;

int net_sendmail(char *recipent, char *subject, char *message);
int net_sendnail_ex(SENDMAIL *pMail, int *pResult);

#endif  //  __SENDMAIL_H__
