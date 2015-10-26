#include "../defines.h"
#include "../translate.h"

bool dconvert(const char *dictFilePath)
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
		DICT2_ENTRY de2;
		memset(&de2, 0, sizeof(DICT2_ENTRY));
//		charLower(de.source);
		strcpy(de2.source, de.source);
		strcpy(de2.target, de.target);
		strcpy(de2.nick, "n/a");
		strcpy(de2.target_o, "n/a");
		strcpy(de2.nick_o, "n/a");
		de2.time = time(&de2.time);
		de2.time_o = (time_t)(-1);
		fwrite(&de2, sizeof (DICT2_ENTRY), 1, dictionaryW);
	}

	fclose(dictionary);
	fclose(dictionaryW);

	return true;
}

bool dump(const char *dictFilePath)
{
	DICT2_ENTRY de;
	FILE *dictionary;
	int cnt = 0;

	dictionary = fopen(dictFilePath, "rb");

	if (!dictionary)
	{
		printf("No items");
		return false;
	}

	while (fread(&de, sizeof (DICT2_ENTRY), 1, dictionary) == 1)
	{
		cnt++;
		// <tr><td>1</td><td>2</td><td>3</td><td>4</td><td>5</td><td>6</td></tr>
		//printf("%s %s\n", de.source, de.target);
		printf("<tr><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", cnt, de.source, de.target, de.nick, de.target_o, de.nick_o);
	}

	fclose(dictionary);

	return true;
}

int main()
{
//	dconvert(CDICT_RUSSIAN_ENGLISH);
//	dconvert(CDICT_ENGLISH_RUSSIAN);

	dump(CDICT_RUSSIAN_ENGLISH);
//	dump(CDICT_ENGLISH_RUSSIAN);
	
	return 0;
}
