#define	VERIFY_RET_VAL(arg) { int nRet = arg; if (nRet) return nRet; }

#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>

#include "sendmail.h"

#define SOCKET_ERROR -1


// private functions - used internally //////////////////////////

int SendMailMessage(SENDMAIL *pMail); 
int Send(int s, const char *lpszBuff, int nLen, int nFlags);
int Receive(int s, char *lpszBuff, int nLenMax, int nFlags, const char *lpszReplyCode);


#define LOG LOG_err
int LOG_err(const char *errorText, ...)
{
    va_list arglist;
    va_start(arglist, errorText);
    int retCode = vfprintf(stderr, errorText, arglist);
    va_end(arglist);
    
    return retCode;
}

#define ESC ESC_err
char *ESC_err(char *s)
{
    static char _es[1024];
    char *es = (char *)_es;
    int sl = strlen(s);
    int i;
    for (i = 0; i < 1024 && i < sl; i++)
    {
	switch (s[i])
        {
	case '\r':
        case '\n':
	case '\t':
        case '\b':
	    es[i] = '?';
	    break;
        default:
    	    es[i] = s[i];
	}
    }
    return es;
}

// Static variables /////////////////////////////////////////////

static char gszMailerID[] = "X-Mailer: sendmail\r\n";

// SendMail - sends an SMTP mail message to specified host. This
// is the only exported function from this DLL.
/////////////////////////////////////////////////////////////////
int net_sendmail(char *recipent, char *subject, char *message)
{
    SENDMAIL mail;
    int res;
    char message2[4096] = "\n\n";
    int ml = strlen(message);
    int i, i2 = 0;
    memset(&mail, 0, sizeof(SENDMAIL));
    
    if (ml < 1)
	return 0; // error

    mail.lpszHost = MAIL_HOST;
    mail.lpszSender = MAIL_SENDER;
    mail.lpszSenderName = MAIL_SENDER2;

    mail.lpszRecipient = recipent;
    mail.lpszSubject = subject;
    
    for (i = 0; i < ml && i < 4096; i++)
    {
	switch (message[i])
	{
	case '\\':
	    i++;
	    switch (message[i])
	    {
	    case 0:
	    case '\\':
		message2[i2++] = '\\';
    		break;
	    case 'n':
		message2[i2++] = '\n';
    		break;
	    case 't':
		message2[i2++] = '\t';
    		break;
	    default:
		message2[i2++] = '\\';
		message2[i2++] = message[i];
    		break;
	    }
	    break;
	default:
	    message2[i2++] = message[i];
	    break;
	}
    }
    message2[i2++] = 0;
    
    mail.lpszMessage = message2;
    
    int res2 = net_sendnail_ex(&mail, &res);
    return res2 == 0;
}

int net_sendnail_ex(SENDMAIL *pMail, int *pResult)
{
//	unsigned short wVersion = 0x0101;
	int nRet, nRet2;
	
	LOG("SendMail(%08X, %08X);\n", pMail, pResult);

	if (!pResult)
		pResult = &nRet2;

	// check for required parameters
	if (pMail == NULL || pResult == NULL ||	pMail->lpszHost == NULL ||
//		pMail->lpszRecipient == NULL ||	pMail->lpszSubject == NULL ||
		pMail->lpszRecipient == NULL ||	
		pMail->lpszMessage == NULL)
	{
		if (pResult)
			*pResult = -1;
		return -13;
	}

	// try to send the message
	nRet = SendMailMessage(pMail);

	if (nRet != 0)
	{
	}

	*pResult = nRet;
	return (*pResult == 0);
}


