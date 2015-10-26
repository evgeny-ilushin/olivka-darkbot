#define MATCH_MONTH			0x01
#define MATCH_WEEKDAY			0x02

// #define DATES_FILE_PATH		"/usr/irc-bots/tools/dates/main.dat"
// #define DATES_FILEBK_PATH	"/usr/irc-bots/tools/dates/main.bk"

#define DATES_FILE_PATH		"/home/olivka/bot/dates/main.dat"
#define DATES_FILEBK_PATH	"/home/olivka/bot/dates/main.bk"

#define MAX_DATE_INFO2	1024

#pragma pack(1)

typedef struct tagDATE_INFOex
{
	int id;
	unsigned char day;
	unsigned char month;
	unsigned char weekDay;
	unsigned short int year;
	unsigned char type;
	char text[256];

} DATE_INFOex, *LPDATE_INFOex;

typedef struct tagDATE_INFO
{
	int id;
	unsigned char day;
	unsigned char month;
	unsigned char weekDay;
	unsigned short int year;
	unsigned char type;
	char text[MAX_DATE_INFO2];
	char nick[16];
	time_t created;

} DATE_INFO, *LPDATE_INFO;

typedef struct tagDATE_INFO_HEADER
{
	char logo[3];
	long lastId;
	long numEntries;

} DATE_INFO_HEADER, *LPDATE_INFO_HEADER;

#pragma pack()


bool filter(char *, LPDATE_INFO);
bool parseDate(char *, LPDATE_INFO);

int trim(char **);
int trim2(char **);
int getDateRecords(LPDATE_INFO, int, time_t);
bool addDateRecordFromQuery(char *, int *, LPDATE_INFO, char *nick);
bool addDateRecord(LPDATE_INFO, int *, LPDATE_INFO);
bool delDateRecord(int);
bool readHeader(FILE *, LPDATE_INFO_HEADER);
bool writeHeader(FILE *, LPDATE_INFO_HEADER);
bool getDayName(int, char *);

// +30/03/2007
bool updateDateRecord(LPDATE_INFO);
bool updateDateRecordFromQuery(char *, int *, char *nick);


