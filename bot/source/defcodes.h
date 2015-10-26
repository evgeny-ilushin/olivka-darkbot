#define DTC2 " \n\r\t"
//#define DTC2 " \n\r\t\'\""

	static int u2_trim(char *pszSource, const char *pszTrimChars)
	{
		size_t sl = strlen(pszSource);
		size_t cl = strlen(pszTrimChars);
		int nCnt = 0;
		for (size_t i = sl-1; i >= 0; i--)
		{
			bool gotIt = false;
			for (size_t j = 0; j < cl; j++)
			{
				if (pszSource[i] == pszTrimChars[j])
				{
					pszSource[i] = 0;
					gotIt = true;
					nCnt++;
					break;
				}
			}
			if (!gotIt)
				break;
		}
		return nCnt;
	}

	// Trim left
	static int u2_trim2(char **pszSource, const char *pszTrimChars)
	{
		size_t sl = strlen(*pszSource);
		size_t cl = strlen(pszTrimChars);
		int nCnt = 0;
		for (size_t i = 0; i < sl-1; i++)
		{
			bool gotIt = false;
			for (size_t j = 0; j < cl; j++)
			{
				if ((*pszSource)[i] == pszTrimChars[j])
				{
					(*pszSource)[i] = 0;
					*pszSource++;
					gotIt = true;
					nCnt++;
					break;
				}
			}
			if (!gotIt)
				break;
		}
		return nCnt + u2_trim(*pszSource, pszTrimChars);
	}

bool get_def_info(char *response, char *buffer, int bufLen)
{
	bool gres = false;
	strcpy(buffer, "Not found");

	do
	{
		char *line = response;
		if (!line) break;
		
		int rLen = strlen(line);
		if (rLen < 100) break;

		char *nLine = strchr(line, '\n');
		if (!nLine) break;

		*nLine = 0;
		nLine++;

		u2_trim2(&line, DTC2);
		if (!strstr(line, "searchCodeResultsTable")) break;
		bool nOp1 = false;
		bool nOp2 = false;
		bool nOp3 = false;

		char o1[1024] = "";
		char o2[1024] = "";
		char o3[1024] = "";

		int ores = 0;

		do
		{
			line = nLine;
			nLine = strchr(line, '\n');
			if (!nLine) break;
			*nLine = 0; nLine++;
			u2_trim2(&line, DTC2);


			// All done
			if (ores >= 3)
			{
				sprintf(buffer, "%s (%s, %s)", o1, o2, o3);
				return true;
			}


			// PARAM 3
			if (nOp3)
			{
				if (!memcmp(line, "<td abbr=\"", 10))
				{
					char *p0 = line+10;
					char *p1 = strstr(p0, "\" align=\"");
					if (p1)
					{
						*p1 = 0; p1++;
						u2_trim2(&p0, DTC2);
						strcpy(o3, p0);
						ores++;
						nOp1 = false;
						nOp2 = false;
						nOp3 = false;
					}
				}
			}

			// PARAM 1
			if (nOp1)
			{
				if (!memcmp(line, "<td abbr=\"", 10))
				{
					char *p1 = strstr(line+10, "\">");
					if (p1)
					{
						p1 += 2;
						char *p2 = strstr(p1, "</td>");
						if (p2)
						{
							*p2 = 0; p2++;
							u2_trim2(&p1, DTC2);
							strcpy(o1, p1);
							ores++;
							nOp1 = false;
							nOp2 = true;
						}
					}
				}

				if (!nOp2 || nOp1) break;
			}

			// PARAM 2
			if (nOp2)
			{
				if (!memcmp(line, "<td abbr=\"", 10))
				{
					char *p1 = strstr(line+10, "\">");
					if (p1)
					{
						p1 += 2;
						char *p2 = strstr(p1, "</td>");
						if (p2)
						{
							*p2 = 0; p2++;
							u2_trim2(&p1, DTC2);
							strcpy(o2, p1);
							ores++;
							nOp1 = false;
							nOp2 = false;
							nOp3 = true;

							//
							line = nLine;
							nLine = strchr(line, '\n');
							if (!nLine) break;
							*nLine = 0; nLine++;
							u2_trim2(&line, DTC2);
							//
						}
					}
				}
			}

			if (!strcmp(line, "<tr class=\"last\">"))
			{
				//printf("123");
				nOp1 = true;
				continue;
			}
		}
		while (nLine);

		return ores;
	}
	while (false);

	return false;
}