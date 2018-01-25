//#include "stdafx.h"
#include "bc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

int bc1_numKnownNicks;
BCNICK bc1_nicks[MAX_NICKS];

char bc1_file_path[] = "/home/olivka/bot/dat/decay-casino.bin";


void bc1_timeAgo(long d, char *buf)
{
    if (d < 60)
    {
        sprintf(buf, "%ds", d);
    }
    else
    if (d < 3600)
    {
        sprintf(buf, "%dm", d/60);
    }
    else
    if (d < 86400)
    {
        sprintf(buf, "%dh", d/3600);
    }
    else
    {
        sprintf(buf, "%dd", d/86400);
    }
}

void bc1_load()
{
    memset(&bc1_nicks, 0, sizeof(bc1_nicks));

    FILE *f = fopen(bc1_file_path, "rb");
    if (f)
    {
	fread(&bc1_numKnownNicks, sizeof(int), 1, f);
	fread(bc1_nicks, sizeof(BCNICK), MAX_NICKS, f);
	fclose(f);
    }
}

void bc1_save()
{
    FILE *f = fopen(bc1_file_path, "wb");
    if (f)
    {
	fwrite(&bc1_numKnownNicks, sizeof(int), 1, f);
	fwrite(bc1_nicks, sizeof(BCNICK), MAX_NICKS, f);
	fclose(f);
    }
}

void bc1_reset()
{
    bc1_numKnownNicks = 0;
    bc1_load();
}

int bc1_count(char *text)
{
    char *m[] = { "хуй", "хуе", "хуи", "ху€", "залуп", "ебан", "ебат", "ебал", "ебло", "ебис", "пизд", "писд", "песд", "пезд", "бл€", "блеа", "блиа", "бле€", "хуЄ", "Єба", "золуп", "дроч", "муд" };
    int num = (sizeof(m))/(sizeof(char *));
    int nc = 0;
    
    char text2[10240];
    strcpy(text2, text);

    char *t = text2;

    for (int i1 = 0; i1 < strlen(text2); i1++)
    {
	text2[i1] = tolower(text2[i1]);
    }

    for (int i = 0; i < num; i++)
    {
	t = text2;
	char *t1;
	do
	{
	    t1 = strstr(t, m[i]);
	    if (t1)
	    {
		t = t1+1;
		nc++;
	    }
	}
	while (t1);
    }
    return nc;
}

int bc1_update(char *nick, char *text)
{
    int cnt = bc1_count(text);
    int found = -1;
    int l = strlen(text);
    int res = 1;
    long int t = (long int)time(0);

    for (int i = 0; i < bc1_numKnownNicks; i++)
    {
	if (!strncasecmp(nick, bc1_nicks[i].nick, MAX_NICK))
	{
	    found = i;
	    break;
	}
    }
    
    if (found != -1)
    {
	// Punish flooder
	if (cnt > bc1_MAX_ONCE)
	{
		res = 0;
		bc1_nicks[found].counter = (bc1_nicks[found].counter >= bc1_PUNISH?  bc1_nicks[found].counter-bc1_PUNISH : 0);
		bc1_nicks[found].counter1 += l;
		bc1_nicks[found].lastSeen = t;
	}
	else
	{
		if (cnt > 0)
		{
		    bc1_nicks[found].counter += cnt;
		    bc1_nicks[found].lastSeen = t;
		}
		bc1_nicks[found].counter1 += l;
	}
    }
    else
    {
	if (bc1_numKnownNicks < MAX_NICKS-1)
	{
	    bc1_nicks[bc1_numKnownNicks].counter = cnt;
	    bc1_nicks[bc1_numKnownNicks].counter1 = l;
	    bc1_nicks[bc1_numKnownNicks].lastSeen = t;
	    strcpy(bc1_nicks[bc1_numKnownNicks].nick, nick);
	    bc1_numKnownNicks++;
	}
    }

    bc1_save();
    return res;
}

int rnd1(int max)
{
    return (int)(rand()/(RAND_MAX + 1.0)*max) + 1;
}

