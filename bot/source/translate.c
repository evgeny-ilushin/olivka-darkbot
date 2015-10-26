#include <stdio.h>
#include <time.h>
#include <string.h>
#include "stdafx.h"

#ifndef __cplusplus
typedef enum
{ false, true }
bool;
#endif

#include "translate.h"

void S(const char *format, ...);

bool _translate(const char *word, char *buffer, int size, int direction)
{
	const char *dictName;
	const char *altDictName;

	if (direction == DIR_AUTO)
		direction = getDirection(word);

	if (direction == DIR_UNKNOWN)
		return false;

	dictName = direction == DIR_RUSSIAN_ENGLISH? DICT_RUSSIAN_ENGLISH : DICT_ENGLISH_RUSSIAN;
	altDictName = direction == DIR_RUSSIAN_ENGLISH? CDICT_RUSSIAN_ENGLISH : CDICT_ENGLISH_RUSSIAN;

	return _findWord(word, buffer, size, direction, dictName, altDictName);
}

bool translate2(const char *word, DICT2_ENTRY *buffer, int direction, bool full)
{
	const char *dictName;
	const char *altDictName;

	if (direction == DIR_AUTO)
		direction = getDirection(word);

	if (direction == DIR_UNKNOWN)
		return false;

	dictName = direction == DIR_RUSSIAN_ENGLISH? DICT_RUSSIAN_ENGLISH : DICT_ENGLISH_RUSSIAN;
	altDictName = direction == DIR_RUSSIAN_ENGLISH? CDICT_RUSSIAN_ENGLISH : CDICT_ENGLISH_RUSSIAN;

	return findWord2(word, buffer, direction, dictName, altDictName, full);
}

int getDirection(const char *word)
{
	int nChars = strlen(word);
	int index, csi;
	
	char *ruCharSet = "¿¡¬√ƒ≈®∆«»… ÀÃÕŒœ–—“”‘’÷◊ÿŸ‹€⁄›ﬁﬂ‡·‚„‰Â∏ÊÁËÈÍÎÏÌÓÔÒÚÛÙıˆ˜¯˘¸˚˙˝˛ˇ";
	char *enCharSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	int nRuChars = 0;
	int nEnChars = 0;

	bool isKnownChar;

	for (index = 0; index < nChars; index++)
	{
		isKnownChar = false;

		for (csi = 0; csi < 66; csi++)
		{
			if (word[index] == ruCharSet[csi])
			{
				nRuChars++;
				isKnownChar = true;
			}
		}
		for (csi = 0; csi < 52; csi++)
		{
			if (word[index] == enCharSet[csi])
			{
				nEnChars++;
				isKnownChar = true;
			}
		}

		if (!isKnownChar)
		{
			nRuChars++;
			nEnChars++;
		}
	}

	return nRuChars == nChars? DIR_RUSSIAN_ENGLISH : (nEnChars == nChars? DIR_ENGLISH_RUSSIAN : DIR_UNKNOWN);
}

bool _findWord(const char *word, char *buffer, int size, int direction, const char *dictFilePath, const char *altDictFilePath)
{
	if (findWordEx(word, buffer, size, direction, dictFilePath))
		return true;
	else
		return findWordEx(word, buffer, size, direction, altDictFilePath);
}

bool findWord2(const char *word, DICT2_ENTRY *buffer, int direction, const char *dictFilePath, const char *altDictFilePath, bool full)
{
	if (full)
	{
		if (findWordEx2(word, buffer, direction, altDictFilePath))
			return true;
		else
		{
			buffer->time = -2;
			return findWordEx(word, buffer->target, DICT_MAX_TARGET, direction, dictFilePath);
		}
	}
	else
	{
		if (findWordEx(word, buffer->target, DICT_MAX_TARGET, direction, dictFilePath))
		{
			buffer->time = -2;
			return true;
		}
		else
			return findWordEx2(word, buffer, direction, altDictFilePath);
	}
}

bool findWordEx(const char *word, char *buffer, int size, int direction, const char *dictFilePath)
{
	DICT_ENTRY de;
	FILE *dictionary;
	char wordLower[1024];
	bool result = false;

	strcpy(wordLower, word);
	charLower(wordLower);

	dictionary = fopen(dictFilePath, "rb");

	if (!dictionary)
		return false;

	while (fread(&de, sizeof (DICT_ENTRY), 1, dictionary) == 1)
	{
		if (!strcmp(wordLower, de.source))
		{
			result = true;
			strncpy(buffer, de.target, size);
			break;
		}
	}

	fclose(dictionary);

	return result;
}

bool findWordEx2(const char *word, DICT2_ENTRY *buffer, int direction, const char *dictFilePath)
{
	DICT2_ENTRY de;
	FILE *dictionary;
	char wordLower[1024];
	bool result = false;

	strcpy(wordLower, word);
	charLower(wordLower);

	dictionary = fopen(dictFilePath, "rb");

	if (!dictionary)
		return false;

	while (fread(&de, sizeof (DICT2_ENTRY), 1, dictionary) == 1)
	{
		if (!strcmp(wordLower, de.source))
		{
			result = true;
			memcpy(buffer, &de, sizeof (DICT2_ENTRY));
			break;
		}
	}

	fclose(dictionary);

	return result;
}

