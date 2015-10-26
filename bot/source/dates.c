#include <stdio.h>
#include <time.h>
#include <string.h>

#ifndef __cplusplus
typedef enum
{ false, true }
bool;
#endif

#include "dates.h"

#include <ctype.h>
#include <stdio.h>
#if     NEED_CLOCK_T == ON
#include <time.h>
#endif
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
#ifdef	WIN32
#include <windows.h>
#endif
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>

void S(const char *format, ...);

bool parseDate(char *text2, LPDATE_INFO lpDi)
{
	char text[1024];
	strncpy(text, text2, 1023);

	char *d = text;
	char *m = strchr(d, '/');
	char *y;
	time_t localTime;
	struct tm timeStruct;

	memset(lpDi, 0, sizeof(DATE_INFO));

	if (!m)
		return false;

	*m = 0;
	m++;

	y = strchr(m, '/');

	if (y)
	{
		*y = 0;
		y++;

		lpDi->year = (unsigned short)atoi(y);
	}
	else
		lpDi->year = 0;

	lpDi->day = (unsigned char)atoi(d);
	lpDi->month = (unsigned char)atoi(m);

	localTime = time(NULL);
	timeStruct = *localtime(&localTime);

	timeStruct.tm_mday = lpDi->day;
	timeStruct.tm_mon = lpDi->month;
	timeStruct.tm_year = lpDi->year == 0? 0 : lpDi->year-1900;

	localTime = mktime(&timeStruct);
	timeStruct = *localtime(&localTime);

	lpDi->weekDay = localtime(&localTime)->tm_wday;

	return true;
}

int getDateRecords(LPDATE_INFO lpDiSet, int maxCount, time_t now_t)
{
	struct tm *now;
	DATE_INFO di;
	FILE *f_dbase = fopen(DATES_FILE_PATH, "rb");
	bool isValid;
	int counter = 0;
	DATE_INFO_HEADER header;

	if (now_t < 0)
		return 0;

	if (now_t == (time_t)NULL)
		now_t= time(NULL);

	now = localtime(&now_t);

	if (!f_dbase)
		return 0;

	readHeader(f_dbase, &header);
	fseek(f_dbase, sizeof(DATE_INFO_HEADER), SEEK_SET);

	while (fread(&di, sizeof(DATE_INFO), 1, f_dbase) == 1)
	{
		isValid = false;
		
		if (di.day == now->tm_mday && di.month == now->tm_mon+1 && di.type == MATCH_MONTH)
			isValid = true;
		
		if (di.weekDay == now->tm_wday && di.type == MATCH_WEEKDAY)
			isValid = true;

		if (isValid && counter < maxCount)
		{
			memcpy(&lpDiSet[counter], &di, sizeof(DATE_INFO));
			counter++;
		}

		if (counter >= maxCount)
			break;
	}

	fclose(f_dbase);
	return counter;
}

// Format: "xx/xx/xx [w or m] Desc"
bool addDateRecordFromQuery(char *query, int *id, LPDATE_INFO pDi, char *nick)
{
	char buffer[2048];
	char *pbuffer = buffer;
	char *t, *d;
	DATE_INFO di;

	strncpy(buffer, query, 2047);
	trim(&pbuffer);
	
	t = strchr(buffer, ' ');

	if (!t)
		return false;

	*t = 0; t++;

	d = strchr(t, ' ');

	if (!d)
		return false;

	*d = 0; d++;

	memset(&di, 0, sizeof(DATE_INFO));

	if (!parseDate(buffer, &di))
		return false;

	trim(&t);
	trim(&d);

	if (!strcmp(t, "m") || !strcmp(t, "M"))
		di.type = (unsigned char)MATCH_MONTH;
	else
	if (!strcmp(t, "w") || !strcmp(t, "W"))
		di.type = (unsigned char)MATCH_WEEKDAY;
	else
		return false;

	strncpy(di.text, d, MAX_DATE_INFO2);
	di.text[MAX_DATE_INFO2-1] = 0;
	strncpy(di.nick, nick, 15);
	di.nick[15] = 0;
	di.created = time(0);

	return addDateRecord(&di, id, pDi);
}

bool addDateRecord(LPDATE_INFO lpDi, int *id, LPDATE_INFO pDi)
{
	FILE *f_dbase = fopen(DATES_FILE_PATH, "r+b");
	bool result = false;
	DATE_INFO_HEADER header;

	if (pDi)
		memcpy(pDi, lpDi, sizeof(DATE_INFO));

	if (!f_dbase)
		return false;

	readHeader(f_dbase, &header);
	header.lastId++;

	lpDi->id = header.lastId;
	*id = header.lastId;

	fseek(f_dbase, 0, SEEK_END);
	result = fwrite(lpDi, sizeof(DATE_INFO), 1, f_dbase) == 1;
	
	header.numEntries++;

	writeHeader(f_dbase, &header);

	fclose(f_dbase);
	return result;
}

int trim(char **pText)
{
	char *text = *pText;
	
	int lineEnd = strlen(text)-1;

	if (lineEnd > 0)
	{
		while (lineEnd >= 0 && (text[lineEnd] == ' ' || text[lineEnd] == '\n' || text[lineEnd] == '\r' || text[lineEnd] == '\t'))
		{
			text[lineEnd] = 0;
			lineEnd--;
		}

		while (lineEnd >= 0 && (text[0] == ' ' || text[0] == '\n' || text[0] == '\r' || text[0] == '\t'))
		{
			text++;
			lineEnd--;
		}
	}

	*pText = text;
	
	return lineEnd+1;
}

