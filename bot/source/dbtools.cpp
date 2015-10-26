// dbtools.cpp : DBFS interface for olivka : implementation file
//

//#include "stdafx.h"

#include "dbtools.h"
#include "langtools.h"

#define APPEND(s, c) { *s = c; s++; }

char _rdbfile::logo[] = "DICTIONARY V2.0";

bool _rdbfile::add(char *src, char *dst)
{
	return false;
}

bool _rdbfile::open()
{
	if (!_file::open())
		return false;

	return readHeader();
}

bool _rdbfile::create(int recSize)
{
	if (!_file::open("w+"))
		return false;

	memset(&header, 0, sizeof(DICTHEADER));
	header.created = time(0);
	header.numRecords = 0;
	header.recordSize = recSize;
	strncpy(header.logo, _rdbfile::logo, sizeof(header.logo));

	return writeHeader();
}

bool _rdbfile::openRW()
{
	if (!_file::open("r+"))
		return false;

	return readHeader();
}

bool _rdbfile::readHeader()
{
	rewind();
	if (read(&header, sizeof(DICTHEADER)) != sizeof(DICTHEADER))
	{
		memset(&header, 0, sizeof(DICTHEADER));
		return false;
	}

	return strncmp(header.logo, _rdbfile::logo, sizeof(DICTHEADER)) == 0;
}

bool _rdbfile::writeHeader()
{
	rewind();
	return write(&header, sizeof(DICTHEADER)) == sizeof(DICTHEADER);
}

bool _rdbfile::readAt(unsigned i, void *r)
{
	if (i < 0 || i >= header.numRecords)
		return false;
	rewind();
	fpos_t offset = { sizeof(DICTHEADER)+header.recordSize*i, 0 };
	int ret = fsetpos(iofile, &offset);
	if (ftell() != offset.__pos)
		return false;
	return read(r, header.recordSize) == header.recordSize;
}

bool _hashfile::find(char *word, LPDICTREC result, bool matchCase)
{
	fpos_t offset = { sizeof(DICTHEADER), 0 };
	int ret = fsetpos(iofile, &offset);
	bool found = false;
	
	if (ftell() != offset.__pos)
		return false;

	do
	{
		if (read(result, header.recordSize) != header.recordSize)
			break;
		if (matchCase)
		{
		    if (!strcmp(result->source, word))
			    return true;
		}
		else
		{
		    if (!strcasecmp(result->source, word))
			    return true;
		}
	}
	while (!found);

	if (!found)
		memset(result, 0, header.recordSize);

	return found;
}

bool _hashfile::add(char *word, char *data)
{
    return setKey(word, data, false);
}

bool _hashfile::del(char *word)
{
    return setKey(word, NULL, true);
}

bool _hashfile::setKey(char *word, char *data, bool disable)
{
	if (!word)
	    return false;

	if (!data && !disable)
	    return false;
	    
	int wl = strlen(word);
	int dl = disable? 0:strlen(data);
	
	if (wl >= MAX_DICT_SRC) return false;
	if (dl >= MAX_DICT_DST) data[MAX_DICT_DST-1] = 0;
	
	DICTREC rec, recf;
	LPDICTREC result = &recf;
	//bool update = find(word, &rec);
	memset(&rec, 0, sizeof(DICTREC));
	memset(&recf, 0, sizeof(DICTREC));
	strcpy(rec.source, word);

	if (!disable)
	    strcpy(rec.destination, data);
	
	fpos_t offset = { sizeof(DICTHEADER), 0 };
	int ret = fsetpos(iofile, &offset);
	bool OK = false;
	
	if (ftell() != offset.__pos)
		return false;
		
	int ofs = sizeof(DICTHEADER);

	do
	{
		if (read(result, header.recordSize) != header.recordSize)
			break;
		
		// skip disabled
		if (!(*result->source))
		    continue;
		    
		printf("  *CMP: [%s] -> [%s] [%08X]\n", word, result->source, ofs);
		if (!strcasecmp(result->source, word))
		{
		    printf("FOUND DUPE: [%s] at #%d\n", word, ofs);
		    
		    if (disable)
		    {
			printf("DEL #08X\n", ofs);
			fpos_t loffset = { ofs	, 0 };
			int lret = fsetpos(iofile, &loffset);
			*result->source = 0;
			return header.recordSize == write(result, header.recordSize);
		    }
		    else
		    {
			memcpy(result->destination, rec.destination, MAX_DICT_DST);
			printf("UPDATE #08X\n", ofs);
			fpos_t loffset = { ofs	, 0 };
			int lret = fsetpos(iofile, &loffset);
			if (header.recordSize == write(result, header.recordSize))
			{
			    // header.numRecords++;
			    return writeHeader();
			}
			else
			    return false;
		    }
		}
		
		ofs += header.recordSize;
	}
	while (true);

	if (header.recordSize == write(&rec, header.recordSize))
	{
	    header.numRecords++;
	    return writeHeader();
	}
	return false;
}

bool _dictfile::findFirst(char *word, LPDICTREC result)
{
	fpos_t offset = { sizeof(DICTHEADER), 0 };
	int ret = fsetpos(iofile, &offset);
	if (ftell() != offset.__pos)
		return false;

	return findNext(word, result);
}

bool _dictfile::findNext(char *word, LPDICTREC result)
{
	bool found = false;
	
	do
	{
		if (read(result, header.recordSize) != header.recordSize)
			break;
		if (!strcmp(result->source, word))
			return true;
	}
	while (!found);

	if (!found)
		memset(result, 0, header.recordSize);

	return found;
}

