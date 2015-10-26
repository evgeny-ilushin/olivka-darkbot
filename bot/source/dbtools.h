//////////////////////////////////////////////////////////////////////
// dbtools.h: DBFS interface for olivka
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBTOOLS_H__BE818ED9_F8E2_46D4_B2D8_53852E5A8A1E__INCLUDED_)
#define AFX_DBTOOLS_H__BE818ED9_F8E2_46D4_B2D8_53852E5A8A1E__INCLUDED_

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

// DBFS Storage structure
//////////////////////////////////////////////////////////////////////

#pragma pack(1)

typedef struct tagDICTREC
{
	char source[MAX_DICT_SRC];
	char destination[MAX_DICT_DST];

} DICTREC, *LPDICTREC;

typedef struct tagDICTREC2
{
	char keys[MAX_DICT_KEYS][MAX_DICT_KEY2];
	int numKeys;
	char source[MAX_DICT_SRC2];
	char destination[MAX_DICT_DST2];

} DICTREC2, *LPDICTREC2;

typedef struct tagDICTHEADER
{
	char logo[32];
	unsigned long numRecords;
	long created;
	unsigned long recordSize;

} DICTHEADER, *LPDICTHEADER;

typedef struct tagDICTSEARCH
{
	char source[MAX_DICT_SEARCH][MAX_DICT_SRC];
	int numRecords;

} DICTSEARCH, *LPDICTSEARCH;

typedef struct tagDICTSEARCH2
{
	char source[MAX_DICT_SEARCH][MAX_DICT_SRC2];
	int numRecords;

} DICTSEARCH2, *LPDICTSEARCH2;

typedef struct tagWTFREC
{
	long created;
	long modified;
	char nick[16];
	char source[64];
	char destination[512];

} WTFREC, *LPWTFREC;

#pragma pack()

// DBFS Search engines
//////////////////////////////////////////////////////////////////////
#define _MAX_WORD	256
#define _MAX_WORDS	128

// Basic I/O file
class _file
{
public:
	_file() { iofile = NULL; *path = 0; }
	_file(char *s) { iofile = NULL; strncpy(path, s, 2048); }
	~_file() { close(); }

public:
	virtual bool open() { return _open(path, "rb"); }
	bool open(char *m) { return _open(path, m); }
	bool open(char *s, char *m) { strncpy(path, s, 2048); return _open(path, m); }
	void close() { if (iofile) { fclose(iofile); iofile = NULL; } }	
	FILE *getHandle() { return iofile; }
	char *getPath() { return path; }
	size_t read(void *d, int c) { return ::fread(d, sizeof(char), c, iofile); }
	size_t write(const void *d, int c) { return ::fwrite(d, sizeof(char), c, iofile); }
	int seek(int origin, long offset = 0) { return ::fseek(iofile, origin, offset); }
	void rewind() { ::rewind(iofile); }
	long ftell() { return ::ftell(iofile); }

	operator char *() { return path; }
	operator FILE *() { return iofile; }

protected:
	bool _open(char *s, char *m) { return (iofile = fopen(path, m)) != NULL; }

protected:
	char path[2048];
	FILE *iofile;
};

class _rdbfile : public _file
{
public:
	_rdbfile() : _file() { memset(&header, 0, sizeof(DICTHEADER)); }
	_rdbfile(char *s) : _file(s) { memset(&header, 0, sizeof(DICTHEADER)); }

public:
	bool create(int recSize);
	bool open();
	bool openRW();
	bool save() { return writeHeader(); }
	bool readAt(unsigned i, void *r);

	bool add(char *, char *);

	DICTHEADER header;
	static char logo[];

protected:
	bool readHeader();
	bool writeHeader();
};

class _hashfile : public _rdbfile
{
public:
	_hashfile(char *s) : _rdbfile(s) {}
	bool create() { return _rdbfile::create(sizeof(DICTREC)); }
public:
	bool setKey(char *, char *, bool);
	bool find(char *, LPDICTREC, bool matchCase = false);
	//bool findLike(char *, LPDICTSEARCH, bool);
	bool add(char *, char *);
	bool del(char *);
};

class _dictfile : public _rdbfile
{
public:
	_dictfile(char *s) : _rdbfile(s) {}
public:
	bool findFirst(char *, LPDICTREC);
	bool findNext(char *, LPDICTREC);
	bool findLike(char *, LPDICTSEARCH);

	static int parseDictLine(char *src, char *dst);
};

class _dictfile2 : public _rdbfile
{
public:
	_dictfile2(char *s) : _rdbfile(s) {}
public:
	bool findFirst(char *, LPDICTREC2);
	bool findFirstKey(char *, LPDICTREC2);
	bool findNext(char *, LPDICTREC2, bool byKey = false);
	bool findLike(char *, LPDICTSEARCH2, bool byKey = false);
};

// Simple OneWord container
class _word
{
public:
	_word() { clear(); }
	_word(char *s) { set(s); }
	_word(char *s, int n) { set(s, n); }

public:
	void clear() { memset(letters, 0, _MAX_WORD); _length = 0; }
	char *set(char *s) { strncpy(letters, s, _MAX_WORD-1); letters[_MAX_WORD-1] = 0; _length = strlen(letters); return letters; }
	char *set(char *s, int n) { memcpy(letters, s, n); letters[_MAX_WORD-1] = 0; _length = n; return letters; }
	char *get() { return letters; }
	int length() { return _length; }

//	operator char *() { return letters; }
//	operator int() { return _length; }
//	operator =(char *s) { set(s); return _length; }

private:
	int _length;
	char letters[_MAX_WORD];
};

// Lexic parser: sentence
class _sentence
{
public:
	_sentence() { clear(); }
	_sentence(char *s) { clear(); parse(s); }

public:
	void clear() { for (int i = 0; i < wordCount; i++) words[i].clear(); wordCount = 0; numDots = 0; numCommas = 0; numNumbers = 0; numBrackets = 0; }
	void parse(char *s);

public:
	int wordCount;
	int numDots, numCommas, numNumbers, numBrackets;

private:
	_word words[_MAX_WORDS];

};

#endif // !defined(AFX_DBTOOLS_H__BE818ED9_F8E2_46D4_B2D8_53852E5A8A1E__INCLUDED_)