int trim2(char **pText)
{
	char *text = *pText;
	
	int lineEnd = strlen(text)-1;

	if (lineEnd > 0)
	{
		while (lineEnd >= 0 && (text[lineEnd] == ' ' || text[lineEnd] == '.' || text[lineEnd] == '\n' || text[lineEnd] == '\r' || text[lineEnd] == '\t'))
		{
			text[lineEnd] = 0;
			lineEnd--;
		}

		while (lineEnd >= 0 && (text[0] == ' ' || text[0] == '.' || text[0] == '\n' || text[0] == '\r' || text[0] == '\t'))
		{
			text++;
			lineEnd--;
		}
	}

	*pText = text;
	
	return lineEnd+1;
}

bool readHeader(FILE *f, LPDATE_INFO_HEADER h)
{
	long current = ftell(f);
	bool result = false;
	
	fseek(f, 0, SEEK_SET);
	result = fread(h, sizeof(DATE_INFO_HEADER), 1, f) == 1;
	fseek(f, current, SEEK_SET);

	return result;
}

bool writeHeader(FILE *f, LPDATE_INFO_HEADER h)
{
	long current = ftell(f);
	bool result = false;

	memcpy(h->logo, "PDS", 3);
	
	fseek(f, 0, SEEK_SET);
	result = fwrite(h, sizeof(DATE_INFO_HEADER), 1, f) == 1;
	fseek(f, current, SEEK_SET);

	return result;
}

bool delDateRecord(int id)
{
	DATE_INFO di;
	char bkFilePath[1024];
	FILE *f_dbase, *f_dbaseBk;
	DATE_INFO_HEADER header;
	bool dateExists = false;
	int numRecords = 0;

	f_dbase = fopen(DATES_FILE_PATH, "rb");

	if (!f_dbase)
		return 0;

	readHeader(f_dbase, &header);
	fseek(f_dbase, sizeof(DATE_INFO_HEADER), SEEK_SET);

	while (fread(&di, sizeof(DATE_INFO), 1, f_dbase) == 1)
	{
		if (di.id == id)
		{
			dateExists = true;
			break;
		}
	}

	if (dateExists)
	{
		f_dbaseBk = fopen(DATES_FILEBK_PATH, "w+b");

		if (!f_dbaseBk)
		{
			fclose(f_dbase);
			return false;
		}
		
		writeHeader(f_dbaseBk, &header);

		fseek(f_dbase, sizeof(DATE_INFO_HEADER), SEEK_SET);
		fseek(f_dbaseBk, sizeof(DATE_INFO_HEADER), SEEK_SET);

		while (fread(&di, sizeof(DATE_INFO), 1, f_dbase) == 1)
		{
			if (di.id != id)
			{
				fwrite(&di, sizeof(DATE_INFO), 1, f_dbaseBk);
				numRecords++;
			}
		}

		header.numEntries = numRecords;
		writeHeader(f_dbaseBk, &header);

		fclose(f_dbase);
		fclose(f_dbaseBk);

		unlink(DATES_FILE_PATH);
		rename(DATES_FILEBK_PATH, DATES_FILE_PATH);
		
		return true;
	}
	else
		fclose(f_dbase);
	
	return false;
}

// +30/03/2007
bool updateDateRecord(LPDATE_INFO lpdi)
{
	DATE_INFO di;
	FILE *f_dbase;
	DATE_INFO_HEADER header;
	bool dateExists = false;

	if (!lpdi)
		return false;

	f_dbase = fopen(DATES_FILE_PATH, "r+b");

	if (!f_dbase)
		return false;

	readHeader(f_dbase, &header);
	fseek(f_dbase, sizeof(DATE_INFO_HEADER), SEEK_SET);

	while (fread(&di, sizeof(DATE_INFO), 1, f_dbase) == 1)
	{
		if (di.id == lpdi->id)
		{
			dateExists = true;
			break;
		}
	}

	if (dateExists)
	{
		fseek(f_dbase, -((long)sizeof(DATE_INFO)), SEEK_CUR);
		lpdi->day = di.day;
		lpdi->month = di.month;
		lpdi->type = di.type;
		lpdi->weekDay = di.weekDay;
		lpdi->year = di.year;
		fwrite(lpdi, sizeof(DATE_INFO), 1, f_dbase);
		fseek(f_dbase, 0, SEEK_END);
		fclose(f_dbase);
		return true;
	}
	else
	{
		fseek(f_dbase, 0, SEEK_END);
		fclose(f_dbase);
	}
	
	return false;
}

bool updateDateRecordFromQuery(char *query, int *id, char *nick)
{
	char buffer[1024];
	char *pbuffer = buffer;
	char *t, *d;
	DATE_INFO di;

	strncpy(buffer, query, 1024);
	trim(&pbuffer);
	
	t = query;
	trim(&t);

	d = strchr(t, ' ');

	if (!d)
		return false;

	*d = 0; d++;
	trim(&d);

	if (strlen(d) < 2)
		return false;

	memset(&di, 0, sizeof(DATE_INFO));

	di.id = atol(t);
	*id = di.id;
	strncpy(di.text, d, MAX_DATE_INFO2);
	di.text[MAX_DATE_INFO2-1] = 0;
	strncpy(di.nick, nick, 15);
	di.nick[15] = 0;
	di.created = time(0);

	return updateDateRecord(&di);
}

bool getDayName(int dayNum, char *buffer)
{
	char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

	if (dayNum < 0 || dayNum > 6)
		return false;

	strcpy(buffer, days[dayNum]);

	return true;
}