int bc1_dice(char *nick, int sum)
{
    int found = -1;
    int res = 1;
    long int t = (long int)time(0);

    for (int i = 0; i < bc1_numKnownNicks; i++)
    {
	if (!strncasecmp(nick, bc1_nicks[i].nick, MAX_NICK))
	{
	    found = i;
	    break;
	}
    }
    
    if (found != -1)
    {
	//int r = rnd(3);
	int r = rnd(3);
	if (r == 1)
	{
	    if (sum <= bc1_nicks[found].counter)
	    {
		bc1_nicks[found].counter += sum;
		bc1_nicks[found].lastSeen = t;
		res = 0;
	    }
	    else
		res = 2;
	}
	else if (r == 2)
	{
	    if (bc1_nicks[found].counter >= sum)
	    {
		bc1_nicks[found].counter -= sum;
		res = 1;
	    }
	    else
	    {
		bc1_nicks[found].counter = 0;
		res = 2;
	    }
	    bc1_nicks[found].lastSeen = t;
	}
	else
	{
	    res = 2;
	}
    }
    else
    {
	if (bc1_numKnownNicks < MAX_NICKS-1)
	{
	    bc1_nicks[bc1_numKnownNicks].counter = 0;
	    bc1_nicks[bc1_numKnownNicks].counter1 = 0;
	    bc1_nicks[bc1_numKnownNicks].lastSeen = t;
	    strcpy(bc1_nicks[bc1_numKnownNicks].nick, nick);
	    bc1_numKnownNicks++;
	}
    }

    bc1_save();
    return res;
}

int bc1_info5(char *buf)
{
    bc1_sort();
    strcpy(buf, "пиздец");

    if (bc1_numKnownNicks <= 0)
    {
	sprintf(buf, "пока данных нет");
    }
    else
    if (bc1_numKnownNicks == 1)
    {
	sprintf(buf, "первыйнах - \2%s\2 [%u]", bc1_nicks[0].nick, bc1_nicks[0].counter);
    }
    else
    if (bc1_numKnownNicks == 2)
    {
	sprintf(buf, "первыйнах - \2%s\2 [%u], второнахом %s [%u]", bc1_nicks[0].nick, bc1_nicks[0].counter, bc1_nicks[1].nick, bc1_nicks[1].counter);
    }
    else
    if (bc1_numKnownNicks == 3)
    {
	sprintf(buf, "\2%s\2 [%u], %s [%u], %s [%u]", bc1_nicks[0].nick, bc1_nicks[0].counter, bc1_nicks[1].nick, bc1_nicks[1].counter, bc1_nicks[2].nick, bc1_nicks[2].counter);
    }
    else
    if (bc1_numKnownNicks == 4)
    {
	sprintf(buf, "\2%s\2 [%u], %s [%u], %s [%u], %s [%u]", bc1_nicks[0].nick, bc1_nicks[0].counter, bc1_nicks[1].nick, bc1_nicks[1].counter, bc1_nicks[2].nick, bc1_nicks[2].counter, bc1_nicks[3].nick, bc1_nicks[3].counter);
    }
    else
    if (bc1_numKnownNicks > 4)
    {
	sprintf(buf, "1: \2%s\2 [%u], 2: %s [%u], 3: %s [%u], 4: %s [%u], 5: %s [%u]", bc1_nicks[0].nick, bc1_nicks[0].counter, bc1_nicks[1].nick, bc1_nicks[1].counter, bc1_nicks[2].nick, bc1_nicks[2].counter, bc1_nicks[3].nick, bc1_nicks[3].counter, bc1_nicks[4].nick, bc1_nicks[4].counter);
    }

    return bc1_numKnownNicks;
}

int bc1_info10(char *buf)
{
    char buf2[1024];
    bc1_info5(buf);

    if (bc1_numKnownNicks > 10)
    {
	sprintf(buf2, ", 6: %s [%u], 7: %s [%u], 8: %s [%u], 9: %s [%u], 10: %s [%u]", bc1_nicks[5].nick, bc1_nicks[5].counter, bc1_nicks[6].nick, bc1_nicks[6].counter, bc1_nicks[7].nick, bc1_nicks[7].counter, bc1_nicks[8].nick, bc1_nicks[8].counter, bc1_nicks[9].nick, bc1_nicks[9].counter);
	strcat(buf, buf2);
    }

    return bc1_numKnownNicks;
}