// SendMailMessage - actually sends the message.This is a private
// function.
/////////////////////////////////////////////////////////////////
int SendMailMessage(SENDMAIL *pMail)
{
	char szBuff[MAX_LINE_SIZE + 1]; // transmit/receive buffer
	char szUser[MAX_NAME_SIZE + 1] = "olivka";
	char szName[MAX_NAME_SIZE + 1]; // host name buffer
//	unsigned long dwSize = MAX_NAME_SIZE;
	int s;

	struct hostent *ph;
	struct sockaddr_in addr;

	char szTime[MAX_NAME_SIZE + 1]; // time related vars
	time_t tTime;
	struct tm *ptm;
//	struct timeb tbTime;

	LOG("SendMailMessage(%08X);\n", pMail);
	
	// connect to the SMTP port on remote host
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)

		return -9;
	LOG("socket() = %08X);\n", s);

	if (isdigit(*pMail->lpszHost) && strchr(pMail->lpszHost, '.'))
	{
		unsigned long iaddr = inet_addr(pMail->lpszHost);
		ph = gethostbyaddr((const char *)&iaddr, 4, PF_INET);
	}
	else
		ph = gethostbyname(pMail->lpszHost);

	if (ph == NULL)
		return -1;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(SMTP_PORT);
	memcpy(&addr.sin_addr, ph->h_addr_list[0], sizeof(struct in_addr));

	if (connect(s, (struct sockaddr *)&addr, sizeof(struct sockaddr)))
		return -2;

	LOG("connect() = %08X);\n", s);

	// receive signon message
	VERIFY_RET_VAL(Receive(s, szBuff, MAX_LINE_SIZE, 0, "220"););

	// get user name and local host name
	gethostname(szName, MAX_NAME_SIZE);

	// send HELO message
	sprintf(szBuff, "HELO %s\r\n", szName);
	VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)
	VERIFY_RET_VAL(Receive(s, szBuff, MAX_LINE_SIZE, 0, "250");)

	LOG("HELO - ok\n");
	
	// send MAIL message
	if (pMail->lpszSender)
	{
		sprintf(szBuff, "MAIL FROM: <%s", pMail->lpszSender);
		if (strchr(pMail->lpszSender, '@'))
			strcat(szBuff, ">\r\n");
		else
			sprintf(szBuff + strlen(szBuff), "@%s>\r\n", szName);
	}
	else
		sprintf(szBuff, "MAIL FROM:<%s@%s>\r\n", szUser, szName);

	VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)
	VERIFY_RET_VAL(Receive(s, szBuff, MAX_LINE_SIZE, 0, "250");)

	LOG("MAIL from - ok\n");
		// send RCPT message
		sprintf(szBuff, "RCPT TO: <%s>\r\n", pMail->lpszRecipient);
	VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)
	VERIFY_RET_VAL(Receive(s, szBuff, MAX_LINE_SIZE, 0, "25");)

		// send DATA message
		sprintf(szBuff, "DATA\r\n");
	VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)
	VERIFY_RET_VAL(Receive(s, szBuff, MAX_LINE_SIZE, 0, "354");)

	LOG("DATA - ok\n");
	
		// construct date string
		tTime = time(NULL);
	ptm = localtime(&tTime);

	strftime(szTime, MAX_NAME_SIZE, "%a, %d %b %Y %H:%M:%S %Z", ptm);

	// find time zone offset and correct for DST
	//ftime(&tbTime);
	//if (tbTime.dstflag)
	//	tbTime.timezone -= 60;

	sprintf(szTime + strlen(szTime), " 0400");
	//	-tbTime.timezone / 60, tbTime.timezone % 60);

	// send mail headers
	// Date:
	sprintf(szBuff, "Date: %s\r\n", szTime);
	VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)

		// X-Mailer:
	VERIFY_RET_VAL(Send(s, gszMailerID, strlen(gszMailerID), 0);)

	LOG("DATE - ok\n");
		// Message-ID:
		if (pMail->lpszMessageID)
		{
			sprintf(szBuff, "Message-ID: %s\r\n", pMail->lpszMessageID);
			VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)
		}

		// Encoding
			sprintf(szBuff, "Content-Type: text/plain; charset=cp1251\r\n");
			VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)
	LOG("LANG - ok\n");
			
		// To:
		sprintf(szBuff, "To: %s", pMail->lpszRecipient);
		if (pMail->lpszRecipientName)
			sprintf(szBuff + strlen(szBuff), " (%s)\r\n",
			pMail->lpszRecipientName);
		else
			strcat(szBuff, "\r\n");
		VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)

			// From:
			if (pMail->lpszSender)
			{
				sprintf(szBuff, "From: %s", pMail->lpszSender);
				if (pMail->lpszSenderName)
					sprintf(szBuff + strlen(szBuff), " (%s)\r\n",
					pMail->lpszSenderName);
				else
					strcat(szBuff, "\r\n");
			}
			else
				sprintf(szBuff, "From: %s@%s\r\n", szUser, szName);
		VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)

	LOG("FROM - ok\n");
			// Reply-To:
			if (pMail->lpszReplyTo)
			{
				sprintf(szBuff, "Reply-To: %s", pMail->lpszReplyTo);
				if (pMail->lpszReplyToName)
					sprintf(szBuff + strlen(szBuff), " (%s)\r\n",
					pMail->lpszReplyToName);
				else
					strcat(szBuff, "\r\n");
				VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)
			}

			// Subject:
			if (pMail->lpszSubject)
			{
			sprintf(szBuff, "Subject: %s\r\n", pMail->lpszSubject);
			VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)
			}

				/////////////////////////////////////////////////////////
				// 08/13/98 rlb
				// empty line needed after headers, RFC822
				strcpy(szBuff, "\r\n");
			VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)
				/////////////////////////////////////////////////////////

	LOG("SUBJ - ok\n");

				// send message text
				VERIFY_RET_VAL(Send(s, pMail->lpszMessage, \
				strlen(pMail->lpszMessage), 0);)

				// send message terminator and receive reply
				VERIFY_RET_VAL(Send(s, "\r\n.\r\n", 5, 0);)
				VERIFY_RET_VAL(Receive(s, szBuff, MAX_LINE_SIZE, 0, "250");)

	LOG("BODY - ok\n");
				// send QUIT message
				sprintf(szBuff, "QUIT\r\n");
			VERIFY_RET_VAL(Send(s, szBuff, strlen(szBuff), 0);)
				VERIFY_RET_VAL(Receive(s, szBuff, MAX_LINE_SIZE, 0, "221");)

				close(s);

	LOG("Done.\n");
			return 0;
}


