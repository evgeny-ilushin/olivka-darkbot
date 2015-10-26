#include "seen.h"

#include <stdio.h>
#include <string.h>

int seenBy(char *text, char res[][256], int *nres, int pos, bool noIdent)
{
	FILE *f = fopen("/home/olivka/bot/dat/seen.db", "rt");
	if (!f) return 0;
	char line[1024];
	int nCnt = 0;
	while (fgets(line, 1023, f))
	{
		char *n = line;
		while (*n == ' ') n++;
		char *a = n;
		while (*a != ' ') a++;
		*a = 0; a++;
		char *c = a;
		while (*c != ' ') c++;
		*c = 0; c++;
		if (noIdent)
		{
			char *a1 = a;
			while (*a1 != '@' && *a1 != 0) a1++;
			*a1 = 0;
			a = a1+1;
		}
		//printf("[%s] [%s]\n", n, a);
		if ((pos == 0 && !strcmp(n, text)) || (pos == 1 && !strcmp(a, text)))
		{
			//printf("MATCH: [%s] [%s] -> %s\n", n, a, text);
			
			if (nCnt >= *nres) break;
			char *r = (pos == 0? a : n);
			//printf("CPY %s to %d\n", r, nCnt);
			strcpy(res[nCnt++], r);
		}
	}
	fclose(f);
	*nres = nCnt;
	return nCnt;
}

int seenByNick(char *nick, char res[][256], int *nres, bool noIdent)
{
	return seenBy(nick, res, nres, 0, noIdent);
}

int seenByAddr(char *addr, char res[][256], int *nres, bool noIdent)
{
	return seenBy(addr, res, nres, 1, noIdent);
}

int nicksByAddr(char *addr, char *res, bool noIdent)
{
	char r[100][256];
	int nr = 100;
	int nr1 =  seenBy(addr, r, &nr, 1, noIdent);
	if (nr1 <= 0)
	{
		*res = 0;
		return 0;
	}
	*res = 0;
	for (int i = 0; i < nr1; i++)
	{
		strcat(res, r[i]);
		if (i < nr1-1)
			strcat(res, ", ");
	}
	return nr1;
}

int nicksByAddrEx(char *addr, char *res)
{
	return nicksByAddr(addr, res, strchr(addr, '@') == NULL);
}

