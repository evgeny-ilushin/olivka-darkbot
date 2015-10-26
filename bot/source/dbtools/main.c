#include <stdio.h>
#include "dbtools.h"

void dictLatin(char *src, char *buf)
{
    DICTREC rec;
    _dictfile f("/home/olivka/bot/dict/rus2lat.dict2");
    
    if (!f.open())
    {
		printf("cannot open DB\n");
		return;
    }
    
    if (f.findFirst(src, &rec))
    	printf("%s: %s\n", rec.source, rec.destination);
    else
    {
		DICTSEARCH ds;
		if (f.findLike(src, &ds))
		{
			*buf = 0;
			for (int i = 0; i < ds.numRecords; i++)
			{
				strcat(buf, ds.source[i]);
				if (i < ds.numRecords-1)
					strcat(buf, ", ");
			}
			printf("%s: %s\n", src, buf);
		}
		else
    		printf("not found\n");
    }
    
    f.close();
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: dbtools <word>\n");
		return 2;
	}
	
	char t[1024];
	
    dictLatin(argv[1], t);
    
    return 0;
}
