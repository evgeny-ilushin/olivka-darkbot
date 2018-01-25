#include <ctype.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#include "tg.h"

char tnick[512];
char tchannel[512];
char ttext[10240];
char *z;

int TG_REPLY = 0;
 
void substituteTgMessage(char *nick, char *channel, char *text)
{
    char *x;
    TG_REPLY = 0;

    //printf("Initially:\n");
    //printf(" * nick: %s\n", nick);
    //printf(" * channel: %s\n", channel);
    //printf(" * text: %s\n", text);

    if (strcmp(channel, "#123456") == 0 || strcmp(channel, "#telegram") == 0)
    {
	//printf("8\n");
	if (strcmp(nick, "tg") == 0 || strcmp(nick, "tg!tg@redip.ru") == 0)
	{
		//printf("0\n");
		if (strlen(text) > 5)
		{
		    //printf("TG!!!\n");
		    if (text[0] == ':' && text[1] == '[' && text[2] == '\3' && isdigit(text[3]) && isdigit(text[4]))
		    {
			//printf("1\n");
			strcpy(ttext, text);
			strcpy(tchannel, channel);
			char *n = ttext+5;
			x = n;
			
			while (*n != 0 && *n != '\3')
			{
			    n++;
			}
			n++;
			z=n-1;
			if (*n != 0 && *n == ']')
			{
			    n++;
			    if (*n != 0)
			    {
				char *t = n;
				*z = 0;
				while (*t != 0 && (*t == ' ' || *t == '\t')) { t++; }
				//printf("3: {%s}\n", t);
				//printf("4: {%s}\n", x);
				//x--;
				//*x  = '@'; // TG prefix
				printf("**** r-rewrite: {%s} {%s}\n", t, x);
				*text = ':';
				strcpy(text+1, t);
				strcpy(z, "!tg@redip.ru");
				*nick = '@';
				strcpy(nick+1, x);
				strcpy(channel, tchannel);
				TG_REPLY = 1;
			    }
			}
			
			
		    }
		}
	}
    }

    // : )

    //printf("--> nick: %s\n", nick);
    //printf("--> channel: %s\n", channel);
    //printf("--> text: %s\n", text);


    fflush(stdout);
}

