char *xtrim(char *);

int escapeWord2(char *word)
{
    int wl = strlen(word), wl2 = 0, i = 0;
for (i = 0; i < wl; i++)
{
    if (isalpha(word[i]))
	wl2++;
    else
    {
        word[i] = 0;
	break;
    }
}
return wl2;
}

bool dict(char *channel, char *nick, char *word, char *dictionary, bool silent)
{
	char *xchr, *buffer;
	char sysCommand[512];
	int maxLines = 10;
	int lineCount = 0;
	int replCount = 1;
	bool typeID = false;
	bool noHeader = true;
	bool singleLine = false;
	bool saidSomething = false;
	
	int wl2 = escapeWord2(word);

	if (wl2 < 1 && !silent)
	{
		S("PRIVMSG %s :%s, bad request\n", channel, nick);
		return false;
	}
	sprintf(sysCommand, "dict -C -d %s %s\n", dictionary, word);
	memset(f_tmp, 0, STRING_LONG);
	buffer = (char *)run_program(sysCommand);

	if (!memcmp(buffer, "No definitions found", 19) && !silent)
		S("PRIVMSG %s :%s, no definitions found\n", channel, nick);
	else
	{
		singleLine = *buffer == '1';
		while ((buffer = strchr(buffer, '\n')) && lineCount <= maxLines)
//		while (fgets(buffer, 512, reply) && lineCount <= maxLines)
		{
			xchr = xtrim(buffer+1);
			if (strlen(xchr))
			{
				if (!strncmp(xchr, "(END", 4))
				{
					break;
				}
				else
				if (!strncmp(xchr, "From V.E.R.A.", 13))
				{
					//fgets(buffer, 512, reply);
					//fgets(buffer, 512, reply);
					xchr = xtrim(strchr(xchr, '\n'));
					if (xchr)// xchr = xtrim(strchr(xchr, '\n'));
					{
						typeID = true;
						buffer = xchr;
					}
					continue;
				}
				if (typeID)
				{
					if (singleLine)
					{
						char *rchr = strchr(xchr, '\r');
						if (rchr) *rchr = 0;
						S("PRIVMSG %s :%s, %s\n", channel, nick, xchr);
						saidSomething = true;
						if (rchr) *rchr = '\r';
						buffer = xchr;
						replCount++;
						break;
					}
					else
					{
						if (noHeader)
						{
							S("PRIVMSG %s :%s:\n", channel, nick);
							noHeader = false;
						}
						
						char *rchr = strchr(xchr, '\r');
						if (rchr) *rchr = 0;
						S("PRIVMSG %s :%d. %s\n", channel, replCount, xchr);
						saidSomething = true;
						if (rchr) *rchr = '\r';
						buffer = xchr;
						replCount++;
					}
				}
				else
				{
					char *rchr = strchr(xchr, '\n');
					if (rchr) *rchr = 0;
					S("PRIVMSG %s :  %s\n", channel, xchr);
					if (rchr) *rchr = '\n';
					buffer = xchr;
					//replCount++;
				}
					
				lineCount++;
				typeID = false;
			}
		}
	}
	
//	fclose(reply);	
//	unlink(fileName);
	
	return saidSomething;
}

char *xtrim(char *buffer)
{
	char *result = buffer;
	int bufLength = strlen(buffer);

	while (*result == ' ' || *result == '\t' || *result == '\r' || *result == '\n')
		result++;
	
	while ((buffer[bufLength-1] == ' ') || (buffer[bufLength-1] == '\t') || (buffer[bufLength-1] == '\r') || (buffer[bufLength-1] == '\n'))
		buffer[--bufLength] = '\x0';
		
	return result;
}

