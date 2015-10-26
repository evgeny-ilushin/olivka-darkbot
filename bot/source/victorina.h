//////////////////////////////////////////////////////////////////////
// dbtools.h: DBFS interface for olivka
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_V_H__BE818ED9_F8E2_46D4_B2D8_53852E5A8A1E__INCLUDED_)
#define AFX_V_H__BE818ED9_F8E2_46D4_B2D8_53852E5A8A1E__INCLUDED_

#ifdef _WIN32
#include "stdafx.h"
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

#define MAX_DICT_SRC	40
#define MAX_DICT_DST	420
#define MAX_DICT_SEARCH	16

#define MAX_DICT_KEYS	4
#define MAX_DICT_KEY2	32
#define MAX_DICT_SRC2	64
#define MAX_DICT_DST2	200

#pragma pack(1)

typedef struct tagQREC
{
	char q[520];
	char a[32];
	int ql;
	int al;

} QREC, *LPQREC;


#pragma pack()


bool findAbyQ(int qs, char *q, char *a, bool bstrict)
{
    FILE *f = fopen("/home/olivka/bot/victorina/q3.db", "rt");
    if (!f) return false;
    char buf[10240] = "";
    while (fgets(buf, 10240, f))
    {
	int ll = strlen(buf);
	if (ll < 5)
	    continue;
	char *cq = buf+3;
	int cqs = buf[1]-'A';
	//printf("%s %d %d\n", buf, cqs, qs);
	if (cqs != qs)
	    continue;
	char *ca = strchr(cq, '|');
	if (!ca)
	    continue;
	*ca = 0;
	ca++;
	//printf("%s\n", a);
	if (bstrict)
	{
	    if (!strcmp(cq, q))
	    {
		strcpy(a, ca);
		fclose(f);
		return true;
	    }
	}
	else
	{
	    if (strstr(cq, q))
	    {
		strcpy(a, ca);
		fclose(f);
		return true;
	    }
	}
    
    }
    fclose(f);    
    return false;
}

#endif // !defined(AFX_DBTOOLS_H__BE818ED9_F8E2_46D4_B2D8_53852E5A8A1E__INCLUDED_)
