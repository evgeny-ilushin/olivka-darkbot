//#include "stdafx.h"
#include "bc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

int bc_numKnownNicks;
BCNICK bc_nicks[MAX_NICKS];

char bc_file_path[] = "/home/olivka/bot/dat/decay.bin";


void bc_timeAgo(long d, char *buf)
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

void bc_load()
{
    memset(&bc_nicks, 0, sizeof(bc_nicks));

    FILE *f = fopen(bc_file_path, "rb");
    if (f)
    {
	fread(&bc_numKnownNicks, sizeof(int), 1, f);
	fread(bc_nicks, sizeof(BCNICK), MAX_NICKS, f);
	fclose(f);
    }
}

void bc_save()
{
    FILE *f = fopen(bc_file_path, "wb");
    if (f)
    {
	fwrite(&bc_numKnownNicks, sizeof(int), 1, f);
	fwrite(bc_nicks, sizeof(BCNICK), MAX_NICKS, f);
	fclose(f);
    }
}

void bc_reset()
{
    bc_numKnownNicks = 0;
    bc_load();
}

int bc_count(char *text)
{
    char *m[] = { "хуй", "хуе", "хуи", "ху€", "залуп", "ебан", "ебин", "ебат", "ебал", "ебло", "ебис", "пизд", "писд", "песд", "пезд", "бл€", "блеа", "блиа", "бле€", "хуЄ", "Єба", "золуп", "дроч", "муд" };
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

int bc_update(char *nick, char *text)
{
    int cnt = bc_count(text);
    int found = -1;
    int l = strlen(text);
    int res = 1;
    long int t = (long int)time(0);

    for (int i = 0; i < bc_numKnownNicks; i++)
    {
	if (!strncasecmp(nick, bc_nicks[i].nick, MAX_NICK))
	{
	    found = i;
	    break;
	}
    }
    
    if (found != -1)
    {
	// Punish flooder
	if (cnt > BC_MAX_ONCE)
	{
		res = 0;
		bc_nicks[found].counter = (bc_nicks[found].counter >= BC_PUNISH?  bc_nicks[found].counter-BC_PUNISH : 0);
		bc_nicks[found].counter1 += l;
		bc_nicks[found].lastSeen = t;
	}
	else
	{
		if (cnt > 0)
		{
		    bc_nicks[found].counter += cnt;
		    bc_nicks[found].lastSeen = t;
		}
		bc_nicks[found].counter1 += l;
	}
    }
    else
    {
	if (bc_numKnownNicks < MAX_NICKS-1)
	{
	    bc_nicks[bc_numKnownNicks].counter = cnt;
	    bc_nicks[bc_numKnownNicks].counter1 = l;
	    bc_nicks[bc_numKnownNicks].lastSeen = t;
	    strcpy(bc_nicks[bc_numKnownNicks].nick, nick);
	    bc_numKnownNicks++;
	}
    }

    bc_save();
    return res;
}

int rnd(int max)
{
    return (int)(rand()/(RAND_MAX + 1.0)*max) + 1;
}

int bc_dice(char *nick, int sum)
{
    int found = -1;
    int res = 1;
    long int t = (long int)time(0);

    for (int i = 0; i < bc_numKnownNicks; i++)
    {
	if (!strncasecmp(nick, bc_nicks[i].nick, MAX_NICK))
	{
	    found = i;
	    break;
	}
    }
    
    if (found != -1)
    {
	//int r = rnd(3);
	int r = rnd(2);
	if (r == 1)
	{
	    if (sum <= bc_nicks[found].counter)
	    {
		bc_nicks[found].counter += sum;
		bc_nicks[found].lastSeen = t;
		res = 0;
	    }
	    else
		res = 2;
	}
	else if (r == 2)
	{
	    if (bc_nicks[found].counter >= sum)
	    {
		bc_nicks[found].counter -= sum;
		res = 1;
	    }
	    else
	    {
		bc_nicks[found].counter = 0;
		res = 2;
	    }
	    bc_nicks[found].lastSeen = t;
	}
	else
	{
	    res = 2;
	}
    }
    else
    {
	if (bc_numKnownNicks < MAX_NICKS-1)
	{
	    bc_nicks[bc_numKnownNicks].counter = 0;
	    bc_nicks[bc_numKnownNicks].counter1 = 0;
	    bc_nicks[bc_numKnownNicks].lastSeen = t;
	    strcpy(bc_nicks[bc_numKnownNicks].nick, nick);
	    bc_numKnownNicks++;
	}
    }

    bc_save();
    return res;
}

int bc_info5(char *buf)
{
    bc_sort();
    strcpy(buf, "пиздец");

    if (bc_numKnownNicks <= 0)
    {
	sprintf(buf, "пока данных нет");
    }
    else
    if (bc_numKnownNicks == 1)
    {
	sprintf(buf, "первыйнах - \2%s\2 [%u]", bc_nicks[0].nick, bc_nicks[0].counter);
    }
    else
    if (bc_numKnownNicks == 2)
    {
	sprintf(buf, "первыйнах - \2%s\2 [%u], второнахом %s [%u]", bc_nicks[0].nick, bc_nicks[0].counter, bc_nicks[1].nick, bc_nicks[1].counter);
    }
    else
    if (bc_numKnownNicks == 3)
    {
	sprintf(buf, "\2%s\2 [%u], %s [%u], %s [%u]", bc_nicks[0].nick, bc_nicks[0].counter, bc_nicks[1].nick, bc_nicks[1].counter, bc_nicks[2].nick, bc_nicks[2].counter);
    }
    else
    if (bc_numKnownNicks == 4)
    {
	sprintf(buf, "\2%s\2 [%u], %s [%u], %s [%u], %s [%u]", bc_nicks[0].nick, bc_nicks[0].counter, bc_nicks[1].nick, bc_nicks[1].counter, bc_nicks[2].nick, bc_nicks[2].counter, bc_nicks[3].nick, bc_nicks[3].counter);
    }
    else
    if (bc_numKnownNicks > 4)
    {
	sprintf(buf, "1: \2%s\2 [%u], 2: %s [%u], 3: %s [%u], 4: %s [%u], 5: %s [%u]", bc_nicks[0].nick, bc_nicks[0].counter, bc_nicks[1].nick, bc_nicks[1].counter, bc_nicks[2].nick, bc_nicks[2].counter, bc_nicks[3].nick, bc_nicks[3].counter, bc_nicks[4].nick, bc_nicks[4].counter);
    }

    return bc_numKnownNicks;
}

int bc_info10(char *buf)
{
    char buf2[1024];
    bc_info5(buf);

    if (bc_numKnownNicks > 10)
    {
	sprintf(buf2, ", 6: %s [%u], 7: %s [%u], 8: %s [%u], 9: %s [%u], 10: %s [%u]", bc_nicks[5].nick, bc_nicks[5].counter, bc_nicks[6].nick, bc_nicks[6].counter, bc_nicks[7].nick, bc_nicks[7].counter, bc_nicks[8].nick, bc_nicks[8].counter, bc_nicks[9].nick, bc_nicks[9].counter);
	strcat(buf, buf2);
    }

    return bc_numKnownNicks;
}


void bc_sort()
{
    if (bc_numKnownNicks < 2)
	return;

    for (int i = 0; i < bc_numKnownNicks-1; i++)
    {
	for (int i1 = 0; i1 < bc_numKnownNicks-1; i1++)
	{
	    if (bc_nicks[i1].counter < bc_nicks[i1+1].counter)
	    {
		BCNICK m;
		memcpy(&m, &bc_nicks[i1+1], sizeof(BCNICK));
		memcpy(&bc_nicks[i1+1], &bc_nicks[i1], sizeof(BCNICK));
		memcpy(&bc_nicks[i1], &m, sizeof(BCNICK));
	    }
	}
    }
}

int bc_info5_f(char *buf)
{
    bc_sort_f();
    strcpy(buf, "пиздец");

    if (bc_numKnownNicks <= 0)
    {
	strcpy(buf, "пока данных нет");
    }
    else
    if (bc_numKnownNicks == 1)
    {
	sprintf(buf, "первыйнах - \2%s\2 [%u]", bc_nicks[0].nick, bc_nicks[0].counter1);
    }
    else
    if (bc_numKnownNicks == 2)
    {
	sprintf(buf, "первыйнах - \2%s\2 [%u], второнахом %s [%u]", bc_nicks[0].nick, bc_nicks[0].counter1, bc_nicks[1].nick, bc_nicks[1].counter1);
    }
    else
    if (bc_numKnownNicks == 3)
    {
	sprintf(buf, "\2%s\2 [%u], %s [%u], %s [%u]", bc_nicks[0].nick, bc_nicks[0].counter1, bc_nicks[1].nick, bc_nicks[1].counter1, bc_nicks[2].nick, bc_nicks[2].counter1);
    }
    else
    if (bc_numKnownNicks == 4)
    {
	sprintf(buf, "\2%s\2 [%u], %s [%u], %s [%u], %s [%u]", bc_nicks[0].nick, bc_nicks[0].counter1, bc_nicks[1].nick, bc_nicks[1].counter1, bc_nicks[2].nick, bc_nicks[2].counter1, bc_nicks[3].nick, bc_nicks[3].counter1);
    }
    else
    if (bc_numKnownNicks > 4)
    {
	sprintf(buf, "1: \2%s\2 [%u], 2: %s [%u], 3: %s [%u], 4: %s [%u], 5: %s [%u]", bc_nicks[0].nick, bc_nicks[0].counter1, bc_nicks[1].nick, bc_nicks[1].counter1, bc_nicks[2].nick, bc_nicks[2].counter1, bc_nicks[3].nick, bc_nicks[3].counter1, bc_nicks[4].nick, bc_nicks[4].counter1);
    }

    return bc_numKnownNicks;
}

void bc_sort_f()
{
    if (bc_numKnownNicks < 2)
	return;

    for (int i = 0; i < bc_numKnownNicks-1; i++)
    {
	for (int i1 = 0; i1 < bc_numKnownNicks-1; i1++)
	{
	    if (bc_nicks[i1].counter1 < bc_nicks[i1+1].counter1)
	    {
		BCNICK m;
		memcpy(&m, &bc_nicks[i1+1], sizeof(BCNICK));
		memcpy(&bc_nicks[i1+1], &bc_nicks[i1], sizeof(BCNICK));
		memcpy(&bc_nicks[i1], &m, sizeof(BCNICK));
	    }
	}
    }
}

int bc_infoU(char *nick, char *buf)
{
    int found = -1;
    struct tm *sTm;
    struct tm *sTm1;
    char buff[128];

    bc_sort();

    for (int i = 0; i < bc_numKnownNicks; i++)
    {
	if (!strncasecmp(nick, bc_nicks[i].nick, MAX_NICK))
	{
	    found = i;
	    break;
	}
    }
    
    if (found != -1)
    {
	time_t now = (time_t)bc_nicks[found].lastSeen;
	time_t now1 = (time_t)time(0);
	sTm = localtime(&now);
	char b[128];
	bc_timeAgo(((long)now1)-((long)now), b);
	strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);
	if (bc_nicks[found].lastSeen == 0)
	{
	    sprintf(buf, "\2%s\2: є%d, %d points", nick, found+1, bc_nicks[found].counter);
	}
	else
	{
	    sprintf(buf, "\2%s\2: є%d, %d points, last updated %s ago (%s)", nick, found+1, bc_nicks[found].counter, b, buff);
	}
    }
    else
    {
	sprintf(buf, "\2%s\2: not found", nick);
    }
    return found;
}
