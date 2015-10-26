bool t_netpack(char *channel, char *nick, char *word)
{
	FILE *npDb = fopen(DICT_NETPACK, "rt");

	if (!npDb)
	{
		S("NOTICE %s :database access error\n", channel);
		return true;
	}

	char sline[1024];
	int nCnt = 0;

	while (fgets(sline ,1023, npDb))
	{
		if (*sline != '#')
		{
			char lc[1024];
			memcpy(lc, sline, 1024);
			char *lName = isalpha(*lc)?lc:NULL;
			char *lPort = lName?strchr(lName, ' '):lc;
			if (!lPort) continue;
			*lPort = 0; lPort++;
			while (*lPort == ' ' || *lPort == '\t') lPort++;
			char *lPe = strchr(lPort, '/'); if (!lPe) lPe = strchr(lPort, '\t');
			if (!lPe) continue;
			*lPe = 0; lPe++;
			char *lPt = lPe;
			char *lDesc = strchr(lPt, ' '); if (!lDesc) lDesc = strchr(lPt, '\t');
			if (!lDesc) continue;
			*lDesc = 0;
			lDesc++;
			while (*lDesc == ' ' || *lDesc == '\t') lDesc++;
			if ((lName && strstr(lName, word)) || !strcmp(lPort, word))
			{
				// Got match :)
				if (nCnt < MAX_NETPACK_REPLY)
				{
					int dl = strlen(lDesc);
					while (dl && (lDesc[dl-1] == '\n' || lDesc[dl-1] == '\r')) lDesc[--dl] = 0;
					// PRN
					//printf("%s%s%s/%s %s\n", lName?lName:"", lName?": ":"", lPort, lPt, lDesc);
					if (!nCnt)
					    S("PRIVMSG %s :%s:\n", channel, nick);
					S("PRIVMSG %s :%s%s%s/%s %s\n", channel, lName?lName:"", lName?": ":"", lPort, lPt, lDesc);
				}
				nCnt++;
			}
		}
	}
	if (nCnt > MAX_NETPACK_REPLY)
	{
		// PRN
		//printf("(%d total)\n", nCnt);
	    S("PRIVMSG %s :(%d total)\n", channel, nCnt);
	}
	if (!nCnt)
	{
	    S("PRIVMSG %s :%s, no data\n", channel, nick);
	}

	fclose(npDb);

	return false;
}