void bc1_sort()
{
    if (bc1_numKnownNicks < 2)
	return;

    for (int i = 0; i < bc1_numKnownNicks-1; i++)
    {
	for (int i1 = 0; i1 < bc1_numKnownNicks-1; i1++)
	{
	    if (bc1_nicks[i1].counter < bc1_nicks[i1+1].counter)
	    {
		BCNICK m;
		memcpy(&m, &bc1_nicks[i1+1], sizeof(BCNICK));
		memcpy(&bc1_nicks[i1+1], &bc1_nicks[i1], sizeof(BCNICK));
		memcpy(&bc1_nicks[i1], &m, sizeof(BCNICK));
	    }
	}
    }
}

int bc1_info5_f(char *buf)
{
    bc1_sort_f();
    strcpy(buf, "пиздец");

    if (bc1_numKnownNicks <= 0)
    {
	strcpy(buf, "пока данных нет");
    }
    else
    if (bc1_numKnownNicks == 1)
    {
	sprintf(buf, "первыйнах - \2%s\2 [%u]", bc1_nicks[0].nick, bc1_nicks[0].counter1);
    }
    else
    if (bc1_numKnownNicks == 2)
    {
	sprintf(buf, "первыйнах - \2%s\2 [%u], второнахом %s [%u]", bc1_nicks[0].nick, bc1_nicks[0].counter1, bc1_nicks[1].nick, bc1_nicks[1].counter1);
    }
    else
    if (bc1_numKnownNicks == 3)
    {
	sprintf(buf, "\2%s\2 [%u], %s [%u], %s [%u]", bc1_nicks[0].nick, bc1_nicks[0].counter1, bc1_nicks[1].nick, bc1_nicks[1].counter1, bc1_nicks[2].nick, bc1_nicks[2].counter1);
    }
    else
    if (bc1_numKnownNicks == 4)
    {
	sprintf(buf, "\2%s\2 [%u], %s [%u], %s [%u], %s [%u]", bc1_nicks[0].nick, bc1_nicks[0].counter1, bc1_nicks[1].nick, bc1_nicks[1].counter1, bc1_nicks[2].nick, bc1_nicks[2].counter1, bc1_nicks[3].nick, bc1_nicks[3].counter1);
    }
    else
    if (bc1_numKnownNicks > 4)
    {
	sprintf(buf, "1: \2%s\2 [%u], 2: %s [%u], 3: %s [%u], 4: %s [%u], 5: %s [%u]", bc1_nicks[0].nick, bc1_nicks[0].counter1, bc1_nicks[1].nick, bc1_nicks[1].counter1, bc1_nicks[2].nick, bc1_nicks[2].counter1, bc1_nicks[3].nick, bc1_nicks[3].counter1, bc1_nicks[4].nick, bc1_nicks[4].counter1);
    }

    return bc1_numKnownNicks;
}

void bc1_sort_f()
{
    if (bc1_numKnownNicks < 2)
	return;

    for (int i = 0; i < bc1_numKnownNicks-1; i++)
    {
	for (int i1 = 0; i1 < bc1_numKnownNicks-1; i1++)
	{
	    if (bc1_nicks[i1].counter1 < bc1_nicks[i1+1].counter1)
	    {
		BCNICK m;
		memcpy(&m, &bc1_nicks[i1+1], sizeof(BCNICK));
		memcpy(&bc1_nicks[i1+1], &bc1_nicks[i1], sizeof(BCNICK));
		memcpy(&bc1_nicks[i1], &m, sizeof(BCNICK));
	    }
	}
    }
}

int bc1_infoU(char *nick, char *buf)
{
    int found = -1;
    struct tm *sTm;
    struct tm *sTm1;
    char buff[128];

    bc1_sort();

    for (int i = 0; i < bc1_numKnownNicks; i++)
    {
	if (!strncasecmp(nick, bc1_nicks[i].nick, MAX_NICK))
	{
	    found = i;
	    break;
	}
    }
    
    if (found != -1)
    {
	time_t now = (time_t)bc1_nicks[found].lastSeen;
	time_t now1 = (time_t)time(0);
	sTm = localtime(&now);
	char b[128];
	bc1_timeAgo(((long)now1)-((long)now), b);
	strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
	if (bc1_nicks[found].lastSeen == 0)
	{
	    sprintf(buf, "\2%s\2: є%d, %d points", nick, found+1, bc1_nicks[found].counter);
	}
	else
	{
	    sprintf(buf, "\2%s\2: є%d, %d points, last updated %s ago (%s)", nick, found+1, bc1_nicks[found].counter, b, buff);
	}
    }
    else
    {
	sprintf(buf, "\2%s\2: not found", nick);
    }
    return found;
}
