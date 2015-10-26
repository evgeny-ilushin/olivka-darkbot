#include <stdio.h>
#include <string.h>

#ifndef __cplusplus
typedef enum
{ false, true }
bool;
#endif

#include "color.h"

int nrand(int);

int lastRndIndex = -1;
int rndIndex = -1;

int secureRand()
{
	rndIndex = nrand(15);

	while (rndIndex == lastRndIndex)
		rndIndex = nrand(15);

	lastRndIndex = rndIndex;

	return rndIndex;
}

void color(char *text)
{
	if (strstr(text, " ") || strlen(text) > 20)
		colorTextEx(text);
	else
	{
		if (nrand(10) == 1)
			colorTextEx(text);
		else
			if (nrand(10) == 1)
				colorText(text);
			else
				colorWord(text);
	}
}

void colorText(char *text)
{
	char ctext[1024];
	int cNdx = secureRand();
	int colorPareA[16] = { 4, 0, 11, 8,  8,  11, 13, 9,  8, 9, 13, 13, 12, 12, 8,  13 };
	int colorPareB[16] = { 8, 9, 13, 13, 12, 12, 8,  13, 4, 0, 11, 8,  8,  11, 13, 9 };

	sprintf(ctext, "\3%d,%d %s \2", colorPareA[cNdx], colorPareB[cNdx], text);
	strcpy(text, ctext);
}

void colorTextEx(char *text)
{
	char ctext[1024], color[32];
	int i = 0, cLen, tLen = strlen(text), cNdx = secureRand();
	int colorPareA[16] = { 4, 0, 11, 8,  8,  11, 13, 9,  8, 9, 13, 13, 12, 12, 8,  13 };
	int colorPareB[16] = { 8, 9, 13, 13, 12, 12, 8,  13, 4, 0, 11, 8,  8,  11, 13, 9 };

	sprintf(ctext, "\3%d,%d ", colorPareA[cNdx], colorPareB[cNdx]);

	for (i = 0; i < tLen; i++)
	{
		cLen = strlen(ctext);
		
		ctext[cLen] = text[i];
		ctext[cLen+1] = 0;

		if (text[i] == ' ' || text[i] == '\t')
		{
			cNdx = secureRand();
			sprintf(color, "\3%d,%d ", colorPareA[cNdx], colorPareB[cNdx]);
			strcat(ctext, color);
		}
	}

	sprintf(text, "%s \2", ctext);
}

void colorWord(char *text)
{
	char ctext[1024], color[32];
	int i = 0, tLen = strlen(text), cNdx;
	int colorPareA[16] = { 4, 0, 11, 8,  8,  11, 13, 9,  8, 9, 13, 13, 12, 12, 8,  13 };
	int colorPareB[16] = { 8, 9, 13, 13, 12, 12, 8,  13, 4, 0, 11, 8,  8,  11, 13, 9 };

	strcpy(ctext, "");
	secureRand();

	for (i = 0; i < tLen; i++)
	{
		cNdx = secureRand();
		
		if (!i)
			sprintf(color, "\3%d,%d %c", colorPareA[cNdx], colorPareB[cNdx], text[i]);
		else
			sprintf(color, "\3%d,%d%c", colorPareA[cNdx], colorPareB[cNdx], text[i]);

		strcat(ctext, color);
	}

	sprintf(text, "%s \2", ctext);
}
