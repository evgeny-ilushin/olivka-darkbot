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

public:

	url_cache(char *fn) { m_fn = fn; }

	bool add(LPURL fu)
	{
		if (strlen(fu->href) > 19
&& (
!strncmp("http://img.vim-cn.com", fu->href, 21)
|| !strncmp("http://redip.ru/tg/", fu->href, 19)
|| !strncmp("http://temp.redip.ru", fu->href, 20)
|| !strncmp("http://newtemp.redip.ru", fu->href, 23)
))
		    return false;

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
		if (strlen(url) > 19 
&& (
!strncmp("http://img.vim-cn.com", url, 21)
|| !strncmp("http://redip.ru/tg/", url, 19)
|| !strncmp("http://temp.redip.ru", fu->href, 20)
|| !strncmp("http://newtemp.redip.ru", fu->href, 23)
))
		    return false;

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
				if (!stricmp(url2, pUrl))
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
			while (*p && ((*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') || *p == ',' || *p == '.' || *p == '(' || *p == ')' || *p == '&' || *p == '?' || *p == '-'|| *p == '/'|| *p == '\\' || *p == '%' || *p == '~' || *p == '{' || *p == '}' || *p == '[' || *p == ']' || *p == '_' || *p == '+' || *p == ';' || *p == '=' || *p == ':'))
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
};
