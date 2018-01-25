#define DTC " \n\r\t\'\""

typedef struct tagURL
{
	char href[1024];
	char *nick;
	char *chan;
	time_t date;
	tagURL() { reset(); }
	void reset() { date = time(0); href[0] = 0; nick = 0; chan = 0; }

} URL, *LPURL;

class url_cache
{
private:
	static int u_trim(char *pszSource, const char *pszTrimChars)
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
	static int u_trim2(char **pszSource, const char *pszTrimChars)
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
		return nCnt;
	}

private:
	char *m_fn;
	char *str(char *s) { return s? s: (char *)"*"; }
	char *str_ssi(char *s) { return s? s: (char *)"&nbsp;"; }

public:

	url_cache(char *fn) { m_fn = fn; }

	bool add(LPURL fu)
	{
		FILE *f = fopen(m_fn, "a+t");
		do
		{
			if (!f) break;
			char line[10240];
			sprintf(line, "%d %s %s %s\n", fu->date, str(fu->nick), str(fu->chan), fu->href);
			fputs(line, f);
			fclose(f);
		}
		while (false);
		return false;
	}

	bool find(char *url, LPURL fu)
	{
		FILE *f = fopen(m_fn, "rt");
		do
		{
			if (!url || !fu) break;
			if (!f) break;
			char line[10240];
			char url2[10240];
			int tl = strlen(url);
			strncpy(url2, url, 10239);
			for (int i = 0; i < tl; i++) url2[i] = tolower(url2[i]);
			while (fgets(line, 10239, f))
			{
				char *pDate = line;
				char *pNick = strchr(line, ' ');
				if (!pNick) continue;
				*pNick = 0; pNick++;
				u_trim2(&pNick, DTC);
				char *pChan = strchr(pNick, ' ');
				if (!pChan) continue;
				*pChan = 0; pChan++;
				u_trim2(&pChan, DTC);
				char *pUrl = strchr(pChan, ' ');
				if (!pUrl) continue;
				*pUrl = 0; pUrl++;
				//u_trim2(&pUrl, DTC);
				u_trim(pUrl, DTC);
#ifdef WIN32
				if (!strcmpi(url2, pUrl))
#else
				if (!strcasecmp(url2, pUrl))
#endif
				{
					//printf("MATCH1: [%s]\n", url2);
					//printf("MATCH2: [%s]\n", pUrl);

					fu->nick = pNick;
					fu->chan = pChan;
					fu->date = (time_t)atol(pDate);
					strncpy(fu->href, pUrl, 1023);
					fclose(f);
					return true;
				}
				else
				{
					//printf("NOMATCH1: [%s]\n", url2);
					//printf("NOMATCH2: [%s]\n", pUrl);
				}
			}
			fclose(f);
		}
		while (false);
		return false;
	}

	bool getLast(LPURL fu)
	{
		FILE *f = fopen(m_fn, "rt");
		do
		{
			if (!fu) break;
			if (!f) break;
			char line[10240];
			int cnt = 0;
			while (fgets(line, 10239, f))
			{
				cnt++;
			}
			if (cnt)
			{
				char *pDate = line;
				char *pNick = strchr(line, ' ');
				if (!pNick) break;
				*pNick = 0; pNick++;
				u_trim2(&pNick, DTC);
				char *pChan = strchr(pNick, ' ');
				if (!pChan) break;
				*pChan = 0; pChan++;
				u_trim2(&pChan, DTC);
				char *pUrl = strchr(pChan, ' ');
				if (!pUrl) break;
				*pUrl = 0; pUrl++;
				//u_trim2(&pUrl, DTC);
				u_trim(pUrl, DTC);
				fu->nick = pNick;
				fu->chan = pChan;
				fu->date = (time_t)atol(pDate);
				strncpy(fu->href, pUrl, 1023);
				fclose(f);
				return true;
			}
			fclose(f);
		}
		while (false);
		return false;
	}

	bool findUrl(char *text, LPURL url)
	{
		do
		{
			if (!text || !url) break;
			url->reset();
			char text2[10240];
			int tl = strlen(text);
			strncpy(text2, text, 10239);
			for (int i = 0; i < tl; i++) text2[i] = tolower(text2[i]);
			char *p = strstr(text2, "http://");
			if (!p) p = strstr(text2, "ftp://");
			if (!p) p = strstr(text2, "https://");
			if (!p) break;
			int ns = p-text2;
			p = strstr(p, "://") + 3;
			int nc = 0;
			bool isUrl = false;
			while (*p && ((*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') || *p == ',' || *p == '.' || *p == '(' || *p == ')' || *p == '&' || *p == '?' || *p == '-'|| *p == '/'|| *p == '\\' || *p == '%' || *p == '~' || *p == '{' || *p == '}' || *p == '[' || *p == ']' || *p == '_' || *p == '+' || *p == ';' || *p == '='))
			{
				if (*p == '.' && nc > 1) isUrl = true;
				nc++;
				p++;
			}
			if (nc <= 2) break;

			int ndx = p-text2-ns;
			memcpy(url->href, text+ns, ndx);
			url->href[ndx] = 0;
			return true;
		}
		while (false);
		return false;
	}
	
	void unescape(char *src, char *dst)
	{
		int l = strlen(src);
		char c;
		int cnt = 0;
		
		while ((c = *(src++)))
		{
			switch (c)
			{
				case '%':
					{
					char hv[3];
					hv[0] = *(src);
					hv[1] = *(src+1);
					hv[2] = 0;
					int n = 0;
					sscanf(hv, "%02X", &n);
					src += 2;
					dst[cnt++] = (char)n;
					}
					break;
				
				default:
					dst[cnt++] = c;
			}
		}
		dst[cnt++] = 0;
	};

	bool dump_ssi(char *s_mode_esc)
	{
		FILE *f = fopen(m_fn, "rt");
		char ct[128] = "(_O_)";
		char s_mode[1024] = "";
		
		char snicks[512][32];
		char schans[512][32];

		int usnicks[512];
		int uschans[512];
		
		int snn = 0;
		int snc = 0;
		
		// Dump modes
		bool filter = false;
		bool fltStat = false;
		char *fltChan = 0;
		char *fltNick = 0;
		char fltsDate[128];
		time_t fltDate = 0;
		
		
		if (s_mode_esc != 0 && s_mode_esc[0] && (s_mode_esc[1] == '=' || !strcmp(s_mode_esc, "stats")))
		{
			unescape(s_mode_esc, s_mode);
			
			//printf("E:{%s} U:{%s}", s_mode_esc, s_mode);
			
			switch (*s_mode)
			{
			case 'd': fltDate = (time_t)atol(s_mode+2); filter = true; break;
			case 'c': fltChan = s_mode+2; filter = true; break;
			case 'n': fltNick = s_mode+2; filter = true; break;			
			}
			if (!strcmp(s_mode_esc, "stats"))
				fltStat = true;
		}
		


		
		// Do dump
		do
		{
			if (!f) break;
			char line[10240];
			int cnt = 0;

			for (int i = 0; i < 512; i++)
			{
				usnicks[i] = 0;
				uschans[i] = 0;
			}
			


		// Filtered mode
		if (filter && !fltStat)
		{
				if (fltStat)
				{
				}
				else
				{
				if (fltDate > 0)
				{
					struct tm* timeinfo1;
					timeinfo1 = localtime(&fltDate);
					strftime(fltsDate, 128, "%x %X", timeinfo1);

					char ctime3[128];
					strftime(ctime3, 128, "%A, %B %d, %Y", timeinfo1);
					printf("<tr><td colspan=4 align=right><small><b>%s&nbsp;</b></small></td></tr>\n", ctime3);
				}
				
				if (fltNick)
				{
					printf("<tr><td colspan=4 align=right><small><b>%s&nbsp;</b></small></td></tr>\n", fltNick);
				}
				
				if (fltChan)
				{
					printf("<tr><td colspan=4 align=right><small><b>%s&nbsp;</b></small></td></tr>\n", fltChan);
				}
				}
				
			// Do dump
			cnt = 0;
			rewind(f);
			while (fgets(line, 10239, f))
			{
				char *pDate = line;
				char *pNick = strchr(line, ' ');
				if (!pNick) continue;
				*pNick = 0; pNick++;
				u_trim2(&pNick, DTC);
				char *pChan = strchr(pNick, ' ');
				if (!pChan) continue;
				*pChan = 0; pChan++;
				u_trim2(&pChan, DTC);
				char *pUrl = strchr(pChan, ' ');
				if (!pUrl) continue;
				*pUrl = 0; pUrl++;
				//u_trim2(&pUrl, DTC);
				u_trim(pUrl, DTC);
				time_t date = atol(pDate);
				struct tm* timeinfo;
				char ctime1[128];
				char ctime2[128];
				timeinfo = localtime(&date);
				strftime(ctime1, 128, "%x %X", timeinfo);
				memcpy(ctime2, ctime1, 8);
				ctime2[8] = 0;
				
				// Date mode
				if (fltDate > 0)
				{
					if (!memcmp(fltsDate, ctime2, 8))
						printf("<tr><td><small>&nbsp;&nbsp;%d&nbsp;&nbsp;</small></td><td><small>&nbsp;%s&nbsp;</small></td><td><small>&nbsp;<b>%s</b> at %s</small></td><td>&nbsp;<a href=\"%s\">%s</a></td></tr>\n", ++cnt, ctime1, str_ssi(pNick), str_ssi(pChan), pUrl, pUrl);
				}
				else
				// Nick mode
				if (fltNick && !strcmp(fltNick, pNick))
				{
					printf("<tr><td><small>&nbsp;%d&nbsp;</small></td><td><small>&nbsp;%s&nbsp;</small></td><td><small>&nbsp;%s&nbsp;</small></td><td>&nbsp;<a href=\"%s\">%s</a>&nbsp;</td></tr>\n", ++cnt, ctime1, str_ssi(pChan), pUrl, pUrl);
				}
				else
				// Chan mode
				if (fltChan && !strcmp(fltChan, pChan+1))
				{
					//printf("<tr><td><small>&nbsp;%d&nbsp;</small></td><td><small>&nbsp;%s&nbsp;</small></td><td><small>&nbsp;%s&nbsp;</small></td><td>&nbsp;<a href=\"%s\">%s</a>&nbsp;</td></tr>\n", ++cnt, ctime1, str_ssi(pNick), pUrl, pUrl);

					if (memcmp(ct, ctime2, 8))
					{
					char ctime3[128];
						strftime(ctime3, 128, "%A, %B %d, %Y", timeinfo);
						//printf("<tr><td colspan=4 align=right><small><b>%s&nbsp;</b></small></td></tr>\n", ctime3);
						printf("<tr><td colspan=4 align=right><small><b><a href=\"?d=%d\">%s</a>&nbsp;</b></small></td></tr>\n", date, ctime3);
						printf("<tr><td><small>%d</small></td><td><small>%s</small></td><td><small>&nbsp;<b>%s</b></small></td><td>&nbsp;<a href=\"%s\">%s</a></td></tr>\n", ++cnt, ctime1+9, str_ssi(pNick), pUrl, pUrl);
						memcpy(ct, ctime2, 8);
					}
					else
						printf("<tr><td><small>%d</small></td><td><small>%s</small></td><td><small>&nbsp;<b>%s</b></small></td><td>&nbsp;<a href=\"%s\">%s</a></td></tr>\n", ++cnt, ctime1+9, str_ssi(pNick), pUrl, pUrl);
				}
				
//				printf("<tr><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td><a href=\"%s\">%s</a></td></tr>\n", ++cnt, ctime(&date), str_ssi(pNick), str_ssi(pChan), pUrl, pUrl);
			}
			if (cnt <= 0)
				printf("<tr><td colspan=\"4\">No entries</td></tr>\n");
//				printf("<tr><td colspan=\"5\">No entries</td></tr>\n");

//				printf("<tr><td colspan=\"4\">No entries</td></tr>\n");
				
				
			fclose(f);
			return true;
		}



			// Count stats
			while (fgets(line, 10239, f))
			{
				char *pDate = line;
				char *pNick = strchr(line, ' ');
				if (!pNick) continue;
				*pNick = 0; pNick++;
				u_trim2(&pNick, DTC);
				char *pChan = strchr(pNick, ' ');
				if (!pChan) continue;
				*pChan = 0; pChan++;
				u_trim2(&pChan, DTC);
				char *pUrl = strchr(pChan, ' ');
				if (!pUrl) continue;
				*pUrl = 0; pUrl++;
				
				bool isNew = true;
				for (int i = 0; i < snn; i++)
				{
					if (!strcmp(snicks[i], pNick))
					{
						isNew = false;
						usnicks[i]++;
						break;
					}
				}
				if (isNew && snn < 512)
				{
					strcpy(snicks[snn], pNick);
					usnicks[snn++]++;
				}
				
				isNew = true;
				for (int i = 0; i < snc; i++)
				{
					if (!strcmp(schans[i], pChan))
					{
						isNew = false;
						uschans[i]++;
						break;
					}
				}
				if (isNew && snc < 512)
				{
					strcpy(schans[snc], pChan);
					uschans[snc++]++;
				}
			}
			
			
			// Dump nicks
			
			////////////////////////////////////////////////////////////////////////
			if (fltStat)
			{
				//printf("stats");

			if (snn > 0)
			{
				for (int i = 0; i < snn; i++)
				{
					for (int i1 = 0; i1 < snn-1; i1++)
					{
						char t[32];
						int n;
						if (usnicks[i1] <  usnicks[i1+1])
						{
							strcpy(t, snicks[i1]);
							n = usnicks[i1];
							strcpy(snicks[i1], snicks[i1+1]);
							usnicks[i1] = usnicks[i1+1];
							strcpy(snicks[i1+1], t);
							usnicks[i1+1] = n;
						}
					}
				}
				
				printf("<tr><td colspan=4 align=left><small><b>Nick ratings:</b></small></td></tr>\n");
				for (int i = 0; i < snn; i++)
				{
					printf("<tr><td class=line><small>%d</small></td><td class=line colspan=2><small><a href=\"?n=%s\">%s</a></small></td><td class=line><small>&nbsp;%d&nbsp;</small></td></tr>\n", (i+1), snicks[i], snicks[i], usnicks[i]);
				}
			}

			// Dump chans
			if (snc > 0)
			{
				for (int i = 0; i < snc; i++)
				{
					for (int i1 = 0; i1 < snc-1; i1++)
					{
						char t[32];
						int n;
						if (uschans[i1] < uschans[i1+1])
						{
							strcpy(t, schans[i1]);
							n = uschans[i1];
							strcpy(schans[i1], schans[i1+1]);
							uschans[i1] = uschans[i1+1];
							strcpy(schans[i1+1], t);
							uschans[i1+1] = n;
						}
					}
				}
				
				printf("\n&nbsp;\n<tr><td colspan=4 align=left><small><b>Channel ratings:</b></small></td></tr>\n");
				for (int i = 0; i < snc; i++)
				{
					printf("<tr><td><small>%d</small></td><td colspan=2><small><a href=\"?c=%s\">%s</a></small></td><td><small>&nbsp;%d&nbsp;</small></td></tr>\n", (i+1), schans[i]+1, schans[i], uschans[i]);
				}
			}


			}
			////////////////////////////////////////////////////////////////////////

			else
			{
			if (snn > 0)
			{
				int maxN = 0;
				for (int i = 0; i < snn; i++)
				{
					if (usnicks[i] > maxN) maxN = usnicks[i];
				}
				
				for (int i = 0; i < snn; i++)
				{
					for (int i1 = 0; i1 < snn-1; i1++)
					{
						char t[32];
						int n;
						if (strcmp(snicks[i1], snicks[i1+1]) > 0)
						{
							strcpy(t, snicks[i1]);
							n = usnicks[i1];
							strcpy(snicks[i1], snicks[i1+1]);
							usnicks[i1] = usnicks[i1+1];
							strcpy(snicks[i1+1], t);
							usnicks[i1+1] = n;
						}
					}
				}
				
				printf("<tr><td colspan=4 align=left><small><b>Nicks </b>(%d): &nbsp;\n", snn);
				for (int i = 0; i < snn; i++)
				{
					if (maxN == usnicks[i])
						printf("<a href=\"?n=%s\"><b>%s</b> (%d)</a>%s", snicks[i], snicks[i], usnicks[i], (i == snn-1)?"":", &nbsp;");
					else
					{
						if (usnicks[i] > 1)
							printf("<a href=\"?n=%s\">%s (%d)</a>%s", snicks[i], snicks[i], usnicks[i], (i == snn-1)?"":", &nbsp;");
						else
							printf("<a href=\"?n=%s\">%s</a>%s", snicks[i], snicks[i], (i == snn-1)?"":", &nbsp;");
					}
				}
				printf("</small></td></tr>\n");
			}

			// Dump chans
			if (snc > 0)
			{
				int maxN = 0;
				for (int i = 0; i < snc; i++)
				{
					if (uschans[i] > maxN) maxN = uschans[i];
				}
				
				for (int i = 0; i < snc; i++)
				{
					for (int i1 = 0; i1 < snc-1; i1++)
					{
						char t[32];
						int n;
						if (strcmp(schans[i1], schans[i1+1]) > 0)
						{
							strcpy(t, schans[i1]);
							n = uschans[i1];
							strcpy(schans[i1], schans[i1+1]);
							uschans[i1] = uschans[i1+1];
							strcpy(schans[i1+1], t);
							uschans[i1+1] = n;
						}
					}
				}
				
				printf("<tr><td colspan=4 align=left><small><b>Channels</b> (%d): &nbsp;\n", snc);
				for (int i = 0; i < snc; i++)
				{
					if (maxN == uschans[i])
						printf("<a href=\"?c=%s\"><b>%s</b> (%d)</a>%s", schans[i]+1, schans[i], uschans[i], (i == snc-1)?"":", &nbsp;");
					else
					{
						if (uschans[i] > 1)
							printf("<a href=\"?c=%s\">%s (%d)</a>%s", schans[i]+1, schans[i], uschans[i], (i == snc-1)?"":", &nbsp;");
						else
							printf("<a href=\"?c=%s\">%s</a>%s", schans[i]+1, schans[i], (i == snc-1)?"":", &nbsp;");
					}
				}
					printf("</small></td></tr>\n");
			}
			}

			// Do dump
			cnt = 0;
			rewind(f);
			while (fgets(line, 10239, f) && !fltStat)
			{
				char *pDate = line;
				char *pNick = strchr(line, ' ');
				if (!pNick) continue;
				*pNick = 0; pNick++;
				u_trim2(&pNick, DTC);
				char *pChan = strchr(pNick, ' ');
				if (!pChan) continue;
				*pChan = 0; pChan++;
				u_trim2(&pChan, DTC);
				char *pUrl = strchr(pChan, ' ');
				if (!pUrl) continue;
				*pUrl = 0; pUrl++;
				//u_trim2(&pUrl, DTC);
				u_trim(pUrl, DTC);
				time_t date = atol(pDate);
				struct tm* timeinfo;
				char ctime1[128];
				char ctime2[128];
				timeinfo = localtime(&date);
				strftime(ctime1, 128, "%x %X", timeinfo);
				memcpy(ctime2, ctime1, 8);
				ctime2[8] = 0;
				
				if (memcmp(ct, ctime2, 8))
				{
					char ctime3[128];
					strftime(ctime3, 128, "%A, %B %d, %Y", timeinfo);
					//printf("<tr><td colspan=4 align=right><small><b>%s&nbsp;</b></small></td></tr>\n", ctime3);
					printf("<tr><td colspan=4 align=right><small><b><a href=\"?d=%d\">%s</a>&nbsp;</b></small></td></tr>\n", date, ctime3);
					printf("<tr><td><small>%d</small></td><td><small>%s</small></td><td><small>&nbsp;<b>%s</b> at %s</small></td><td>&nbsp;<a href=\"%s\">%s</a></td></tr>\n", ++cnt, ctime1+9, str_ssi(pNick), str_ssi(pChan), pUrl, pUrl);
					memcpy(ct, ctime2, 8);
				}
				else
					printf("<tr><td><small>%d</small></td><td><small>%s</small></td><td><small>&nbsp;<b>%s</b> at %s</small></td><td>&nbsp;<small><a href=\"%s\">%s</a></small></td></tr>\n", ++cnt, ctime1+9, str_ssi(pNick), str_ssi(pChan), pUrl, pUrl);
//				printf("<tr><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td><a href=\"%s\">%s</a></td></tr>\n", ++cnt, ctime(&date), str_ssi(pNick), str_ssi(pChan), pUrl, pUrl);
			}
			if (cnt <= 0 && !fltStat)
				printf("<tr><td colspan=\"4\">No entries</td></tr>\n");
//				printf("<tr><td colspan=\"5\">No entries</td></tr>\n");

			fclose(f);
			return true;
		}
		while (false);

		return false;

	}
};
