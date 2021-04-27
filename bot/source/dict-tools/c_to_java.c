#include "../defines.h"
#include "../dates.h"

bool dconvert(const char *dictFilePath)
{
	//DATE_INFOex diex;
	DATE_INFO diex;
	DATE_INFO_HEADER dih;

	FILE *dictionary;

	dictionary = fopen(dictFilePath, "rb");

	if (!dictionary) {
		printf("Cannot open: %s\n", dictFilePath);
		return false;
	}

		
	fread(&dih, sizeof (DATE_INFO_HEADER), 1, dictionary);
	//printf("Reading %s (%ld items, %ld last)...\n", dictFilePath, dih.numEntries, dih.lastId);
	printf("{ \"dateInfoFile\": \"%s\", \"numItems\": %ld, \"lastItemId\": %ld,\n", dictFilePath, dih.numEntries, dih.lastId);
	printf("\"items\": [\n");
	
	while (fread(&diex, sizeof (DATE_INFO), 1, dictionary) == 1)
	{
	    char *t = diex.text;
	    int tl = strlen(t);
	    for (int i = 0; i < tl; i++) {
		if (t[i] == '"') {
		    t[i] = '`';
		}
	    }
	    printf("\t{ \"id\": %d, \"day\": %u, \"month\": %u, \"year\": %u, \"text\": \"%s\", \"nick\": \"%s\", \"ts\":%lu },\n",
	    //diex.id, diex.day, diex.month, diex.year, diex.text, diex.nick, diex.created);
	    diex.id, diex.day, diex.month, diex.year, t, diex.nick, diex.created);
	}

	printf("\n]}\n");

	fclose(dictionary);

	return true;
}

/*
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
*/

int main()
{
	dconvert(DATES_FILE_PATH);
	return 0;
}
