#define MAX_NICK 32
#define MAX_NICKS 2000

#define BC_MAX_ONCE 5
#define BC_PUNISH   10

#define bc1_MAX_ONCE 5
#define bc1_PUNISH   10

//#include <string.h>

typedef struct _tagBCNICK
{
    char nick[32];
    long lastSeen;
    long counter;
    int counter1;
} BCNICK, *LPBCNICK;

extern int bc_numKnownNicks;
extern BCNICK bc_nicks[MAX_NICKS];
extern char bc_file_path[];
extern int rnd(int max);

extern int bc1_numKnownNicks;
extern BCNICK bc1_nicks[MAX_NICKS];
extern char bc1_file_path[];

void bc_timeAgo(long d, char *buf);
void bc_reset();
int bc_count(char *text);
int bc_update(char *nick, char *text);
int bc_dice(char *nick, int sum);
int bc_info5(char *buf);
int bc_info5_f(char *buf);
int bc_info10(char *buf);
int bc_infoU(char *nick, char *buf);
void bc_sort();
void bc_sort_f();

void bc_load();
void bc_save();


void bc1_timeAgo(long d, char *buf);
void bc1_reset();
int bc1_count(char *text);
int bc1_update(char *nick, char *text);
int bc1_dice(char *nick, int sum);
int bc1_info5(char *buf);
int bc1_info5_f(char *buf);
int bc1_info10(char *buf);
int bc1_infoU(char *nick, char *buf);
void bc1_sort();
void bc1_sort_f();

void bc1_load();
void bc1_save();



