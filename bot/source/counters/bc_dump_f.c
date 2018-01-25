// url_dump.cpp : Defines the entry point for the console application.
//

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

#include "bc.h"

#define UCN "/home/olivka/bot/dat/urls.txt"

int main(int argc, char *argv[])
{
	int c = 1;
	bc_load();
	bc_sort_f();

//	printf("<table border=1>");

	for (int i = 0; i < MAX_NICKS; i++)
	{
	    if (bc_nicks[i].counter > 0)
	    {
		printf("<tr><td>%d</td><td><b>%s</b></td><td align=right>%d</td><td align=right>%d</td></tr>\n", c++, bc_nicks[i].nick, bc_nicks[i].counter, bc_nicks[i].counter1);
	    }
	}

	return 0;
}