// Send - send the request to the SMTP server, and handle errors.
/////////////////////////////////////////////////////////////////
int Send(int s, const char *lpszBuff, int nLen, int nFlags)
{
	int nCnt = 0;

	LOG("*SEND(%d) >> [%s]", nLen, ESC((char *)lpszBuff));

	while (nCnt < nLen)
	{
		int nRes = send(s, lpszBuff + nCnt, nLen - nCnt, nFlags);
		if (nRes == SOCKET_ERROR)
			return -3;
		else
			nCnt += nRes;
	}

	LOG(" - OK [%d]\n", nCnt);
	
	return 0;
}


// Receive - receive a reply from the SMTP server, and verify that
// the request has succeeded by checking against the specified
// reply code.
/////////////////////////////////////////////////////////////////
int Receive(int s, char *lpszBuff, int nLenMax, int nFlags, const char *lpszReplyCode)
{
	char *p;

	LOG("*RECV >> ");

	int nRes = recv(s, lpszBuff, nLenMax, nFlags);

	if (nRes == SOCKET_ERROR)
	{
		LOG("Failed\n");
		return -4;
	}
	else
		*(lpszBuff + nRes) = '\0';

	// check reply code for success/failure
	p = strtok(lpszBuff, "\n");
	while (p)
	{
		if (*(p + 3) == ' ')
		{
			if (!strncmp(p, lpszReplyCode, strlen(lpszReplyCode)))
			{
				LOG("OK ([%d], \"%s\")\n", nRes, ESC(lpszBuff));
				return 0;
			}
			else
			{
				int nErr = 1;

				sscanf(p, "%d", &nErr);
				LOG("Err (%d)\n", nErr);
				return -nErr;
			}
		}
		else
			p = strtok(NULL, "\n");
	}

	LOG("Done %d [%s]\n", nRes, ESC(lpszBuff));
	return -1;
}

