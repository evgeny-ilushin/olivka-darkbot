int seenBy(char *text, char res[][256], int *nres, int pos, bool noIdent);
int seenByNick(char *nick, char res[][256], int *nres, bool noIdent);
int seenByAddr(char *addr, char res[][256], int *nres, bool noIdent);
int nicksByAddr(char *addr, char *res, bool noIdent);
int nicksByAddrEx(char *addr, char *res);
