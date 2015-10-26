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

#include "urlc.h"

#define UCN "/home/olivka/bot/dat/urls.txt"

int main(int argc, char *argv[])
{
	//printf("argc=%d,argv[0]={%s},argv[1]={%s}", argc, argv[0]?argv[0]:"null", argv[1]?argv[1]:"null");
	url_cache url(UCN);
	url.dump_ssi(argv[1]);
	return 0;
}