bool _dictfile::findLike(char *word, LPDICTSEARCH result)
{
	bool found = false;
	DICTREC rec;
	
	result->numRecords = 0;

	fpos_t offset = { sizeof(DICTHEADER), 0 };
	int ret = fsetpos(iofile, &offset);
	if (ftell() != offset.__pos)
		return false;

	do
	{
		if (read(&rec, header.recordSize) != header.recordSize)
			break;
		if (strstr(rec.source, word))
		{
			if (result->numRecords < MAX_DICT_SEARCH)
			{
				memcpy(result->source[result->numRecords], rec.source, MAX_DICT_SRC);
				result->numRecords++;
				found = true;
			}
		}
	}
	while (result->numRecords < MAX_DICT_SEARCH);

	return found;
}

int _dictfile::parseDictLine(char *src, char *dst)
{
	char *s = src;
	char *d = dst;
	
	*d = 0;
	int sLen = strlen(s);

	bool fBold = false;
	bool fUnd = false;
	
	while (*s)
	{
		// BOLD ->
		if (isRuAlpha(*s) && !fBold)
		{
			APPEND(d, '\2');
			fBold = true;
		}
		else
		// BOLD -<
		if (fBold && !isRuAlpha(*s) && *s != ' ' && *s != ',' && *s != '-' && *s != ':' && *s != '(' && *s != ')')
		{
			APPEND(d, '\2');
			fBold = false;
		}
		// []/[i]/[/i]
		if (*s == '[' && s-src+3 < sLen)
		{
			if (((*(s+1)) == 'i' || (*(s+1)) == 'b') && (*(s+2)) == ']')
			{
				APPEND(d, '\x1F');
				fUnd = true;
				s += 3;
			}
			else
			if ((*(s+1)) == '/' && ((*(s+2)) == 'i' || (*(s+2)) == 'b') && (*(s+3)) == ']')
			{
				APPEND(d, '\x1F');
				fUnd = false;
				s += 4;
			}
			else
			{
				APPEND(d, '[');
				APPEND(d, '\2');
				s++;
				fUnd = true;
			}
		}
		else
		if (*s == ']')
		{
			APPEND(d, '\2');
			fUnd = false;
		}

		// COPY
		APPEND(d, *s);
		s++;
	}

	APPEND(d, 0);

	return 0;
}

bool _dictfile2::findFirst(char *word, LPDICTREC2 result)
{
	fpos_t offset = { sizeof(DICTHEADER), 0 };
	int ret = fsetpos(iofile, &offset);
	if (ftell() != offset.__pos)
		return false;

	return findNext(word, result);
}

bool _dictfile2::findFirstKey(char *word, LPDICTREC2 result)
{
	fpos_t offset = { sizeof(DICTHEADER), 0 };
	int ret = fsetpos(iofile, &offset);
	if (ftell() != offset.__pos)
		return false;

	return findNext(word, result, true);
}

bool _dictfile2::findNext(char *word, LPDICTREC2 result, bool byKey)
{
	bool found = false;
	
	do
	{
		if (read(result, header.recordSize) != header.recordSize)
			break;
		if (!byKey)
		{
			if (!strcmp(result->source, word))
				return true;
		}
		else
		{
			for (int i = 0; i < result->numKeys; i++)
			{
				if (!strcmp(result->keys[i], word))
					return true;
			}
		}
	}
	while (!found);

	if (!found)
		memset(result, 0, header.recordSize);

	return found;
}

bool _dictfile2::findLike(char *word, LPDICTSEARCH2 result, bool byKey)
{
	bool found = false;
	DICTREC2 rec;
	
	result->numRecords = 0;

	fpos_t offset = { sizeof(DICTHEADER), 0 };
	int ret = fsetpos(iofile, &offset);
	if (ftell() != offset.__pos)
		return false;

	do
	{
		if (read(&rec, header.recordSize) != header.recordSize)
			break;
		
		if (byKey)
		{
			bool hasKey = false;
			char *sKey = NULL;
			for (int i = 0; i < rec.numKeys; i++)
			{
				if (strstr(rec.keys[i], word))
				{
					hasKey = true;
					sKey = rec.keys[i];
					break;
				}
			}

			if (hasKey)
			{
				if (result->numRecords < MAX_DICT_SEARCH)
				{
					memcpy(result->source[result->numRecords], sKey, MAX_DICT_SRC);
					result->numRecords++;
					found = true;
				}
			}
		}
		else
		{
			if (strstr(rec.source, word))
			{
				if (result->numRecords < MAX_DICT_SEARCH)
				{
					memcpy(result->source[result->numRecords], rec.source, MAX_DICT_SRC);
					result->numRecords++;
					found = true;
				}
			}
		}
	}
	while (result->numRecords < MAX_DICT_SEARCH);

	return found;
}

// DBFS Search engines
//////////////////////////////////////////////////////////////////////
void _sentence::parse(char *s)
{
	char *e = s;
	char *ws = NULL;
	int l = strlen(s);

	while (e-s < l)
	{
		switch (*e)
		{
		case '.': numDots++; break;
		case ',': numCommas++; break;
		case '(': numBrackets++; break;
		case ')': numBrackets++; break;
		}
		if (*e >= '0' && *e <= '0') numNumbers++;

		char *ws = e;
		bool isWord = false;

		while (((*e >= 'A' && *e <= 'Z') || (*e >= 'a' && *e <= 'z') || (*e >= 'À' && *e <= 'ß') || (*e >= 'à' && *e <= 'ÿ') || (*e >= '0' && *e <= '9')) && e-s < l) e++;
		if (e-ws >= 1 && wordCount < _MAX_WORDS)
		{
			words[wordCount].set(ws, e-ws);
			wordCount++;
			isWord = true;
		}

		if (!isWord) e++;
	}
}
