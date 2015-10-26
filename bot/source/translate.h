#define DIR_AUTO				0
#define DIR_UNKNOWN				1

#define DIR_RUSSIAN_ENGLISH		2
#define DIR_ENGLISH_RUSSIAN		3

// #define DICT_RUSSIAN_ENGLISH	"/usr/irc-bots/tools/dict/Russian_English.wb"
// #define DICT_ENGLISH_RUSSIAN	"/usr/irc-bots/tools/dict/English_Russian.wb"

#define DICT_RUSSIAN_ENGLISH	"/home/olivka/bot/dict/Russian_English.wb"
#define DICT_ENGLISH_RUSSIAN	"/home/olivka/bot/dict/English_Russian.wb"

// #define CDICT_RUSSIAN_ENGLISH	"/usr/irc-bots/tools/dict/Russian_English.alt"
// #define CDICT_ENGLISH_RUSSIAN	"/usr/irc-bots/tools/dict/English_Russian.alt"

#define CDICT_RUSSIAN_ENGLISH	"/home/olivka/bot/dict/Russian_English.alt"
#define CDICT_ENGLISH_RUSSIAN	"/home/olivka/bot/dict/English_Russian.alt"

#define TM_WORD_SIZE			0
#define TM_TRANSLATION_SIZE		1
#define TM_INVALID_DIRECTION	2
#define TM_ACCESS_DENIED		3
#define TM_UPDATED				4
#define TM_ADDED				5

#define DICT_MAX_SOURCE		128
#define DICT_MAX_TARGET		512

#pragma pack(1)

typedef struct tagDICT_ENTRY
{
	char source[31];
	char target[53];

} DICT_ENTRY, *LPDICT_ENTRY;

typedef struct tagDICT2_ENTRY
{
	char source[DICT_MAX_SOURCE];

	char target[DICT_MAX_TARGET];
	char target_o[DICT_MAX_TARGET];
	char nick[16];
	char nick_o[16];
	time_t time;
	time_t time_o;

} DICT2_ENTRY, *LPDICT2_ENTRY;

#pragma pack()


int getDirection(const char *);
int charLower(char *);

// (_O_)
bool _convert(const char *);
bool _translate(const char *, char *, int, int);
bool _findWord(const char *, char *, int, int, const char *, const char *);
int _addWord(const char *, const char *);
bool _t_translate(char *, char *, char *);
void _t_addWord(char *, char *, const char *, const char *);

bool findWordEx(const char *, char *, int, int, const char *);

// new 2006
bool translate2(const char *, LPDICT2_ENTRY,  int, bool);
bool findWord2(const char *, LPDICT2_ENTRY, int, const char *, const char *, bool);
bool findWordEx2(const char *, LPDICT2_ENTRY, int, const char *);
int addWord2(const char *, const char *, const char *);


bool t_translate2(char *, char *, char *, bool full);
void t_addWord2(char *, char *, const char *, const char *);