bool _convert(const char *dictFilePath)
{
	DICT_ENTRY de;

	FILE *dictionary;
	FILE *dictionaryW;

	char dictFilePathW[1024];
	sprintf(dictFilePathW, "%s.v2", dictFilePath);

	dictionary = fopen(dictFilePath, "rb");
	dictionaryW = fopen(dictFilePathW, "wb");

	if (!dictionary || !dictionaryW)
		return false;

	while (fread(&de, sizeof (DICT_ENTRY), 1, dictionary) == 1)
	{
		charLower(de.source);
		fwrite(&de, sizeof (DICT_ENTRY), 1, dictionaryW);
	}

	fclose(dictionary);
	fclose(dictionaryW);

	return true;
}

int charLower(char *word)
{
	char *upperCharSet = "¿¡¬√ƒ≈®∆«»… ÀÃÕŒœ–—“”‘’÷◊ÿŸ‹€⁄›ﬁﬂABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *lowerCharSet = "‡·‚„‰Â∏ÊÁËÈÍÎÏÌÓÔÒÚÛÙıˆ˜¯˘¸˚˙˝˛ˇabcdefghijklmnopqrstuvwxyz";
	int wordLength = strlen(word);
	int index;
	int csi;

	for (index = 0; index < wordLength; index++)
	{
		for (csi = 0; csi < 59; csi++)
			if (word[index] == upperCharSet[csi])
				word[index] = lowerCharSet[csi];
	}

	return wordLength;
}

bool _t_translate(char *channel, char *nick, char *word)
{
	char result[1024];
	
	bool ts = _translate(word, result, 1024, DIR_AUTO);
	if (ts)
	{
		S("PRIVMSG %s :%s, \2%s\2: %s\n", channel, nick, word, result);
		return true;
	}
	else
		S("PRIVMSG %s :%s, I don't know :(\n", channel, nick);
		
	return false;
}

bool t_translate2(char *channel, char *nick, char *word, bool full)
{
	DICT2_ENTRY result;
	
	bool ts = translate2(word, &result, DIR_AUTO, full);
	if (ts)
	{
		if (full)
		{
			if (result.time == -2)
			{
				S("PRIVMSG %s :%s, \2%s\2: %s\n", channel, nick, word, result.target);
			}
			else
			{
				S("PRIVMSG %s :%s, \2%s\2: %s (%s, %s)\n", channel, nick, word, result.target, result.nick, nocrlf(ctime(&result.time)));
				if (strcmp(result.target_o, "n/a") || strcmp(result.nick_o, "n/a"))
					S("PRIVMSG %s :   was: %s (%s, %s)\n", channel, result.target_o, result.nick_o, nocrlf(ctime(&result.time_o)));
			}
		}
		else
			S("PRIVMSG %s :%s, \2%s\2: %s\n", channel, nick, word, result.target);
		return true;
	}
	else
		S("PRIVMSG %s :%s, I don't know :(\n", channel, nick);
		
	return false;
}


int _addWord(const char *word, const char *translation)
{
	DICT_ENTRY de;
	int direction;
	FILE *dictionary;
	bool entryExist = false;
	const char *altDictName;
	long exOffset;

	char s_word[31];
	char s_translation[53];

	if (strlen(word) >= 31)
		return TM_WORD_SIZE;

	if (strlen(translation) >= 31)
		return TM_TRANSLATION_SIZE;

	direction = getDirection(word);

	if (direction == DIR_UNKNOWN)
		return TM_INVALID_DIRECTION;

	altDictName = direction == DIR_RUSSIAN_ENGLISH? CDICT_RUSSIAN_ENGLISH : CDICT_ENGLISH_RUSSIAN;

	memset(s_word, 0, 31);
	memset(s_translation, 0, 53);

	strncpy(s_word, word, 31);
	strncpy(s_translation, translation, 53);

	s_word[30] = 0;
	s_translation[52] = 0;

	charLower(s_word);

	dictionary = fopen(altDictName, "a+b");

	if (!dictionary)
		return TM_ACCESS_DENIED;

	rewind(dictionary);

	while (fread(&de, sizeof(DICT_ENTRY), 1, dictionary) == 1)
	{
		if (!strcmp(s_word, de.source))
		{
			memcpy(de.target, s_translation, 53);
			exOffset = ftell(dictionary)-sizeof(DICT_ENTRY);
			dictionary = freopen(altDictName, "r+b", dictionary);

			if (!dictionary)
				return TM_ACCESS_DENIED;

			fseek(dictionary, exOffset, SEEK_SET);
			fwrite(&de, sizeof(DICT_ENTRY), 1, dictionary);
			fclose(dictionary);

			return TM_UPDATED;
		}
	}

	fseek(dictionary, 0, SEEK_END);

	memcpy(de.source, s_word, 31);
	memcpy(de.target, s_translation, 53);
	fwrite(&de, sizeof (DICT_ENTRY), 1, dictionary);

	fclose(dictionary);

	return TM_ADDED;
}

