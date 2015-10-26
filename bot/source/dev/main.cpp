// mail.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string.h>
//#include "../dbtools.h"
#include "../seen.cpp"

int main(int argc, char* argv[])
{
	char res[100][256];
	int nres = 100;
	
	char *nn = "koms";
	int n = seenByNick(nn, res, &nres, true);
	printf("Seen ret: %d\n", n);
	if (n > 0)
	{
		for (int i = 0; i < nres; i++)
		printf("N%d: %s = %s\n", i+1, nn, res[i]);
	}

	nres = 100;
	char *nn1 = "212.12.25.87";
	char *nn2 = "technik@212.12.25.87";
	int n1 = seenByAddr(nn1, res, &nres, true);
	printf("Seen1 ret: %d\n", n1);
	if (n1 > 0)
	{
		for (int i = 0; i < nres; i++)
		printf("N%d: %s = %s\n", i+1, nn1, res[i]);
	}

	char r1[1024];
	if (nicksByAddrEx(nn1, r1))
	printf("X1: %s = %s\n", nn1, r1);

	char r2[1024];
	if (nicksByAddrEx(nn2, r2))
	printf("X2: %s = %s\n", nn2, r2);

	/*
	_hashfile f("hashfile.dx");
	
	int res = f.openRW() == true;
	
	if (!res)
	    res = f.create() == true;	

	printf("openRW = %d\n", res);
	printf("NUM = %d\n", f.header.numRecords);
	
	res = f.add("test2", "TEST222!!!") == true;
	printf("ADD = %d\n", res);
	
	res = f.add("test3", "TEST333!!!") == true;
	printf("ADD = %d\n", res);

	res = f.del("test") == true;
	printf("DEL = %d\n", res);
	
	res = f.del("test2") == true;
	printf("DEL = %d\n", res);
	
	f.close();
	printf("close\n");
	
	*/
	return 0;
}


//#include "../dbtools.cpp"
//#include "../seen.cpp"
