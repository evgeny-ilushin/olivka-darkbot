//#!/bin/csh

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>


int check_rdb(const char *info2, const char *dir, char *file)
{
    char path[1024], path2[1024], line[4096];
    char *ne = strstr(file, ".rdb");
    if (!ne)
	return 0;
    printf("Checking [%s]...\n", file);
    sprintf(path, "%s/%s", dir, info2);    
    FILE *i2 = fopen(path, "rt");
    if (!i2)
    {
	printf("Cannot open [%s/%s]\n", dir, info2);
	return 0;
    }
    sprintf(path, "~%s", file);    
    sprintf(path2, "^%s", file);    
    path[ne-file+1] = 0;
    path2[ne-file+1] = 0;
    bool hase = false;
    int lcnt = 0;
    while (fgets(line, 4095, i2))
    {
	lcnt++;
	char *e = strstr(line, path);
	if (!e)
	    e = strstr(line, path2);
	if (e)
	{
	    hase = true;
	    printf(" * line [%d]: %s", lcnt, line);
	}
    }
    if (!hase)
	printf("[%s] cannot be found in [%s]\n", path, info2);
	
    fclose(i2);
    
    return hase?1:0;
}

int main(char *, char **)
{
    printf("PREVED! I will check your info2.db for missing rdb files :)\n");

    const char dir[] = "/home/olivka/bot/dat";
    const char info2[] = "info2.db";
    
    DIR *d = opendir(dir);
    if (!d)
    {
	printf("Cannot open [%s]\n", dir);
	return 1;
    }
    struct dirent *de = (dirent *)1;
    while (de)
    {
        de = readdir(d);
	if (!de) break;
	if (de->d_type == 8 && strstr(de->d_name, ".rdb"))
	    check_rdb(info2, dir, de->d_name);
	// printf("%s: %d\n", de->d_name, de->d_type);
	
    }
    closedir(d);
    
    
    return 0;
}