int addWord2(const char *word, const char *translation, const char *nick)
{
	DICT2_ENTRY de, deAdd;
	int direction;
	FILE *dictionary;
	bool entryExist = false;
	const char *altDictName;
	long exOffset;

	if (strlen(word) >= DICT_MAX_SOURCE)
		return TM_WORD_SIZE;

	if (strlen(translation) >= DICT_MAX_TARGET)
		return TM_TRANSLATION_SIZE;

	direction = getDirection(word);

	if (direction == DIR_UNKNOWN)
		return TM_INVALID_DIRECTION;

	altDictName = direction == DIR_RUSSIAN_ENGLISH? CDICT_RUSSIAN_ENGLISH : CDICT_ENGLISH_RUSSIAN;

	memset(&deAdd, 0, sizeof(DICT2_ENTRY));

	strcpy(deAdd.source, word);
	strcpy(deAdd.target, translation);
	strcpy(deAdd.target_o, "n/a");
	strncpy(deAdd.nick, nick, 15);
	strcpy(deAdd.nick_o, "n/a");
	deAdd.time = time(0);
	deAdd.time_o = (time_t)(-1);

	charLower(deAdd.source);

	dictionary = fopen(altDictName, "a+b");

	if (!dictionary)
		return TM_ACCESS_DENIED;

	rewind(dictionary);

	while (fread(&de, sizeof(DICT2_ENTRY), 1, dictionary) == 1)
	{
		if (!strcmp(deAdd.source, de.source))
		{
			memcpy(de.target_o, de.target, DICT_MAX_TARGET);
			memcpy(de.nick_o, de.nick, 16);
			de.time_o = de.time;
			de.time = time(0);
			memcpy(de.target, deAdd.target, DICT_MAX_TARGET);
			memcpy(de.nick, deAdd.nick, 16);
			exOffset = ftell(dictionary)-sizeof(DICT2_ENTRY);
			dictionary = freopen(altDictName, "r+b", dictionary);

			if (!dictionary)
				return TM_ACCESS_DENIED;

			fseek(dictionary, exOffset, SEEK_SET);
			fwrite(&de, sizeof(DICT2_ENTRY), 1, dictionary);
			fclose(dictionary);

			return TM_UPDATED;
		}
	}

	fseek(dictionary, 0, SEEK_END);

	fwrite(&deAdd, sizeof (DICT2_ENTRY), 1, dictionary);

	fclose(dictionary);

	return TM_ADDED;
}

void _t_addWord(char *channel, char *nick, const char *word, const char *translation)
{
	int result = _addWord(word, translation);

	if (translation == NULL)
	{
		S("PRIVMSG %s :%s, you're lame! Command usage is: addword <word> <translation>\n", channel, nick);
		return;
	}

	switch (result)
	{
	case TM_WORD_SIZE:
		S("PRIVMSG %s :%s, word is too long.\n", channel, nick);
		break;
	case TM_TRANSLATION_SIZE:
		S("PRIVMSG %s :%s, you're \2flooder\2, translation is too long :)\n", channel, nick);
		break;
	case TM_INVALID_DIRECTION:
		S("PRIVMSG %s :%s, cannot add mixed language entries.\n", channel, nick);
		break;
	case TM_ACCESS_DENIED:
		S("PRIVMSG %s :%s, access denied.\n", channel, nick);
		break;
	case TM_UPDATED:
		S("PRIVMSG %s :%s, done. Translation of \2%s\2 has been changed.\n", channel, nick, word);
		break;
	case TM_ADDED:
		S("PRIVMSG %s :OK, %s :)\n", channel, nick);
		break;
	default:
		S("PRIVMSG %s :%s, sorry, I cannot :(\n", channel, nick);
		break;
	}
}

void t_addWord2(char *channel, char *nick, const char *word, const char *translation)
{
	int result = addWord2(word, translation, nick);

	if (translation == NULL)
	{
		S("PRIVMSG %s :%s, you're lame! Command usage is: addword <word> <translation>\n", channel, nick);
		return;
	}

	switch (result)
	{
	case TM_WORD_SIZE:
		S("PRIVMSG %s :%s, word is too long (< 128).\n", channel, nick);
		break;
	case TM_TRANSLATION_SIZE:
		S("PRIVMSG %s :%s, you're \2flooder\2, translation is too long (> 512) :)\n", channel, nick);
		break;
	case TM_INVALID_DIRECTION:
		S("PRIVMSG %s :%s, cannot add mixed locale entries.\n", channel, nick);
		break;
	case TM_ACCESS_DENIED:
		S("PRIVMSG %s :%s, access denied.\n", channel, nick);
		break;
	case TM_UPDATED:
		S("PRIVMSG %s :%s, updated \2%s\2.\n", channel, nick, word);
		break;
	case TM_ADDED:
		S("PRIVMSG %s :OK, %s :)\n", channel, nick);
		break;
	default:
		S("PRIVMSG %s :%s, sorry, I cannot :(\n", channel, nick);
		break;
	}
}
