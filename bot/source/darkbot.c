/* Darkbot - Internet Relay Chat Help Robot, darkbot.c
*
* Copyright (C) 2000 Jason Hamilton <jason@darkbot.net>
* http://darkbot.net
*
*
* This program is provided free for non-commercial use only. Any
* commercial use of this source code or binaries compiled thereof
* requires the prior written consent of the author. Contact
* jason@darkbot.net with any questions regarding commercial use.
* Distriubution of modified source code or binaries compiled from
* modified source code is expressly forbidden.

  Darkbot Revision: 6f6-r6
  
*/

/*********************************************************
* ATTENTION: If you are looking for the DEFINES that are
* usually located here, please be aware as of 6.x versions
* of darkbot, they are all located in "defines.h". You
* should not need to edit darkbot.c anymore.
*********************************************************/

#define ON		1
#define OFF 	0
#define R		return

#define STRING_SHORT	512
#define STRING_LONG 	2096

#define LEGAL_TEXT "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-"
#define SAFE_LIST "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define NUMBER_LIST "1234567890"

//#ifndef LANG					/* If not defined, default to english */
//#define LANG 1
//#endif

#include "defines.h"
#include "dbtools.h"
#include "victorina.h"
#include "bc.h"

// 2017
#define TGLOG "/home/olivka/bot/logs/tg.log"
#include <pthread.h>
void start_telegram_reader();
void* telegram_reader(void *arg);
pthread_t ttid;

#include <pthread.h>

#define ENABLE_LATIN

#define UCN "/home/olivka/bot/dat/urls.txt"

// Write I/O
//#define DEBUG2

bool isRuAlpha2(char c) { return c >= 'А' && c <= 'я'; }

// <zloy>
void trace(char *);
void resetNames();
void loadKnownNames();
void addNick(char *, int, int);
int getNickType(char *);
bool checkSex(char *, int, char *);
void do_topicstring(char *, char *, bool, char *, char *, char *);
void parseMultiLine(char *, char *);
bool mapString(char *);

char *wt_tokenize(char *);
char *wt_tokenize2(char **);
char *wt_trimRight(char *);

bool assholize(char *, char *);
bool puppylize(char *);

void dictLatin(char *nick, char *chan, char *src);
void dictLatinList(char *nick, char *chan, char *src);

int t_topekInfo(char *channel, char *nick);
int t_topegInfo(char *channel, char *nick);
int t_glotokInfo(char *channel, char *nick);
int t_glotokInfoUa(char *channel, char *nick);

int nrand(int);

char *male_nicks[64];
char *female_nicks[64];

int num_male_nicks;
int num_female_nicks;

char chr_map_eng[] = { 'o', 'e', 'p', 'a', 'x', 'c', 'y' };
char chr_map_rus[] = { 'о', 'е', 'р', 'а', 'х', 'с', 'у' };

void do_pluginCall(char *target, char *file, char *nick, char *topic);
void showDateDirect(char *channel, char *nick, time_t time);
void showDateByIndex(char *channel, char *nick, char *sindex);

void verify_kick_protect(char *s2, char *s1, char *s, char *s3);

// +25/06/2007
char *s_time(time_t time = 0);
char time_s[64] = "N/A";


#define RNICK_CACHE_TIME 300	// 5 min

#define HID_DELAY1	 3000
#define HID_DELAY2	 17000

#define SENDMAIL_TIME	60
time_t g_last_sendmail_time = 0;

time_t  g_lnow = 0;
char g_rnick_cache[1024] = "Somebody";
char last_rdb_file[128] = "";


// <RTC>
#define MAX_INT_VARS	10

#define INTV_KICK		0
#define INTV_SOWHAT		1
#define INTV_AOP		2
#define INTV_AVOICE		3
#define INTV_GREET		4
#define INTV_DICT		5
#define INTV_RSTUFF		6
#define INTV_ASSHOLE		7
#define INTV_ASSNICK		8
#define INTV_VIKTORINA		9

unsigned long inv_vars[MAX_INT_VARS] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
char *inv_var_names[MAX_INT_VARS] = { "КИКБАННАХ", "НУ_ЧТО", "AUTOOP", "AUTOVOICE", "GREETINGS", "WRITEDICT", "RANDOMSTUFF", "ЗАЖОПЛИВАНИЕ", "AUTOASS", "VIKTORINA" };

int varIdByName(char *);
void upperCase2(char *);
void lowerCase2(char *);
void case2(char *, int);

void getIntVar(char *channel, char *nick, char *);
void setIntVar(char *channel, char *nick, char *, char *);
// </RTC>


// Forbidden channels:
// No talks and commands:
int isNoChan1(char *chan);
// No dumb talks:
int isNoChan2(char *chan);

bool noChanEnable = true;

// </zloy>


/* ------------ Below are function declarations --------------- */
#if STATUS == 1
void parse_252 (char *s), parse_251 (char *s), parse_255 (char *s);
#endif

#ifndef WIN32
inline size_t min (const size_t a, const size_t b);
#endif
char *db_strndup (const char *dupMe, size_t maxBytes);
char **tokenize (char *theString, size_t * numTokens);

void show_seen (char *nick, char *source, char *target),
count_seen (char *source, char *target),
show_info2 (const char *target, const char *source);
long save_seen (char *nick, char *uh, char *chan);

void do_randomtopic (char *target, char *file, char *nick, char *topic);
void get_randomtopic (char *target, char *file, char *nick, char *topic, char *buf);
void do_randomtopic2 (char *target, char *file, char *nick, char *topic, int strictFile);

#ifdef	RANDOM_STUFF
void do_random_stuff (), get_rand_stuff_time (),
del_autotopic (const char *chan), add_randomstuff (char *source,
												   char *target,
												   char *data),
												   do_autotopics ();
#endif
void datasearch (const char *nick, char *topic, char *target),
display_url (char *, char *, char *), set_pass (char *nick, char *uh,
												char *pass,
												char *newpass),
												do_modes (char *source, char *data), process_nick (char *nick,
												char *newnick);
long verify_pass (char *nick, char *chan, char *uh, char *pass),
ifexist_autotopic (char *chan);
#if DO_CHANBOT_CRAP == 1
void save_permbans ();
#endif
void do_quit (const char *nick, long);
#ifdef	DO_MATH_STUFF
void do_math (const char *who, char *target, char *math);
#endif
void parse_who (char *data);
void set_autotopic (char *source, char *target, char *topic);
void delete_user_ram (char *source, char *uh), get_s (),
delete_url (const char *nick, char *topic, char *target),
update_setinfo (const char *new_uh, const char *new_greetz,
				const char *nick);
#if DO_CHANBOT_CRAP == 1
void add_permban (const char *uh, size_t counter, const char *reason);
int del_permban (const char *nick, const char *uh);
#endif
int check_existing_url (const char *source, char *topic,
						char *target);
void show_helper_list (const char *nick, long level),
show_banlist (const char *nick);
int my_chanserv (char *sourse, char *target, char *buf) { return 0; }
void chanserv (char *source, char *target, char *buf),
										   raw_now (char *type), find_url (const char *nick, char *topic,
										   char *target), save_changes (),
										   show_url (char *nick, char *topic, char *target, long, long, char *uh,
										   long);
char *strlwr (char *buf), *rand_reply (const char *nick), *date (),
*get_multiword_topic (char *first), *revert_topic (char *input),
*get_rand_nick (const char *chan), *get_word (long number,
											  char *text);

void info (const char *source, char *target), load_helpers (),
scan_chan_users (char *chan, char *nick, char *banned),
do_login (char *nick, char *pass);
int stricmp (const char *s1, const char *s2);
long do_lastcomm (char *nick, char *target, char *rest),
setinfo_lastcomm (char *rest);
void parse (char *), add_helper (const char *chan, const char *uh,
								 long level, size_t num_join,
								 const char *greetz,
								 const char *pass);
int get_connection (const char *hostname, const char *vhostname,
					int port), readln (), writeln (const char *);
bool check_permban (const char *uh, const char *chan,
					const char *nick);
long cf (char *host, char *nick, char *chan), f_f (char *host),
get_passwd (char *data);
time_t return_useridle (const char *chan, const char *who,
						int toggle);
void a_f (char *host), reset_ (), delete_user (const char *nick,
											   char *chan),
											   add_user (char *chan, char *nick, char *uh, long);
void set_fds (), sig_hup (int), sig_segv (int), save_setup ();
void stripline (char *), init_bot (), sig_alrm (int);
void parse_server_msg (fd_set * read_fds);
void trailing_blanks (char *),
fslog (const char *, const char *, ...),
gs26 (), add_s25 (char *server, long port),
add_banned_server (char *server, char *reason), S (const char *format,
												   ...),
												   del_sendq (long), clear_sendq (long, long);
char L[524], *random_word (char **);
int socketfd, alarmed, check_access (char *uh, char *chan, int toggle,
									 char *nick),
									 match_wild (const char *pattern, const char *str), Send (),
									 get_sendq_count (long);
void check_dbtimers ();

/* ------------ Below are global vars -------------- */
long QUESTIONS = 0, ADDITIONS = 0, DELETIONS = 0, uptime, NO_FLOOD,
NUM_SERV = 0, L_CLIENTS = 0, IRCOPS = 0, xtried = 0, G_USERS =
0, rt = 120, fc, spr = 0, snr = 0, BP = 6667, CHECKED =
1, SEND_DELAY = 1, send_tog = 0, NUM_HELPER = 0, NUMLINESSEEN = 0;
#ifdef	RANDOM_STUFF
long Rand_Stuff = 0, Rand_Idle = 0;
#endif
long AIL4 = 0, Sleep_Toggle = 0, AIL3 = 0, AIL2 = 0, AIL5 = 0,
JOINs = 0, PERMBAN_counter = 0, ram_load_time = 0, AIL9 =
0, AIL666 = 0, AIL8 = 0, LastInput = 0, AIL10 = 0, MARK_CHANGE =
0, html_counter = 0;
char NICK_COMMA[32], COLON_NICK[33];
char ANICK_COMMA[32], COLON_ANICK[33], pass_data[512],
pass_pass[STRING_SHORT], rword[STRING_SHORT];
char lc1[STRING_SHORT] = "0", lc2[STRING_SHORT] = "0",
lc4[STRING_SHORT] = "0", lc3[STRING_SHORT] = "0";
long lcn1 = 0, lcn2 = 0, lcn4 = 0, lcn3 = 0, SeeN = 0, DebuG = 0;
char slc1[STRING_SHORT] = "0", slc2[STRING_SHORT] = "0",
slc4[STRING_SHORT] = "0", slc3[STRING_SHORT] = "0";
long slcn1 = 0, slcn2 = 0, slcn4 = 0, slcn3 = 0;
#ifdef	WIN32
char *rp391 = "niW-6r.6f6tobkraD";
#else
char *rp391 = "w51.2-akvilO";
#endif
char BCOLON_NICK[STRING_SHORT], BCOLON_ANICK[STRING_SHORT], DARKBOT_BIN[STRING_SHORT] = "",
r_reply[STRING_SHORT] = "", data[STRING_SHORT] = "",
g_chan[STRING_SHORT], dbVersion[STRING_SHORT],
strbuff[STRING_SHORT], f_tmp[STRING_LONG], UID[STRING_SHORT] =
"database", BS[STRING_SHORT] = "irc.tula.net", CMDCHAR[2] =
"!", CHAN[STRING_SHORT] = "#ttc", s_Mynick[STRING_SHORT] =
"bot", g_host[STRING_SHORT], Mynick[STRING_SHORT] =
"bot", s_MyAnick[STRING_SHORT] = "оливка", MyAnick[STRING_SHORT] = "оливка",
sleep_chan[STRING_SHORT], VHOST[STRING_SHORT] =
"0", REALNAME[STRING_SHORT] = "http://darkbot.net",
privmsg_log[STRING_SHORT];
#define PBOT "ArchFiend"

char NOCHAN1[STRING_SHORT] = "#bots";
char NOCHAN2[STRING_SHORT] = "#spam";

int sec_sleep(int sec);

/* ------------ Below are structs ------------------ */

struct rusage r_usage;

struct sendq
{
	char data[STRING_SHORT];
	struct sendq *next;
}
*sendqhead = NULL, *sendqtail = NULL;

struct userlist
{								/* internal userlist */
	char chan[STRING_SHORT];
	char nick[STRING_SHORT];
	char uh[STRING_SHORT];
	long level; 				  /* auth */
	short global;				  /* Global user? */
	long idle;
	struct userlist *next;
}
*userhead = NULL;

struct helperlist
{
	char chan[STRING_SHORT];
	char uh[STRING_SHORT];
	char nick[STRING_SHORT];
	long level;
	size_t num_join;
	char greetz[STRING_SHORT];
	char pass[STRING_SHORT];
	struct helperlist *next;
}
*helperhead = NULL;

/**
* 6/23/00 Dan
* - Changed permbanlist to have dynamically allocated
*	userhost and reason fields.
* - Changed type of counter to size_t, this should be an
*	unsigned type.
*/
struct permbanlist
{
	char *uh;
	char *reason;
	size_t counter;
	
	struct permbanlist *next;
}
*permbanhead = NULL;

struct old
{
	char host[200];
	long time;
	int count;
	int value;
	int kick;
}
ood[STRING_SHORT];

struct sl124
{
	char name[STRING_SHORT];
	long port;
	struct sl124 *next;
}
*sh124 = NULL;


/* ------------ Begin function source -------------- */

#ifndef WIN32
const char *
run_program (const char *input)
{
	FILE *read_fp;
	long chars_read;

	
	memset(f_tmp, 0, STRING_LONG);
	read_fp = popen (input, "r");
	if (read_fp != NULL) {
		chars_read = fread (f_tmp, sizeof (char), STRING_LONG, read_fp);
		pclose (read_fp);
		if (chars_read > 0) {
			R f_tmp;
		}
		R "No match";
	}
	return NULL;
}
#endif

void
check_dbtimers ()
{
	DIR *dp;
	long i = 0;
	char filename[STRING_SHORT];
	struct dirent *entry;
	struct stat statbuf;
	FILE *fp;
	char b[STRING_LONG], output[STRING_LONG];
	
	if ((dp = opendir (DBTIMERS_PATH)) == NULL) {
		R;
	}
	while ((entry = readdir (dp)) != NULL) {
		stat (entry->d_name, &statbuf);
		if (S_ISDIR (statbuf.st_mode) && *entry->d_name == '.') {
			continue;				  /* it's a dir, ignore it */
		}
		i = time (NULL);
		if (i >= atoi (entry->d_name)) {
#ifdef WIN32
			sprintf (filename, "%s/%s",
				DBTIMERS_PATH, entry->d_name);
#else
			snprintf (filename, sizeof(filename), "%s/%s",
				DBTIMERS_PATH, entry->d_name);
#endif
			if ((fp = fopen (filename, "r")) == NULL) {
				R;
			}
			while (fgets (b, STRING_LONG, fp)) {
				stripline (b);
#ifdef WIN32
				sprintf (output, "%s\n", b);
#else
				snprintf (output, sizeof(output), "%s\n", b);
#endif
				S (output);
			}
			fclose (fp);
			unlink (filename);
		}
	}
	closedir (dp);
}



// Addons
#include "tg.h"
#include "vera.h"
#include "translate.h"
#include "color.h"
#include "netpack.h"
#include "dates.h"
#include "seen.h"
#include "urlc.h"
#include "defcodes.h"

#include "smail/sendmail.h"

//#include "langtools.h"


char *
get_word (long number, char *string)
{								/* gets a specific word requested */
	long i = 0;
	char *ptr;
	
	number = number - 49;
	
	ptr = strtok (string, "+");
	
	strncpy (f_tmp, ptr, sizeof(f_tmp));
	if (ptr != NULL) {
		while (ptr != NULL) {
			i++;					  /* word number */
			ptr = strtok (NULL, "+");
			if (ptr != NULL) {
				if (i == number) {
#ifdef WIN32
					sprintf (f_tmp,"%s", ptr);
#else
					snprintf (f_tmp, sizeof(f_tmp), "%s", ptr);
#endif
					
					R f_tmp;
				}
			}
		}
		R f_tmp;
	}
	else {						  /* only one word */
		if (number == 1) {
			R f_tmp;
		}
		else
			R "";					  /* no match */
	}
}

char **
tokenize (char *theString, size_t * numTokens)
{
	static char *tokens[STRING_SHORT];
	
	assert (numTokens != NULL && theString != NULL);
	memset (tokens, 0, STRING_SHORT * sizeof (char *));
	
	tokens[(*numTokens = 0)] = strtok (theString, " ");
	if (NULL == tokens[0]) {
		/* 0 tokens */
		return tokens;
	}
	
	while ((tokens[++(*numTokens)] = strtok (NULL, " ")) != NULL) {
		/* NO-OP */ ;
	}
	tokens[*numTokens] = 0;
	
	return tokens;
}

int
check_access (char *uh, char *chan, int toggle, char *nick)
{
	long i = 0, length = 0, A = 0, X = 0, Y = 0;
	struct helperlist *c;
	struct userlist *c2;
	char temp[STRING_LONG] = "";
	c = helperhead;
	c2 = userhead;
	strlwr (uh);
	if (toggle == 0) {			  /* get access level */
		while (c2) {
			if (stricmp (c2->uh, uh) == 0) {
				if ((stricmp (c2->chan, chan) == 0) || (stricmp (c2->chan, "#*") == 0)) {
					if (stricmp (c2->nick, nick) == 0) {
						R c2->level;
					}
				}
			}
			c2 = c2->next;
		}
		return 0;					/* no matches? */
	}
	else
		while (c != NULL) {
			if (!match_wild (c->uh, uh) == 0) {
				if (*c->pass == '0') {
					L001 (nick, Mynick);
					R 0;
				}
				if (c->chan[1] != '*')
					if (stricmp (c->chan, chan) != 0)
						R 0;
					c->num_join++;
					if (*c->greetz == '+')
						A = 1;
					strncpy (data, "", sizeof(data));
					length = Y = strlen (c->greetz);
					if (length > 1) {
						while (length > 0) {
							length--;
							X++;
							if (c->greetz[length] == '^') {
								i++;
#ifdef WIN32
								sprintf (temp,"%s%s", nick, data);
#else
								snprintf (temp, sizeof(temp), "%s%s", nick, data);
#endif
							}
							else if (c->greetz[length] == '%') {
								i++;
#ifdef WIN32
								sprintf (temp, "%ul%s", c->num_join, data);
#else
								snprintf (temp, sizeof(temp), "%ul%s", c->num_join, data);
#endif
							}
							else if (c->greetz[length] == '$') {
								i++;
#ifdef WIN32
								sprintf (temp, "%s%s", uh, data);
#else
								snprintf (temp, sizeof(temp), "%s%s", uh, data);
#endif
							}
							else if (c->greetz[length] == '&') {
								i++;
#ifdef WIN32
								sprintf (temp, "%s%s", chan, data);
#else
								snprintf (temp, sizeof(temp), "%s%s", chan, data);
#endif
							}
							else
#ifdef WIN32
								sprintf (temp, "%c%s",c->greetz[length], data);
#else
							snprintf (temp, sizeof(temp), "%c%s",c->greetz[length], data);
#endif
							if (X == Y && A == 1)
								continue;
							strncpy (data, temp, sizeof(data));
						}					  /* While */
#if JOIN_GREET == 1
						if (i == 0) {
							if (setinfo_lastcomm (uh) == 0) {
								S ("PRIVMSG %s :%s\n",
									//S ("PRIVMSG %s :%ld\2!\2\37(\37%s\37)\37\2:\2 %s\n",
									chan, c->greetz);
							}
						}
						else if (A == 1) {
							if (setinfo_lastcomm (uh) == 0) {
								S ("PRIVMSG %s :\1ACTION %s\1\n", chan, data);
							}
						}
						else {
							if (setinfo_lastcomm (uh) == 0) {
								S ("PRIVMSG %s :%s\n", chan, data);
							}
						}
#endif
						R c->level;
					}
			}
			c = c->next;
		}
		R 0;
}

void
parse_who (char *data)
{
	char *chan, *nick, *ptr, b[STRING_SHORT];
	nick = strtok (data, " ");	  /* botnick */
	strncpy (Mynick, nick, sizeof(Mynick));
	chan = strtok (NULL, " ");
	ptr = strtok (NULL, " ");
#ifdef WIN32
	sprintf (b, "%s@%s", ptr, strtok (NULL, " "));
#else
	snprintf (b, sizeof(b), "%s@%s", ptr, strtok (NULL, " "));
#endif
	nick = strtok (NULL, " ");	  /* server */
	nick = strtok (NULL, " ");
	add_user (chan, nick, b, 1);
}

/// 10240 -> 10 Kb :)
int format_bytes(char *s, unsigned long n, bool kb)
{
	double nd = (double)n;
	if (!kb)
	{
		if (n < 1024)
			return sprintf(s, "%lu bytes", n);
		else if (n < 1048576)
			return sprintf(s, "%.2lf Kb", nd/1024.0);
		else
			return sprintf(s, "%.3lf Mb", nd/1048576.0);
	}
	else
	{
		if (n < 1024)
			return sprintf(s, "%lu Kb", n);
		else if (n < 1048576)
			return sprintf(s, "%.2lf Mb", nd/1024.0);
		else
			return sprintf(s, "%.3lf Gb", nd/1048576.0);
	}

}

void
delete_user (const char *nick, char *chan)
{
	struct userlist *pNode, *pPrev;
	
	pNode = userhead;
	pPrev = NULL;
	
	while (pNode) {
		if (stricmp (pNode->nick, nick) == 0
			&& stricmp (pNode->chan, chan) == 0) {
			save_seen (pNode->nick, pNode->uh, pNode->chan);
			if (pPrev != NULL) {
				pPrev->next = pNode->next;
			}
			else {
				userhead = pNode->next;
			}
			free (pNode);
			pNode = NULL;
			break;
		}
		pPrev = pNode;
		pNode = pNode->next;
	}
}

void
do_modes (char *source, char *data)
{
	char *chan, *mode, *nick, *ptr;
	long PM = 0, j = 0, i = 0;
	
	chan = strtok (data, " ");
	mode = strtok (NULL, " ");
	
	if ((ptr = strchr (source, '!')) != NULL)
		*ptr++ = '\0';
	j = strlen (mode);
	i = -1; 					  /* i needs to start at 0 */
	while (j > 0) {
		j--;
		i++;
		if (mode[i] == '+')
			PM = 1;
		if (mode[i] == '-')
			PM = 0;
		if (mode[i] == 'o') {
			nick = strtok (NULL, " ");
			continue;
		}
		if (mode[i] == 'v') {		/* voice sucks, ignore it */
			nick = strtok (NULL, " ");
			continue;
		}
		if (mode[i] == 'k' || mode[i] == 'b') {
			nick = strtok (NULL, " ");
			if (nick[0] == '*' && nick[1] == '!') {
				nick += 2;
			}
			strlwr (nick);
			if (PM == 1)
				scan_chan_users (chan, source, nick);
			continue;
		}
		if (mode[i] == 'l' && PM == 1) {	/* don't parse if -limit
			* since no parms */
			nick = strtok (NULL, " ");
			continue;
		}
	}
	
}

/**
* do_quit
*
* Purpose:
* 1) delete all instances when a nick matches (nick)
* 2) delete all users off a given channel
* 2) delete everything (i.e., when the bot is disconnected from irc)
*
* toggle 1 = delete user.
* toggle 2 = delete chan
* toggle 3 = everything (when I'm killed).
*/
void
do_quit (const char *nick, long toggle)
{
	
	struct userlist *pNode = userhead;
	struct userlist *pPrev = NULL;
	
	if (toggle == 1) {
		/* delete user */
		while (pNode) {
			if (stricmp (pNode->nick, nick) == 0) {
				/* found a match, remove it */
				save_seen (pNode->nick, pNode->uh, pNode->chan);
				if (pPrev != NULL) {
					pPrev->next = pNode->next;
					free (pNode);
					pNode = pPrev->next;
				}
				else {
					/* first node in the list */
					userhead = pNode->next;
					free (pNode);
					pNode = userhead;
				}
			}
			else {
				/* No match, continue to next node */
				pPrev = pNode;
				pNode = pNode->next;
			}
		}
	}
	else if (toggle == 2) {
		/* delete channel */
		while (pNode) {
			if (stricmp (pNode->chan, nick) == 0) {
				/* found a match, remove it */
				save_seen (pNode->nick, pNode->uh, pNode->chan);
				if (pPrev != NULL) {
					pPrev->next = pNode->next;
					free (pNode);
					pNode = pPrev->next;
				}
				else {
					/* first node in the list */
					userhead = pNode->next;
					free (pNode);
					pNode = userhead;
				}
			}
			else {
				/* No match, continue to next node */
				pPrev = pNode;
				pNode = pNode->next;
			}
		}
	}
	else if (toggle == 3) {
		struct userlist *tempPtr = userhead;
		while (pNode) {
			tempPtr = pNode->next;
			free (pNode);
			pNode = tempPtr;
		}
	}
}

void
do_login (char *nick, char *pass)
{
	long i = 0, x = 0, D = 0;
	char Data[STRING_SHORT] = "", b[STRING_SHORT];
	struct userlist *c;
	c = userhead;
	while (c) {
		if (stricmp (nick, c->nick) == 0) {
			x = verify_pass (c->nick, c->chan, c->uh, pass);
			if (x > 0) {
				i++;
				if (c->level == 0 && x >= 2) {
					/* only if not already authed */
					S ("MODE %s +ov %s %s\n", c->chan, c->nick, c->nick);
					D = 1;
				}
				c->level = x;
#ifdef WIN32
				sprintf (b, "%s[%d] %s",c->chan, (int) c->level, Data);
#else
				snprintf (b, sizeof(b), "%s[%d] %s",c->chan, (int) c->level, Data);
#endif
				strncpy (Data, b, sizeof(Data));
			}
		}
		c = c->next;
	}
	if (i != 0) {
		if (!D) {
			S ("NOTICE %s :Already authed on %s\n", nick, Data);
		}
		else
			S ("NOTICE %s :Verified: %s\n", nick, Data);
	}
}

void
process_nick (char *nick, char *newnick)
{
	struct userlist *c;
	c = userhead;
	newnick++;
	while (c) {
		if (stricmp (nick, c->nick) == 0) {
			strncpy (c->nick, newnick, sizeof(c->nick));
		}
		c = c->next;
	}
}

void
scan_chan_users (char *chan, char *nick, char *banned)
{
	struct userlist *c;
	c = userhead;
	if (banned[0] == '*' && banned[1] == '!' && banned[2] == '*'
		&& banned[3] == '\0') {
		S ("MODE %s -ob %s %s\n", chan, nick, banned);
		R;
	}
#if KICK_ON_BAN == 1
	while (c) {
		if (!match_wild (banned, c->uh) == 0) {
			if (stricmp (c->nick, Mynick) != 0) {
				S ("KICK %s %s :BANNED\n", chan, c->nick);
			}
			else {
				S ("MODE %s -ob %s %s\n", chan, nick, banned);
				R;
			}
		}
		c = c->next;
	}
#endif
}

/**
* 6/23/00 Dan:
* - All method arguments are now pointer to const
* - Return type is now time_t
* - A for loop is now used instead of a while loop
*/
time_t return_useridle (const char *chan, const char *who, int toggle)
{								/* toggle=0 is for idle time, toggle=1 is to check if user
	is in the chan */
	const struct userlist *c = userhead;
	
	for (; c != NULL; c = c->next) {
		if (!stricmp (who, c->nick) && !stricmp (chan, c->chan)) {
			if (toggle == 1) {
				/* If we only care if user is present or not.. */
				return 1;
			}
			else
				return c->idle;
		}							/* if */
	}							  /* for */
	return 0;
}

/**
* 6/23/00 Dan:
* - All method arguments are now pointer to const
* - A for loop is now used instead of a while loop
*/
void
show_chaninfo (const char *nick, const char *chan)
{
	size_t totalUsers = 0, foundUsers = 0;
	const struct userlist *c = userhead;
	
	for (; c != NULL; c = c->next) {
		++totalUsers;
		if (!stricmp (chan, c->chan))
			++foundUsers;
	}
	S ("PRIVMSG %s :%s, I see %d users in %s (%d users total in ram)\n",
		chan, nick, foundUsers, chan, totalUsers);
}

/**
* 6/22/00 Dan
* - Removed srand(), should only be done once, in main()
* - Changed while to for loop
*/
char *
__get_rand_nick (const char *chan)
{
	size_t x = 0;
	size_t i = 0;
	const struct userlist *c = userhead;
	f_tmp[0] = 0;
	
	/* Iterate through the userlist */
	for (; c != NULL; c = c->next) {
		/* Check if this user is on the channel */
		if (stricmp (chan, c->chan) == 0) {
			if (stricmp (Mynick, c->nick) != 0) {
				strncpy (f_tmp, c->nick, sizeof(f_tmp));
				i++;
			}
		}
	}
	
	x = nrand(i)+1;
	i = 0;						  /* reinit! */
	
	for (c = userhead; c != NULL; c = c->next) {
		if (stricmp (chan, c->chan) == 0) {
			i++;
			if (i == x) {
				if (*c->nick == '0') {
					return f_tmp;
				}
				if (stricmp (Mynick, c->nick) != 0) {
					strncpy (f_tmp, c->nick, sizeof(f_tmp));
					return f_tmp;
				}
			}
		}
	}
	return f_tmp;
}


char *
get_rand_nick (const char *chan)
{
    // Nick cache
    time_t _now = time(0);
    if (_now - g_lnow <= RNICK_CACHE_TIME)
    {
		strcpy(f_tmp, g_rnick_cache);
		if (strlen(f_tmp) <= 1)
		    strcpy(f_tmp, "ChanServ");
		return f_tmp;
    }
	
    __get_rand_nick(chan);
    if (strlen(f_tmp) <= 1)
	strcpy(f_tmp, "ChanServ");

    // 2018 - add tg nicks
    if (stricmp(f_tmp, "tg") == 0 || stricmp(f_tmp, "Градуснег") == 0 || nrand(7) == 5)
    {
	char *rn2[] = { "@Alexeyushko_N", "@Maks", "@s_e_r_d_j", "@sanhome" , "@mity_f" , "@eddmasteredd" , "@echobyte" , "@Trancer2d" , "@Undcon" , "@Nick_BASSMAN", "@Maks" };
	int rnx2 = nrand(10);
	strcpy(f_tmp, rn2[rnx2]);
    }

    strncpy(g_rnick_cache, f_tmp, 1023);
    g_lnow = _now;
    return f_tmp;
}



void
add_user (char *chan, char *nick, char *uh, long tog)
{
	/* toggle of 0 means to unidle the client */
	struct userlist *n, *c;
	c = userhead;
	if (strlen (uh) > 399)
		uh[399] = '\0';
	while (c) { 				  /* don't readd data that already exists */
		if (tog == 0) {
			if (stricmp (c->nick, nick) == 0
				&& stricmp (c->chan, chan) == 0) {
				c->idle = time (NULL);
			}
		}
		if (tog == 1) {
			if (stricmp (c->nick, nick) == 0
				&& stricmp (c->chan, chan) == 0) {
				/* If user is somehow already here, just update his data instead
				of readding */
				strncpy (c->chan, chan, sizeof(c->chan));
				strncpy (c->uh, uh, sizeof(c->uh));
				strlwr (c->uh);
				strncpy (c->nick, nick,sizeof(c->nick));
				c->idle = time (NULL);
				c->level = 0;
				R;
			}
		}
		c = c->next;
	}
	if (tog == 0) {
		/* all we wanted to do was unidle, so we can quit now */
		R;
	}
	n = (struct userlist *) malloc (sizeof (struct userlist));
	if (n == NULL) {
		fslog ("error.log", "AHHH! No ram left! in add_user!\n");
		R;
	}
	memset (n, 0, sizeof (struct userlist));
	if (n != NULL) {
		strncpy (n->chan, chan, sizeof(n->chan));
		strncpy (n->uh, uh, sizeof(n->uh));
		strlwr (n->uh);
		strncpy (n->nick, nick, sizeof(n->nick));
		n->idle = time (NULL);
		n->level = 0;
		
		n->next = userhead;
		userhead = n;
	}
}

#if DO_CHANBOT_CRAP == 1
/**
* Remove a permban based on nickname and user@host.
* 6/23/00 Dan:
* - Both arguments are now pointers to const data
* - Added free() for both pNode->uh and pNode->reason now
*	that the permbanlist has dynamically allocated fields
* - Changed type of toggle from long to bool
* - Changed name of toggle variable to (foundBan)
* - permbanlist pointers are now initialized when declared
* - Added if statement at end of method, this will only save
*	the bans if a ban was removed
*/
int
del_permban (const char *nick, const char *uh)
{
	
	bool foundBan = false;
	struct permbanlist *pNode = permbanhead, *pPrev = 0;
	
	while (pNode) {
		if (stricmp (pNode->uh, uh) == 0) {
			L002 (nick, PERMBAN_counter, uh);
			PERMBAN_counter--;
			if (pPrev != NULL) {
				pPrev->next = pNode->next;
			}
			else {
				permbanhead = pNode->next;
			}
			
			free (pNode->uh);
			free (pNode->reason);
			free (pNode);
			foundBan = true;
			pNode = NULL;
			break;
		}
		pPrev = pNode;
		pNode = pNode->next;
	}
	
	if (foundBan) {
		/* Only need to save bans if ban list has changed */
		save_permbans ();
	}
	return foundBan;
}
#endif

char *
revert_topic (char *input)
{
	char *ptr, b[STRING_SHORT];
	
	ptr = strtok (input, "+");
	strncpy (f_tmp, ptr, sizeof(f_tmp));
	if (ptr != NULL) {
		while (ptr != NULL) {
			ptr = strtok (NULL, "+");
			if (ptr != NULL) {
#ifdef WIN32
				sprintf (b, "%s %s", f_tmp, ptr);
#else
				snprintf (b, sizeof(b), "%s %s", f_tmp, ptr);
#endif
				strncpy (f_tmp, b, sizeof(f_tmp));
			}
		}
		R f_tmp;
	}
	else
		R f_tmp;
}

char *
get_multiword_topic (char *first)
{
	char *tmp2;
	
	tmp2 = strtok (NULL, " ");
	if (tmp2 != NULL) {
		sprintf (f_tmp, "%s", first);
		while (tmp2 != NULL) {
			sprintf (f_tmp, "%s+%s", f_tmp, tmp2);
			tmp2 = strtok (NULL, " ");
		}
		R f_tmp;
	}
	else
		R first;
}

/**
* Delete one or more elements from the sendq
* 1 = delete all pri/not's
* 0 = delete first in queue
* 6/23/00 Dan
* - Updated to use head and tail pointer queue
* - All variables now initialized when declared
* - Optimized the main while loop a bit, reduced amount of code
*/
void
del_sendq (long toggle)
{
	struct sendq *pNode = sendqhead, *pPrev = 0;
	
	if (NULL == sendqhead) {
		return;
	}
	
	if (toggle == 0) {
		/* Just delete the head */
		pNode = sendqhead;
		sendqhead = sendqhead->next;
		
		free (pNode);
	}
	else {
	/* Iterate through the queue and delete each element which is
	* a PRIVMSG or NOTICE
		*/
		for (; pNode != NULL; pPrev = pNode, pNode = pNode->next) {
			if (0 == strncmp (pNode->data, "PRI", 3)
				|| 0 == strncmp (pNode->data, "NOT", 3)) {
				/* Found one, let's delete it */
				if (pPrev != NULL) {
					pPrev->next = pNode->next;
				}
				else {
					sendqhead = pNode->next;
				}
				free (pNode);
				pNode = NULL;
				break;
			}
		}							/* for */
	}							  /* else */
	
	/* Update the tail pointer if needed */
	if (NULL == sendqhead) {
		sendqtail = NULL;
	}
}

/**
* 6/23/00 Dan
* - Changed method argument to be pointer to const data
* - Initialized b
*/
int
Snow (const char *format, ...)
{
	va_list arglist;
	char b[STRING_LONG] = { 0 };
	
	va_start (arglist, format);
	vsprintf (b, format, arglist);
	va_end (arglist);
	if (DebuG == 1)
		printf ("OUT: %s\n", b);
	R (writeln (b));
}

/**
* 6/23/00 Dan
* - Changed method argument to be pointer to const data
* - Initialized variables when declared
* - Changed b to a power of 2
*/
void
S (const char *format, ...)
{
	va_list arglist;
	char b[STRING_LONG] = { 0 };
	struct sendq *n = 0;
	
	va_start (arglist, format);
	vsprintf (b, format, arglist);
	va_end (arglist);

	bc_update("olivka", b);
	bc1_update("olivka", b);

// Telegram echo - 2017
// tgecho.sh
/*
if (!strncmp(b, "PRIVMSG #irc", 12))
{
    //S ("PRIVMSG ncuxonycbka :<olivka> %s\n", b+13);
    char temp1[10240];
    snprintf (temp1,sizeof(temp1), "/home/olivka/bot/tgecho.sh \"&lt;<b>olivka</b>&gt; %s\"", b+14);
    const char *res = run_program(temp1);
    return;
}
*/
// Reply to TG;
if (!strncmp(b, "PRIVMSG #telegram", 17) && TG_REPLY)
{
    //S ("PRIVMSG ncuxonycbka :<olivka> %s\n", b+13);
    char temp1[10240];
    //snprintf (temp1,sizeof(temp1), "/home/olivka/bot/tgecho.sh \"&lt;<b>olivka</b>&gt; %s\"", b+19);
    snprintf (temp1,sizeof(temp1), "/home/olivka/bot/tgecho.sh \"%s\"", b+19);
    const char *res = run_program(temp1);
    TG_REPLY = 0;
    return;
}
///


	
	if (send_tog == 0) {
		send_tog = 1;
		if (DebuG == 1) {
			printf ("OUT: %s\n", b);
		}
		writeln (b);
		R;
	}
	
	n = (struct sendq *) malloc (sizeof (struct sendq));
	if (n == NULL) {
		fslog ("error.log", "AHH! no ram left! in S!\n");
		R;
	}
	
	memset (n, 0, sizeof (struct sendq));
	strncpy (n->data, b, sizeof(n->data));
	
	if (sendqhead == NULL) {
		sendqhead = sendqtail = n;
	}
	else {
		sendqtail->next = n;
		sendqtail = sendqtail->next;
	}
}

void
count_seen (char *source, char *target)
{
	FILE *fp;
	char temp[STRING_LONG] = "";
	long i = 0;
	if ((fp = fopen (SEEN_FILE, "r")) == NULL) {
		L003 (source, SEEN_FILE);
		R;
	}
	while (fgets (temp, STRING_LONG, fp)) {
		i++;
	}
	fclose (fp);
	L004 (target, source, i);
}

void
show_seen (char *nick, char *source, char *target)
{
	FILE *fp;
	char temp[STRING_LONG] = "", *intime, *r_nick, *uh, *chan, *ptr;
	long unixtime = 0;
	time_t intime_t = 0;
	
	if (nick == NULL)
		R;
	if (strlen (nick) > 30)
		R;
	if (stricmp (nick, source) == 0) {
		L005 (target, source);
		R;
	}
	if ((ptr = strchr (nick, '?')) != NULL)
		memmove (ptr, ptr + 1, strlen (ptr + 1) + 1);
	if ((fp = fopen (SEEN_FILE, "r")) == NULL) {
		L003 (source, SEEN_FILE);
		R;
	}
	while (fgets (temp, STRING_LONG, fp)) {
		stripline (temp);
		r_nick = strtok (temp, " ");
		if (stricmp (nick, r_nick) == 0) {
			uh = strtok (NULL, " ");
			chan = strtok (NULL, " ");
			if (uh == NULL || chan == NULL)
				continue;
			intime = strtok (NULL, " ");
			if (intime == NULL)
				continue;
			intime_t = (time_t)atoi (intime);
			unixtime = time (NULL) - intime_t;
			if (unixtime > 86400)
				S
				("PRIVMSG %s :%s, I last saw %s (%s) %d day%s, %02d:%02d ago (%s) in %s\n",
				target, source, r_nick, uh, unixtime / 86400,
				(unixtime / 86400 == 1) ? "" : "s", (unixtime / 3600) % 24,
				(unixtime / 60) % 60, s_time(intime_t), chan);
			else if (unixtime > 3600)
				S
				("PRIVMSG %s :%s, I last saw %s (%s) %d hour%s, %d min%s ago (%s) in %s\n",
				target, source, r_nick, uh, unixtime / 3600,
				unixtime / 3600 == 1 ? "" : "s", (unixtime / 60) % 60,
				(unixtime / 60) % 60 == 1 ? "" : "s", s_time(intime_t), chan);
			else
				S
				("PRIVMSG %s :%s, I last saw %s (%s) %d minute%s, %d sec%s ago (%s) in %s\n",
				target, source, r_nick, uh, unixtime / 60,
				unixtime / 60 == 1 ? "" : "s", unixtime % 60,
				unixtime % 60 == 1 ? "" : "s", s_time(intime_t), chan);
			fclose (fp);
			R;
		}
	}
	fclose (fp);
	L006 (target, source, nick, SEEN_REPLY);
}

long
save_seen (char *nick, char *uh, char *chan)
{
	FILE *fp;
	char temp[STRING_LONG] = "", *r_nick, *r_chan, *r_uh, *r_time;
	long toggle = 0, unixtime = 0;
	
#ifdef	WIN32
	printf ("*** Writing seen file: %s (%s)\n", CHAN, SEEN_FILE,
		date ());
#endif
	unlink (TMP_FILE);
	if ((fp = fopen (SEEN_FILE, "r")) == NULL) {
		fslog (SEEN_FILE, "%s %s %s %d\n", nick, uh, chan, time (NULL));
		R - 1;
	}
	while (fgets (temp, STRING_LONG, fp)) {
		stripline (temp);
		r_nick = strtok (temp, " ");
		if (stricmp (nick, r_nick) == 0) {
			toggle = 1;
			fslog (TMP_FILE, "%s %s %s %d\n", nick, uh, chan, time (NULL));
		}
		else {
			r_uh = strtok (NULL, " ");
			r_chan = strtok (NULL, " ");
			r_time = strtok (NULL, " ");
			if (r_uh == NULL || r_chan == NULL || r_time == NULL)
				continue;
			unixtime = time (NULL) - atoi (r_time);
			if (unixtime < MAX_LASTSEEN)
				fslog (TMP_FILE, "%s %s %s %s\n", r_nick, r_uh, r_chan, r_time);
		}
	}
	fclose (fp);
	if (toggle == 0) {
		fslog (TMP_FILE, "%s %s %s %d\n", nick, uh, chan, time (NULL));
	}
	rename (TMP_FILE, SEEN_FILE);
	return toggle;
}

#if DO_CHANBOT_CRAP == 1
/**
* Save the permban list to file.
* 6/23/00 Dan:
* - the permbanlist pointer (c) is now pointer to const, because
*	this is a read only method, and that is a read only variable
* - Initialized c when it is declared
*/
void
save_permbans ()
{
	
	const struct permbanlist *c = permbanhead;
	
	unlink (TMP_FILE);
	
#ifdef	WIN32
	printf ("*** Writing permbans: %s (%s)\n", PERMBAN, date ());
#endif
	
	for (; c != NULL; c = c->next) {
		fslog (TMP_FILE, "%s %d %s\n", c->uh, c->counter, c->reason);
	}
	rename (TMP_FILE, PERMBAN);
	
	if (PERMBAN_counter == 0)
		unlink (PERMBAN);
}
#endif

/**
* Remove the autotopic for a particular channel.
* 6/23/00 Dan:
* - Method argument is now pointer to const data
* - All variables are now initialized when declared
* - Changed size of b to be power of 2
* - Changed variable types of toggle and x in accordance
*	with their use
*/
void
del_autotopic (const char *chan)
{
	FILE *fp = 0;
	char b[STRING_LONG] = { 0 }, *r_chan = 0, *r_data = 0;
	bool toggle = false;
	size_t x = 0;
	
	unlink (TMP_FILE);
	fp = fopen (AUTOTOPIC_F, "r");
	if (NULL == fp) {
		return;
	}
	
	while (fgets (b, STRING_LONG, fp)) {
		x++;
		stripline (b);
		
		r_chan = strtok (b, " ");
		r_data = strtok (NULL, "");
		
		if (stricmp (r_chan, chan) == 0) {
			/* Found the channel */
			toggle = true;
		}
		else {
			fslog (TMP_FILE, "%s %s\n", r_chan, r_data);
		}
	}
	
	fclose (fp);
	if (x == 1 && toggle) {
		/* The autotopic file is now empty */
		unlink (AUTOTOPIC_F);
		unlink (TMP_FILE);
		R;
	}
	
	if (toggle) {
	/* We found the topic, change the temp file to the
		* the name of the autotopic file */
		rename (TMP_FILE, AUTOTOPIC_F);
	}
	else {
	/* We were unable to find the channel, just
		* return */
		unlink (TMP_FILE);
	}
}

void
do_autotopics ()
{
	FILE *fp;
	char b[STRING_LONG], *r_chan, *r_data;
	
	if ((fp = fopen (AUTOTOPIC_F, "r")) == NULL)
		R;
	while (fgets (b, STRING_LONG, fp)) {
		stripline (b);
		r_chan = strtok (b, " ");
		r_data = strtok (NULL, "");
		if (*r_data != '0') {
			db_sleep (1);
			S ("TOPIC %s :%s\n", r_chan, r_data);
		}
	}
	fclose (fp);
}

long
ifexist_autotopic (char *chan)
{
	FILE *fp;
	char b[STRING_LONG], *r_chan;
	
	if ((fp = fopen (AUTOTOPIC_F, "r")) == NULL)
		R - 1;
	while (fgets (b, STRING_LONG, fp)) {
		stripline (b);
		if (*b == '/')
			continue;
		r_chan = strtok (b, " ");
		if (stricmp (r_chan, chan) == 0) {
			fclose (fp);
			R 1;					  /* exists */
		}
	}
	fclose (fp);
	R 0;						  /* doesn't exist */
}

void
set_autotopic (char *source, char *target, char *topic)
{
	long exist = 0;
	
	exist = ifexist_autotopic (target);
	if (exist == 0 && *topic == '0') {	  /* never existed, lets humor
		* the guy */
		L007 (source, target);
		R;
	}
	else if (exist == 1 && *topic == '0') { 	  /* delete it! */
		L008 (source, target);
		S ("TOPIC %s :\n", target);
		del_autotopic (target);
		R;
	}
	if (strlen (topic) >= 400)	  /* make sure no overflow */
		topic[400] = '\0';
	if (exist == 0) {			  /* no such autotopic, so add it */
		L009 (source, target, topic);
		fslog (AUTOTOPIC_F, "%s %s\n", target, topic);
		R;
	}
	/* only thing left is if topic exists and you want to update it */
	del_autotopic (target);
	L010 (source, target, topic);
	S ("TOPIC %s :%s\n", target, topic);
	fslog (AUTOTOPIC_F, "%s %s\n", target, topic);
}

#ifdef	RANDOM_STUFF
void
add_randomstuff (char *source, char *target, char *data)
{
	FILE *fp;
	char b[STRING_LONG], file2[STRING_SHORT], *ptr;
	char *i_s = NULL;
	long y_s = 0;
	long int i = 0, x = 0;
	long TOG = 0;
	
	if (*data == '~') {
		data++;
		TOG = 1;
		ptr = strtok (data, " ");
		if (ptr != NULL)
			strlwr (ptr);
		
		if (strspn (ptr, LEGAL_TEXT) != strlen (ptr)) {
			S
				("PRIVMSG %s :%s, rdb file must be made up of letters and or numbers, no other text is accepted.\n",
				target, source);
			R;
		}
#ifdef WIN32
		sprintf (file2, "dat/%s.rdb", ptr);
#else
		snprintf(file2,sizeof(file2),"dat/%s.rdb",ptr);
#endif
		data = strtok (NULL, "");
	}
	else
#ifdef WIN32
		sprintf (file2, "%s", RAND_FILE);
#else
	snprintf(file2,sizeof(file2), "%s", RAND_FILE);
#endif
	if ((fp = fopen (file2, "r")) == NULL) {
		if (TOG == 1) {
			fslog (file2, "1|%ld\n%s\n", y_s, data);
			S ("PRIVMSG %s :Done, there is 1/[%ld] topic under %s\n", y_s, target,
				file2);
		}
		R;
	}
	unlink (TMP_FILE);
	while (fgets (b, STRING_LONG, fp)) {
		stripline (b);
		i++;
		if (*b == '/')
			continue;
		if (i == 1) {
			if (b != NULL) {

		// 2007
		    i_s = strchr(b, '|');
		    if (i_s)
		    {
			*i_s = 0;
			i_s++;
			y_s = atoi(i_s);
		    }
				x = atol (strtok (b, " "));
//				if (y_s > 0)
					fslog (TMP_FILE, "%ld|%ld\n", x + 1, y_s);
//				else
//					fslog (TMP_FILE, "%ld\n", x + 1);
			}
		}
		else {
			fslog (TMP_FILE, "%s\n", b);
		}
	}
	fslog (TMP_FILE, "%s\n", data);
	L011 (target, source, i, y_s);
	fclose (fp);
	rename (TMP_FILE, file2);
}
#endif

#ifdef	RANDOM_STUFF
void
get_rand_stuff_time ()
{
	Rand_Stuff = rand () % RAND_STUFF_TIME + 2;
	if (Rand_Stuff < RAND_STUFF_TIME / 2)
		Rand_Stuff = RAND_STUFF_TIME;
}
#endif

#ifdef	RANDOM_STUFF
/**
* 6/23/00 Dan:
* - Removed an unused variable
* - Changed initialization of temp
* - Changed size of b to be power of 2, and initialized
* - Initialized all other variables
*/
void
do_random_stuff ()
{
	FILE *fp = 0;
	char temp[STRING_SHORT] = { 0 }, b[STRING_LONG] = { 0 }, *b2 = 0;
	bool A = false;
	size_t i = 0;
	size_t length = 0;
	size_t x = 0, y = 0;
	// srand (time (0));   /* this makes things more random! */
	
	if (!inv_vars[INTV_RSTUFF])
		return;
	
	fp = fopen (RAND_FILE, "r");
	if (NULL == fp) {
		return;
	}
	
	while (fgets (b, STRING_LONG, fp)) {
		if (*b == '/') {
			continue;
		}
		i++;
		stripline (b);
		
		if (i == 1) {
			y = atoi (b);
			x = rand () % y + 2;
		}
		if (i == x) {
			if (*b == '+') {
				A = true;
			}
			length = strlen (b);
			i = 0;
			
			memset (data, 0, sizeof (data));
			while (length > 0) {
				i++;
				length--;
				if (b[length] == '~') {
					/* $chan */
#ifdef WIN32
					sprintf (temp, "%s%s", CHAN, data);
#else
					snprintf(temp, sizeof(temp), "%s%s", CHAN, data);
#endif
				}
				else {
#ifdef WIN32
					sprintf (temp, "%c%s", b[length], data);
#else
					snprintf(temp, sizeof(temp), "%c%s", b[length], data);
#endif
				}
				strncpy (data, temp, sizeof(data));
			}
			if (!A) {
				S ("PRIVMSG %s :%s\n", CHAN, data);
				fclose (fp);
				R;
			}
			else {
				b2 = data;
				b2++;
				S ("PRIVMSG %s :\1ACTION %s\1\n", CHAN, b2);
				fclose (fp);
				R;
			}
		}
	}							  /* while() */
	
	/* fclose(fp); */
}
#endif



void do_randomtopic2 (char *target, char *file, char *nick, char *topic, int strictFile)
{
    if (isNoChan2(target))
	return;
	
    if (strictFile && *last_rdb_file)
    {
	
	if (stricmp(file, last_rdb_file) == 0)
	{

	    return;
	}
    }

    do_randomtopic(target, file, nick, topic);
    
    strncpy(last_rdb_file, file, 127);
}

void
do_randomtopic (char *target, char *file, char *nick, char *topic)
{
    if (isNoChan2(target))
	return;
	
	FILE *fp = 0;
	
	char temp[STRING_SHORT] = { 0 };
	char b2[STRING_LONG] = { 0 };
	char *b = 0;
	char file2[STRING_SHORT] = { 0 };
	char Data[STRING_LONG] = { 0 };
	char *i_s = NULL;
	
	long x = 0;
	long y = 0;
	long y_s = 0;
	long A = 0;
	
	size_t i = 0;
//	size_t i_delay = 0;
	size_t length = 0;
	
	bool Tog = false;
	
	if (file != NULL)
#ifdef WIN32
		sprintf (file2, "dat/%s.rdb", file);
#else
	snprintf (file2, sizeof(file2), "dat/%s.rdb", file);
#endif
	if ((fp = fopen (file2, "r")) == NULL) {
	/*
		S ("PRIVMSG %s :Sorry, I cannot answer that topic because "
			"random text file (rdb) \"%s\" was not found.\n",
			target, file2);
	*/
	// 21.jan.2008
		S ("NOTICE %s :Sorry, I cannot answer that topic because "
			"random text file (rdb) \"%s\" was not found.\n",
			target, file2);
		R;
	}
	db_sleep (1);
	//
	sleep(HID_DELAY1+nrand(HID_DELAY2));
	//
	
	db_sleep(1000);
	//
	if (!strcmp(file, DONNO_RDB))
	{
	sec_sleep(1+nrand(10));
	}
	//
	
	while (fgets (b2, STRING_LONG, fp))
	{
		i++;
		stripline (b2);
		if (i == 1)
		{
		    // 2007 N|N format
		    i_s = strchr(b2, '|');
		    if (i_s)
		    {
			*i_s = 0;
			i_s++;
			y_s = atoi(i_s);
//			y_s = 1000*atoi(i_s);
		    }
			y = atoi(b2);
			x = nrand(y)+1;
			
		}
		if (i == x)
		{
			i = 0;
			b = b2;
			
			if (y_s > 0)
			{
    // !!!
//   S ("PRIVMSG zloy : SLEEP %ld\n", y_s);
			    sec_sleep(1+nrand(y_s));
//			    db_sleep(y_s);
			}
			    
    // !!!
//   S ("PRIVMSG zloy : SLEEP DONE %ld\n", y_s);
   
			do_topicstring(b, b2, Tog, nick, target, topic);
			
			fclose (fp);
			R;
		}
	}
	fclose (fp);
}


void
get_randomtopic (char *target, char *file, char *nick, char *topic, char *buf)
{
	FILE *fp = 0;
	
	char temp[STRING_SHORT] = { 0 };
	char b2[STRING_LONG] = { 0 };
	char *b = 0;
	char file2[STRING_SHORT] = { 0 };
	char Data[STRING_LONG] = { 0 };
	char *i_s = NULL;
	
	long x = 0;
	long y = 0;
	long y_s = 0;
	long A = 0;
	
	size_t i = 0;
//	size_t i_delay = 0;
	size_t length = 0;
	
	bool Tog = false;
	
	if (file != NULL)
#ifdef WIN32
		sprintf (file2, "dat/%s.rdb", file);
#else
	snprintf (file2, sizeof(file2), "dat/%s.rdb", file);
#endif
	if ((fp = fopen (file2, "r")) == NULL) {
		*buf = 0;
        	R;
	}
	//db_sleep (1);
	//
	//
	while (fgets (b2, STRING_LONG, fp))
	{
		i++;
		stripline (b2);
		if (i == 1)
		{
		    // 2007 N|N format
		    i_s = strchr(b2, '|');
		    if (i_s)
		    {
			*i_s = 0;
			i_s++;
			y_s = atoi(i_s);
		    }
			y = atoi(b2);
			x = nrand(y)+1;
			
		}
		if (i == x)
		{
			i = 0;
			b = b2;
			
			if (y_s > 0)
			{
    // !!!
//   S ("PRIVMSG zloy : SLEEP %ld\n", y_s);
			    //db_sleep(y_s);
			    db_sleep(1+nrand(y_s));
			}
			    
    // !!!
//   S ("PRIVMSG zloy : SLEEP DONE %ld\n", y_s);
   
			strcpy(buf, b2);
			
			fclose (fp);
			R;
		}
	}
	fclose (fp);
}


/**
* Add a permban to the permban list.
* 6/23/00 Dan:
* - Both pointer variables are now received as pointer to const data
* - Changed counter to type size_t, this should be an unsigned type
* - Initialiazed n to 0 on declaration
* - Added support for dynamically allocated uh and reason fields
*	in the struct permban list
* - Did some extra memory leak prevention
*/
void
add_permban (const char *uh, size_t counter, const char *reason)
{
	
	struct permbanlist *n = 0;
	n = (struct permbanlist *)
		malloc (sizeof (struct permbanlist));
	if (n == NULL) {
		fslog ("error.log", "AHHH! no ram left! in add_permban!\n");
		R;
	}
	
	memset (n, 0, sizeof (struct permbanlist));
	n->uh = db_strndup (uh, STRING_SHORT);
	if (NULL == n->uh) {
		fslog ("error.log", "add_permban> Memory allocation failure\n");
		/* Prevent memory leaks */
		free (n);
		return;
	}
	
	n->reason = db_strndup (reason, STRING_SHORT);
	if (NULL == n->reason) {
		fslog ("error.log", "add_permban> Memory allocation failure\n");
		/* Prevent memory leaks */
		free (n->uh);
		free (n);
		return;
	}
	
	strlwr (n->uh);
	n->counter = counter;
	PERMBAN_counter++;
	n->next = permbanhead;
	permbanhead = n;
}

/**
* Check if a permban exists for a given uh/channel/nick set.
* 6/23/00 Dan:
* - Changed all method arguments to be pointers to const data
* - Return type is now bool, returns true if ban is found,
*	false otherwise
*/
bool
check_permban (const char *uh, const char *chan, const char *nick)
{
	static char tmpBuf[STRING_SHORT + 1];
	struct permbanlist *c = permbanhead;
	strncpy (tmpBuf, uh, min (STRING_SHORT, strlen (uh)));
	strlwr (tmpBuf);
	for (; c != NULL; c = c->next) {
		if (!match_wild (c->uh, tmpBuf) == 0) {
			c->counter++;
			S ("MODE %s +b %s\n", chan, c->uh);
			S ("KICK %s %s :\2[\2%d\2]\2: %s\n",
				chan, nick, c->counter, c->reason);
			R true;
		}
	}
	R false;
}

#ifndef WIN32
size_t min (const size_t a, const size_t b)
{
	return ((a < b) ? a : b);
}
#endif

long
get_pass (char *data)
{
	/* returns 0 for no data */
	/* returns 1 for just pass */
	/* returns 2 for pass and data */
	char b[STRING_SHORT], b2[STRING_SHORT], *temp;
	long i = 0;
	strncpy (pass_data, "0", sizeof(pass_data));  /* init */
	strncpy (pass_pass, "0", sizeof(pass_pass));
	if (data == NULL)
		R 0;
	strncpy (b2, data, sizeof(b2));
	temp = strtok (data, " ");
	if (temp == NULL)
		R - 1;
	strncpy (b, temp, sizeof(b));
	while (temp != NULL) {
		i++;
		strncpy (pass_pass, temp, sizeof(pass_pass));
		temp = strtok (NULL, " ");
		if (temp == NULL)
			break;
#ifdef WIN32
		sprintf (b,"%s %s", b, temp);
#else
		snprintf (b, sizeof(b),"%s %s", b, temp);
#endif
	}
	strncpy (b, "", sizeof(b)); 		  /* reinit */
	temp = strtok (b2, " ");
	strncpy (b, temp, sizeof(b));
	while (i > 2) {
		i--;
		temp = strtok (NULL, " ");
#ifdef WIN32
		sprintf (b,"%s %s", b, temp);
#else
		snprintf (b, sizeof(b),"%s %s", b, temp);
#endif
	}
	if (stricmp (b, pass_pass) == 0) {
		strncpy (pass_data, "0", sizeof(pass_data));
		R 1;
	}
	strncpy (pass_data, b, sizeof(pass_data));
	R 2;
}

void
set_pass (char *nick, char *uh, char *pass, char *newpass)
{
	struct helperlist *c;
	c = helperhead;
	strlwr (uh);
	while (c) {
		if (!match_wild (c->uh, uh) == 0) {
			if (strcmp (c->pass, pass) == 0) {
				strncpy (c->pass, newpass, sizeof(c->pass));
				L012 (nick, uh);
				save_changes ();
				R;
			}
			else {
				L013 (nick);
				R;
			}
		}
		c = c->next;
	}
	L014 (nick);
}

long
verify_pass (char *nick, char *chan, char *uh, char *pass)
{
	struct helperlist *c;
	c = helperhead;
	strlwr (uh);
	while (c) {
		if (!match_wild (c->uh, uh) == 0) {
			if (*c->pass == '0')
				R 0;					/* no pass set */
			if (strcmp (c->pass, pass) == 0) {
				if (c->chan[0] == '#' && c->chan[1] == '*')
					R c->level;
				if (*chan == '*')
					R c->level;
				if (stricmp (c->chan, chan) == 0)
					R c->level;
				R 0;					/* don't match chan access */
			}
		}
		c = c->next;
	}
	R 0;
}

void
delete_user_ram (char *source, char *uh)
{
	struct helperlist *pNode, *pPrev;
	pNode = helperhead;
	pPrev = NULL;
	while (pNode) {
		if (stricmp (pNode->uh, uh) == 0) {
			L015 (source, pNode->uh, pNode->level, pNode->num_join);
			if (pPrev != NULL) {
				pPrev->next = pNode->next;
			}
			else
				helperhead = pNode->next;
			free (pNode);
			pNode = NULL;
			break;
		}
		pPrev = pNode;
		pNode = pNode->next;
	}
	save_changes ();
}

#ifdef	DO_MATH_STUFF
void
do_math (const char *who, char *target, char *math)
{
	char input[STRING_SHORT];
	char number_string[STRING_SHORT];
	char op = 0;
	unsigned int index = 0;
	unsigned int to = 0;
	unsigned int input_length = 0;
	unsigned int number_length = 0;
	double result = 0.0;
	double number = 0.0;
	strncpy (input, math, sizeof(input));
	input_length = strlen (input);
	for (to = 0, index = 0; index <= input_length; index++)
		if (*(input + index) != ' ')
			*(input + to++) = *(input + index);
		input_length = strlen (input);
		index = 0;
		if (input[index] == '=')
			index++;
		else {
			number_length = 0;
			if (input[index] == '+' || input[index] == '-')
				*(number_string + number_length++) = *(input + index++);
			for (; isdigit (*(input + index)); index++)
				*(number_string + number_length++) = *(input + index);
			if (*(input + index) == '.') {
				*(number_string + number_length++) = *(input + index++);
				for (; isdigit (*(input + index)); index++)
					*(number_string + number_length++) = *(input + index);
			}
			*(number_string + number_length) = '\0';
			if (number_length > 0)
				result = atof (number_string);
		}
		for (; index < input_length;) {
			op = *(input + index++);
			number_length = 0;
			if (input[index] == '+' || input[index] == '-')
				*(number_string + number_length++) = *(input + index++);
			for (; isdigit (*(input + index)); index++)
				*(number_string + number_length++) = *(input + index);
			if (*(input + index) == '.') {
				*(number_string + number_length++) = *(input + index++);
				for (; isdigit (*(input + index)); index++)
					*(number_string + number_length++) = *(input + index);
			}
			*(number_string + number_length) = '\0';
			number = atof (number_string);
			switch (op) {
			case '+':
				result += number;
				break;
			case '-':
				result -= number;
				break;
			case '*':
				result *= number;
				break;
			case '/':
				if (number == 0) {
					L016 (target, who);
					R;
				}
				else
					result /= number;
				break;
			case '%':
				if ((long) number == 0) {
					L016 (target, who);
					R;
				}
				else
					result = (double) ((long) result % (long) number);
				break;
			default:
				L017 (target, who);
				R;
			}
		}
		S ("PRIVMSG %s :%s\2:\2 %f\n", target, who, result);
}
#endif

long
cf (char *host, char *nick, char *chan)
{
	int f_n;
	if (check_access (host, chan, 0, nick)
		>= 3)
		R 0;
	f_n = f_f (host);
	if (f_n == -1) {
		a_f (host);
		R 0;
	}
	if (ood[f_n].value)
		R 1;
	ood[f_n].count++;
	if ((time (NULL) - ood[f_n].time) > ft)
		ood[f_n].count = 0;
	else if ((time (NULL) - ood[f_n].time) <= ft
		&& ood[f_n].count >= fr) {
		ood[f_n].value = true;
		if (!ood[f_n].kick) {
			ood[f_n].kick = 1;
#ifdef	FLOOD_KICK
			if (*chan == '#' || *chan == '&') {
				L018 (chan, nick, FLOOD_REASON, fc, host);
			}
			else
				L019 (CHAN, fc, host);
#else
			if (*chan == '#' || *chan == '&') {
				L019 (CHAN, fc, host);
			}
			else
				L019 (CHAN, fc, host);
#endif
		}
		R 1;
	}
	ood[f_n].time = time (NULL);
	R 0;
}

/**
* 6/23/00 Dan:
* - Initialized all variables
*/
void
raw_now (char *type)
{
	FILE *fp = 0;
	long i = 0, counter = 0;
	char str[STRING_LONG] = {
		0
	}
	, *dat = 0, *ptr = 0, *tmp1 = 0, *tmp2 = 0, *tmp3 = 0;
	if (stricmp (type, "PERFORM") == 0)
		if ((fp = fopen (PERFORM, "r")) == NULL)
			R;
		if (stricmp (type, "PERMBAN") == 0)
			if ((fp = fopen (PERMBAN, "r")) == NULL)
				R;
			if (stricmp (type, "DEOP") == 0)
				if ((fp = fopen (DEOP, "r")) == NULL)
					R;
				if (stricmp (type, "SERVERS") == 0)
					if ((fp = fopen (SERVERS, "r")) == NULL) {
						printf
							("%s not found. You must create the file with format:\n",
							SERVERS);
						printf
							("server port ...this list can be as long as you want.\n");
						exit (0);
					}
					if (stricmp (type, "SETUP") == 0)
						if ((fp = fopen (SETUP, "r")) == NULL) {
							printf ("Unable to locate %s! You must run configure!.\n",
								SETUP);
							exit (0);
						}
						while (!feof (fp)) {
							if (stricmp (type, "SETUP") == 0) {
								printf ("Loading %s data...\n", SETUP);
								SeeN = 1;
								while (fgets (str, STRING_LONG, fp)) {
									if (*str == '/')
										continue;
									stripline (str);
									dat = strtok (str, "");
									if ((ptr = strchr (dat, '=')) != NULL)
										*ptr++ = '\0';
									if (stricmp (dat, "NICK") == 0) {
										strncpy (Mynick, ptr, sizeof(Mynick));
										strncpy (s_Mynick, ptr, sizeof(s_Mynick));
#if 	LOG_PRIVMSG == 1
#ifdef WIN32
										sprintf (privmsg_log,"%s%s-privmsg.log",
											LOG_DIR, Mynick);
#else
										snprintf (privmsg_log,sizeof(privmsg_log), "%s%s-privmsg.log",
											LOG_DIR, Mynick);
#endif
#endif
									}
									
									if (stricmp (dat, "ANICK") == 0) {
										strncpy (MyAnick, ptr, sizeof(MyAnick));
										strncpy (s_MyAnick, ptr, sizeof(s_MyAnick));
#if 	LOG_PRIVMSG == 1
#ifdef WIN32
										sprintf (privmsg_log,"%s%s-privmsg.log",
											LOG_DIR, MyAnick);
#else
										snprintf (privmsg_log,sizeof(privmsg_log), "%s%s-privmsg.log",
											LOG_DIR, MyAnick);
#endif
#endif
									}
									else if (stricmp (dat, "USERID") == 0) {
										strncpy (UID, ptr, sizeof(UID));
									}
									else if (stricmp (dat, "CHAN") == 0) {
										strncpy (CHAN, ptr, sizeof(CHAN));
									}
									else if (stricmp (dat, "NOCHAN1") == 0) {
										strncpy (NOCHAN1, ptr, sizeof(NOCHAN1));
									}
									else if (stricmp (dat, "NOCHAN2") == 0) {
										strncpy (NOCHAN2, ptr, sizeof(NOCHAN2));
									}
									else if (stricmp (dat, "SEEN") == 0) {
										SeeN = atoi (ptr);
									}
									else if (stricmp (dat, "VHOST") == 0) {
										strncpy (VHOST, ptr, sizeof(VHOST));
									}
									else if (stricmp (dat, "REALNAME") == 0) {
										strncpy (REALNAME, ptr, sizeof(REALNAME));
									}
									else if (stricmp (dat, "CMDCHAR") == 0) {
										*CMDCHAR = *ptr;
									}
								}
#ifdef	VERB
								printf ("   - botnick(%s),", Mynick);
								printf ("botAnick(%s),", MyAnick);
								printf ("userid(%s),", UID);
								printf ("channel(%s)\n", CHAN);
								printf ("   - cmdchar(%c),", *CMDCHAR);
								printf ("vhost(%s),", VHOST);
								printf ("seen(%s)\n", SeeN == 1 ? "On" : "Off");
								printf ("   - realname(%s)\n", REALNAME);
#endif
							}
							else if (stricmp (type, "PERMBAN") == 0) {
								while (fgets (str, STRING_LONG, fp)) {
									stripline (str);
									tmp1 = strtok (str, " ");
									if (tmp1 == NULL)
										continue;
									tmp2 = strtok (NULL, " ");
									if (tmp2 == NULL)
										tmp2 = "0";
									tmp3 = strtok (NULL, "");
									if (tmp3 == NULL)
										tmp3 = "Permbanned!";
									strlwr (tmp1);
									counter = atoi (tmp2);
									add_permban (tmp1, counter, tmp3);
								}
							}
							else if (stricmp (type, "SERVERS") == 0) {
#ifndef WIN32
								printf ("Loading %s file ", SERVERS);
#endif
								while (fgets (str, STRING_LONG, fp)) {
									i++;
									printf (".");
									fflush (stdout);
									stripline (str);
									tmp1 = strtok (str, " ");
									if (tmp1 == NULL) {
										printf
											("Found error in %s! Aboring! please re-run configure!\n",
											SERVERS); exit (0);
									}
									else
										tmp2 = strtok (NULL, " ");
									if (tmp2 == NULL) {
										printf ("%s has no matching port in %s!\n", tmp1, SERVERS);
										exit (0);
									}
									add_s25 (tmp1, atoi (tmp2));
								}
								printf ("done(%d).\n", (int) i);
							}
							else if (fgets (str, STRING_LONG, fp))
								S ("%s\n", str);
						}
						fclose (fp);
}

/**
* 6/22/00 Dan
* - Function argument is now pointer to const
* - Fixed a problem where the file was never closed
* - All variables are now initialized when declared
* - Removed an unused variable
* - Changed long variables to type size_t, they should be
*	unsigned
* - Changed reinitialization of data
* - Moved the big if/else structure to a switch
*/
char *
rand_reply (const char *nick)
{
	
	FILE *fp = 0;
	char temp[STRING_SHORT] = {
		0
	};
	size_t i = 0, x = 0, y = 0, length = 0;
	fp = fopen (RAND_SAY, "r");
	if (NULL == fp) {
		return 0;
	}
	
	while (fgets (r_reply, STRING_SHORT, fp)) {
		if (*r_reply == '/') {
			continue;
		}
		i++;
		stripline (r_reply);
		if (i == 1) {
			/* Read in random # from top line of * random.ini */
			y = atoi (r_reply);
			x = nrand(y) + 1;
		}
		if (i != x) {
			continue;
		}
		
		/* Found it */
		fclose (fp);
		length = strlen (r_reply);
		i = 0;
		data[0] = 0;
		while (length > 0) {
			i++;
			length--;
			switch (r_reply[length]) {
			case '^':
#ifdef WIN32
				sprintf (temp,"%s%s", nick, data);
				break;
			case '%':
				/* Bold */
				sprintf (temp,"\2%s", data);
				break;
			case '&':
				/* Underline */
				sprintf (temp,"\37%s", data);
				break;
			case '~':
				/* Inverse */
				sprintf (temp,"\26%s", data);
				break;
			default:
				sprintf (temp,"%c%s", r_reply[length], data);
				break;
#else
				snprintf (temp,sizeof(temp), "%s%s", nick, data);
				break;
			case '%':
				/* Bold */
				snprintf (temp,sizeof(temp), "\2%s", data);
				break;
			case '&':
				/* Underline */
				snprintf (temp,sizeof(temp), "\37%s", data);
				break;
			case '~':
				/* Inverse */
				snprintf (temp,sizeof(temp), "\26%s", data);
				break;
			default:
				snprintf (temp,sizeof(temp), "%c%s", r_reply[length], data);
				break;
#endif
			}						  /* switch */
			strncpy (data, temp, sizeof(data));
		}							/* while( length > 0 ) */
		
		return data;				/* Found random line */
	}							  /* while(fgets()) */
	
	/* Unable to find match */
	fclose (fp);
	/* A space is returned to prevent crashing */
	return " ";
}

/**
* Update a nick's channel greeting and user@host.
* 6/23/00 Dan:
* - All method arguments are now pointers to const data
* - Rewrote to use a for loop, and fewer variables
* - Info is only saved to disk if changes are made
*/
void
update_setinfo (const char *new_uh,
				const char *new_greetz, const char *nick)
{
	struct helperlist *c = helperhead;
	bool madeChange = false;
	size_t i = 0;
	for (; c != NULL; c = c->next) {
		++i;
		if (!match_wild (c->uh, new_uh) == 0) {
			strncpy (c->greetz, new_greetz, sizeof(c->greetz));
			strlwr (c->uh);
			L020 (nick, i, c->uh, new_greetz);
			madeChange = true;
		}
	}
	if (madeChange) {
		save_changes ();
	}
}

void
save_setup ()
{
#ifdef	WIN32
	printf ("*** Writing setup file: %s (%s)\n", SETUP, date ());
#endif
	unlink (TMP_FILE);
	fslog (TMP_FILE, "NICK=%s\n", s_Mynick);
	fslog (TMP_FILE, "ANICK=%s\n", s_MyAnick);
	fslog (TMP_FILE, "USERID=%s\n", UID);
	fslog (TMP_FILE, "CHAN=%s\n", CHAN);
	fslog (TMP_FILE, "NOCHAN1=%s\n", NOCHAN1);
	fslog (TMP_FILE, "NOCHAN2=%s\n", NOCHAN2);
	fslog (TMP_FILE, "VHOST=%s\n", VHOST);
	fslog (TMP_FILE, "REALNAME=%s\n", REALNAME);
	fslog (TMP_FILE, "CMDCHAR=%c\n", *CMDCHAR);
	fslog (TMP_FILE, "SEEN=%d\n", SeeN);
	rename (TMP_FILE, SETUP);
}

void
save_changes ()
{
	long i = 0;
	struct helperlist *c;
	c = helperhead;
	unlink (TMP_FILE);
	while (c != NULL) {
		i++;
		fslog (TMP_FILE, "%s %s %d %d %s %s\n",
			c->chan, c->uh, c->level, c->num_join, c->pass, c->greetz);
		c = c->next;
	}
	rename (TMP_FILE, HELPER_LIST);
}

void
datasearch (const char *nick, char *topic, char *target)
{
	FILE *fp;
	long i = 0, FOUND = 0, x = 0;
	char b[STRING_LONG], *dorf, *subj, *ptr2, DATA[STRING_SHORT] = "";
	if (strlen (topic) > MAX_TOPIC_SIZE)
		topic[MAX_TOPIC_SIZE] = '\0';
	strlwr (topic);
	if ((fp = fopen (URL2, "r")) == NULL) {
		L003 (nick, URL2);
		R;
	}
	while (fgets (b, STRING_LONG, fp)) {
		x++;
		stripline (b);
		strlwr (b);
		subj = strtok (b, " ");
		dorf = strtok (NULL, "");
		ptr2 = strstr (dorf, topic);
		if (ptr2 != NULL) {
			i++;
			FOUND = 1;
			sprintf (DATA, "%s %s", DATA, subj);
			if (strlen (DATA) >= MAX_SEARCH_LENGTH)
				break;
		}
	}
	fclose (fp);
	if (FOUND == 0) {
		L021 (target, NO_TOPIC, topic, x);
	}
	else if (i > 19) {
		L022 (target, i, DATA);
	}
	else if (i == 1) {
		L023 (target, nick, DATA);
	}
	else
		L024 (target, i, nick, DATA);
}

/**
* 6/23/00 Dan:
* - All variables now initialized when declared
* - Altered variable types to reflect usage
*/
void
info (const char *source, char *target)
{
	FILE *fp = 0;
	clock_t starttime = 0;
	char b[STRING_LONG] = {
		0
	};
	size_t topics = 0, dup = 0;
	time_t t2time = 0, c_uptime = 0;
#ifdef FIND_DUPS
	char *ptr = 0, *subj = 0;
	size_t last = 0, last2 = 0;
#endif
	t2time = time (NULL);
	unlink (TMP_URL);
	starttime = clock ();
	fp = fopen (URL2, "r");
	if (NULL == fp) {
		L003 (source, URL2);
		R;
	}
	while (fgets (b, STRING_LONG, fp)) {
		topics++;
#ifdef	FIND_DUPS
		stripline (b);
		subj = strtok (b, " ");
		ptr = strtok (NULL, "");
		strlwr (subj);
		if (stricmp (last, subj) == 0) {
			dup++;
#ifdef	SAVE_DUPS
			fslog (BACKUP_DUP, "%s %s\n", subj, ptr);
#endif
		}
		else {
			fslog (TMP_URL, "%s %s\n", subj, ptr);
		}
		strncpy (last2, subj, sizeof(last2));
		last = last2;
#endif
	}
	
	fclose (fp);
	rename (TMP_URL, URL2);
#ifdef	FIND_DUPS
	if (dup > 0) {
		L025 (target, dup);
	}
#endif
	c_uptime = time (NULL) - uptime;
	topics -= dup;
	if (c_uptime > 86400) {
		L026 (target,
			dbVersion,
			topics,
			c_uptime / 86400,
			(c_uptime / 86400 ==
			1) ? "" : "s",
			(c_uptime / 3600) % 24,
			(c_uptime / 60) % 60, QUESTIONS,
			ADDITIONS, DELETIONS,
			(double) (clock () -
			starttime) /
			CLOCKS_PER_SEC,
			(((double)
			(clock () - starttime) / CLOCKS_PER_SEC) ==
			1) ? "" : "s");
	}
	else if (c_uptime > 3600) {
		L027 (target,
			dbVersion,
			topics,
			c_uptime / 3600,
			c_uptime / 3600 == 1 ? "" : "s",
			(c_uptime / 60) % 60,
			(c_uptime / 60) % 60 ==
			1 ? "" : "s", QUESTIONS,
			ADDITIONS, DELETIONS,
			(double) (clock () -
			starttime) /
			CLOCKS_PER_SEC,
			(((double)
			(clock () - starttime) / CLOCKS_PER_SEC) ==
			1) ? "" : "s");
	}
	else {
		L028 (target,
			dbVersion,
			topics,
			c_uptime / 60,
			c_uptime / 60 == 1 ? "" : "s",
			c_uptime % 60,
			c_uptime % 60 == 1 ? "" : "s",
			QUESTIONS, ADDITIONS, DELETIONS,
			(double) (clock () - starttime) / CLOCKS_PER_SEC, (((double)
			
			(clock
			() -
			starttime)
			/
			CLOCKS_PER_SEC)
			==
			1) ? "" :
		"s");
	}
}

int
check_existing_url (const char *source, char *topic, char *target)
{
	FILE *fp;
	char b[STRING_LONG], *subj;
	if ((fp = fopen (URL2, "r")) == NULL) {
		L003 (source, URL2);
		R 0;
	}
	while (fgets (b, STRING_LONG, fp)) {
		stripline (b);
		subj = strtok (b, " ");
		if (stricmp (subj, topic) == 0) {
			fclose (fp);
			R 1;
		}
	}
	fclose (fp);
	R 0;
}

void
find_url (const char *nick, char *topic, char *target)
{
	FILE *fp;
	long i = 0, FOUND = 0, x = 0;
	char b[STRING_LONG], *subj, *ptr2, DATA[STRING_SHORT] = "";
	if (strlen (topic) > MAX_TOPIC_SIZE)
		topic[MAX_TOPIC_SIZE] = '\0';
	strlwr (topic);
	if ((fp = fopen (URL2, "r")) == NULL) {
		L003 (nick, URL2);
		R;
	}
	while (fgets (b, STRING_LONG, fp)) {
		x++;
		stripline (b);
		subj = strtok (b, " ");
		strlwr (subj);
		ptr2 = strstr (subj, topic);
		if (ptr2 != NULL) {
			i++;
			FOUND = 1;
			sprintf (DATA, "%s %s", DATA, subj);
			if (strlen (DATA) >= MAX_SEARCH_LENGTH)
				break;
		}
	}
	fclose (fp);
	if (FOUND == 0) {
		L021 (target, NO_TOPIC, topic, x);
	}
	else if (i > 19) {
		L022 (target, i, DATA);
	}
	else if (i == 1) {
		L023 (target, nick, DATA);
	}
	else
		L024 (target, i, nick, DATA);
}

void
display_url (char *target, char *nick, char *topic)
{
	FILE *fp;
	long x = 0;
	int index;
	char b[STRING_LONG], *subj, *ptr;
	strlwr (topic);
	if ((fp = fopen (URL2, "r")) == NULL) {
		R;
	}
	while (fgets (b, STRING_LONG, fp)) {
		x++;
		stripline (b);
		subj = strtok (b, " ");
		ptr = strtok (NULL, "");
		if (stricmp (subj, topic) == 0 || !match_wild (subj, topic) == 0) {
			QUESTIONS++;
			S ("PRIVMSG %s :Raw data for %s is: %s\n", target, topic, ptr);
			fclose (fp);
			R;
		}							/* Subject match */
	}
	fclose (fp);
	S
		("PRIVMSG %s :%s, I do not know of any topic named %s\n",
		target, nick, topic);
}

void
delete_url (const char *nick, char *topic, char *target)
{
	FILE *fp;
	long i = 0, FOUND = 0;
	char b[STRING_LONG], *subj, *ptr, DATA[STRING_SHORT] = "";
	if (*topic == '~') {
		topic++;
		if (topic != NULL)
			strlwr (topic);
#ifdef WIN32
		sprintf (DATA,"dat/%s.rdb", topic);
#else
		snprintf (DATA,sizeof(DATA), "dat/%s.rdb", topic);
#endif
		if (strspn (topic, LEGAL_TEXT) != strlen (topic)) {
			S
				("PRIVMSG %s :%s, rdb files are made up of letters and or numbers, no other text is accepted.\n",
				target, nick);
			R;
		}
		
		if ((fp = fopen (DATA, "r")) == NULL) {
			S
				("PRIVMSG %s :%s, %s.rdb does not exist.\n",
				target, nick, topic); R;}
		fclose (fp);
		unlink (DATA);
		S ("PRIVMSG %s :I have unlinked %s.\n", target, DATA);
		R;
	}
	
	if ((fp = fopen (URL2, "r")) == NULL) {
		L003 (nick, URL2);
		R;
	}
	unlink (TMP_URL);
	while (fgets (b, STRING_LONG, fp)) {
		stripline (b);
		subj = strtok (b, " ");
		ptr = strtok (NULL, "");
		i++;
		if (stricmp (subj, topic) == 0) {
			FOUND = 1;
			DELETIONS++;
			L029 (target, nick, i, topic);
		}
		else if (strstr (subj, " ") == NULL)
			fslog (TMP_URL, "%s %s\n", subj, ptr);
	}
	fclose (fp);
	rename (TMP_URL, URL2);
	if (FOUND == 0)
		L030 (target, nick, topic);
}

void chanserv_log_history(char *source, char *target, char *buf);

void
chanserv (char *source, char *target, char *buf)
{
	// 2017 Telegram replies
	substituteTgMessage(source, target, buf);

	// 2018
	chanserv_log_history(source, target, buf);

	// :)
/*
   printf("Finally:\n");
   printf(" # source: %s\n", source);
    printf(" # target: %s\n", target);
    printf(" # buf: %s\n\n", buf);
*/

	int g_pwLen = 3 + nrand(2)*nrand(6) + nrand(2);
	
	int id;
	int level;
	int index;
	char textBuffer10240[10240];
	char bufCopy[10240];
	char textBuffer1024[1024];
	char *cmd, *s, *s2, *s3, *s4, *s5, *ptr3, temp[1024], *userhost;
	long sn2 = 0, sn = 0, i = 0, unixtime = 0;
	
	DATE_INFO di;
	DATE_INFO di1;
	
	struct tm timeStruct;
	time_t localTime;
	
	strncpy(bufCopy, buf, 10239);
	
#ifdef	RANDOM_STUFF
	if (stricmp (target, CHAN) == 0)
		Rand_Idle = 0;
#endif
	stripline (buf);
	stripline (source);
	if (buf == NULL || target == NULL || source == NULL)
		R;
	cmd = strtok (buf, " ");
	if (cmd == NULL)
		R;
	if (*cmd == ':')
		cmd++;
	if ((userhost = strchr (source, '!')) != NULL) {
		*userhost++ = '\0';
	}

// No Chan1
//printf("1: T:%s, S:%s, BC:%s\n", target, source, bufCopy);
//fflush(stdout);

	if (*target == '#')
	{
		// COUNT xyu & chars
		if (bc_update(source, bufCopy+1) == 0)
		{
		    S("NOTICE %s :punishing %s (over %d entries - surcharge %d points)\n", target, source, BC_MAX_ONCE, BC_PUNISH);
		}
		if (bc1_update(source, bufCopy+1) == 0)
		{
		    //S("NOTICE %s :punishing %s (over %d entries - surcharge %d points)\n", target, source, BC_MAX_ONCE, BC_PUNISH);
		}
//printf("2: T:%s, S:%s, BC:%s\n", target, source, bufCopy);

// Telegram echo - 2017
// tgecho.sh

if (!strcmp(target, "#irc") && TG_REPLY)
{
    //S ("PRIVMSG ncuxonycbka :<%s> %s\n", source, bufCopy+1);
    char temp1[10240];
    snprintf (temp1,sizeof(temp1), "/home/olivka/bot/tgecho.sh \"&lt;<b>%s</b>&gt; %s\"", source, bufCopy+1);
    const char *res = run_program(temp1);
}

///

	    if (isNoChan1(target))
	    {
		    // MAT check
		//bc_update(source, bufCopy+1);
//printf("3: T:%s, S:%s, BC:%s\n", target, source, bufCopy);

		  // URL check
		  url_cache uc(UCN);
		  URL uu, uu2;
//		  S ("PRIVMSG йопт :BUF: %s\n", buf);
//		  S ("PRIVMSG йопт :S: %s\n", bufCopy);
		  if (uc.findUrl(bufCopy, &uu))
		  {
		  	if (uc.find(uu.href, &uu2))
		  	{
				// Ignore some :)
//int aaa=stricmp(uu.href, "http://temp.2396.ru/");
//S ("PRIVMSG zloy :S: [%s] [%s] [%d] [%s]\n", bufCopy, uu.href, aaa, uu2.href);
				if (strstr(uu2.href, "http://temp.2396.ru/") != NULL)
				    return;
				if (strstr(uu2.href, "http://newtemp.redip.ru/") != NULL)
				    return;
				// 2017 - no #ttc chan
				if (!strcmp(target, "#ttc"))
				    return;
		  		S ("PRIVMSG %s :%s, это уже cкурили (%s/%s, %s)\n", target, source, uu2.nick, uu2.chan, nocrlf(ctime(&uu2.date)));
		  		db_sleep(5);
		  		return;
		  	}
		  	else
		  	{
		  		uu.nick = source;
		  		uu.chan = target;
		  		uc.add(&uu);
		  		return;
		  	}
		  }
		  
		// NoChan ON/OFF check
		// Enable/disable silent channels
		 if (strstr(bufCopy, "смирно") || strstr(bufCopy, "вольно"))
		 {
		 }
		 else
		 	return;
	    }
	}

	/* ------ commands that require a privmsg ------ */
	if (*target != '#' && *target != '&' && *target != '+') {
		if (stricmp (cmd, "PASS") == 0
			|| stricmp (cmd, "PASSWORD") == 0
			|| stricmp (cmd, "PASSWD") == 0) {
			s = strtok (NULL, " ");
			s2 = strtok (NULL, " ");
			if (s == NULL || s2 == NULL) {
				L031 (source, Mynick);
				R;
			}
			if (strlen (s2) > 25)
				s2[25] = '\0';
			set_pass (source, userhost, s, s2);
			R;
		}
		else if (stricmp (cmd, "RAW") == 0 || stricmp (cmd, "ануб") == 0) {
			if (check_access (userhost, target, 0, source) >= RAW_LEVEL) {
				s = strtok (NULL, "");
				if (s != NULL)
					S ("%s\n", s);
				// S ("PRIVMSG %s :\1ACTION %s\1\n", CHAN, b2);
			}
		}
		else if (stricmp (cmd, "ADDUSER") == 0 || stricmp (cmd, "кросавчег") == 0) {
			if (check_access (userhost, "#*", 0, source) >= ADMIN_LEVEL) {
				s4 = strtok (NULL, " ");
				s = strtok (NULL, " ");
				s2 = strtok (NULL, " ");
				s5 = strtok (NULL, " ");
				if (s == NULL || s4 == NULL || s2 == NULL || s5 == NULL) {
					L055 (source);
					R;
				}
				sn = atoi (s2);
				if (sn > 10 || sn <= 0)
					R;
				if (strlen (s) < 7)
					R;
				L056 (*CMDCHAR);
				add_helper (s4, s, sn, 0, temp, s5);
				L057 (source, s, sn);
				save_changes ();
			}
		}
		
		else if (stricmp (cmd, "DIE") == 0 || stricmp (cmd, "QUIT") == 0) {
			s = strtok (NULL, "");
			level = check_access (userhost, target, 0, source);
			//
			// S ("PRIVMSG %s :1: userhost=%s, target=%s, source=%s, level=%d\n", target, userhost, target, source, level);
			//
			if (level >= ROOT_LEVEL) {
				// S ("PRIVMSG %s :%s, you are \2[%d]\n", source, source, level);
				if (s == NULL) {
					L032 (source);
				}
				else
					Snow ("QUIT :%s\n", s);
				db_sleep (1);
#ifdef	WIN32
				//printf ("\n\nGood-bye! %s (c) Jason Hamilton\n\n", dbVersion);
				uptime = time (NULL) - uptime;
				printf
					("Time elapsed: %d hour%s, %d min%s\n\n",
					uptime / 3600,
					uptime / 3600 == 1 ? "" : "s",
					(uptime / 60) % 60, (uptime / 60) % 60 == 1 ? "" : "s");
				db_sleep (5);
#endif
				exit (0);
			}
			else
				S ("PRIVMSG %s : %s, хрен тебе!\n", target, source);
			
#if CTCP == 1
		}
		else if (stricmp (cmd, "\1VERSION\1") == 0) {
			if (cf (userhost, source, target))
				R;
			if (cf (userhost, source, target))
				R;
			S ("NOTICE %s :\1VERSION %s\1\n", source, VERSION_REPLY);
		}
		else if (stricmp (cmd, "\1PING") == 0) {
			if (cf (userhost, source, target))
				R;
			if (cf (userhost, source, target))
				R;
			s2 = strtok (NULL, "");
			if (s2 != NULL) {
				if (strlen (s2) > 21)
					s2[21] = '\0';
				S ("NOTICE %s :\1PING %s\n", source, s2);
			}
#endif
		}
		else if (stricmp (cmd, "INVITE") == 0)
		{
			S ("PRIVMSG %s :Invite query from %s, \2%s\n", source, userhost, source);
			
			if (strstr(userhost, "uic.tula.ru"))
			{
				S ("NOTICE zloy :Invite query from %s was \2rejected.\n", userhost);
				S ("PRIVMSG %s :8============>\n", source);
				R;
			}
			
			S ("NOTICE zloy :Invite query from %s was accepted.\n", userhost);
			
			level = check_access(userhost, "#party", 0, source);
			if (level < CMD_LEVEL)
			{
				// S ("PRIVMSG %s :%s, you're too lame [%d] to talk on \2#party!\n", source, source, level);
				// R;
			}
			s = strtok (NULL, " ");
			
			if (s == NULL)
			{
				S ("INVITE %s #party\n", source);
				S ("PRIVMSG %s :You have been invited to \2#party\n", source);
			}
			else
			{
				S ("INVITE %s #party\n", s);
				S ("PRIVMSG %s :%s has been invited to \2#party\n", source, s);
			}
		}
		else if (stricmp (cmd, "LOGIN") == 0) {
			s = strtok (NULL, " ");
			if (s == NULL)
				R;
			do_login (source, s);
		}
		R;
	}
	add_user (target, source, userhost, 0); 	  /* Unidle */
	/* ------ Commands that require a CMDCHAR to activate ------ */
	if (*cmd == *CMDCHAR) {
		if (Sleep_Toggle == 1)
			R;
		cmd++;
		if (cf (userhost, source, target))
			R;
		if (stricmp (cmd, "USERLIST") == 0
			|| stricmp (cmd, "HLIST") == 0
			|| stricmp (cmd, "ACCESS") == 0) {
			if (check_access (userhost, target, 0, source) < ROOT_LEVEL)
				R;
			s = strtok (NULL, " ");
			if (s != NULL) {
				show_helper_list (source, atoi (s));
			}
			else
				show_helper_list (source, 0);
		}
		else if (stricmp (cmd, "BANLIST") == 0) {
			if (check_access (userhost, target, 0, source) < ROOT_LEVEL)
				R;
			show_banlist (source);
		}
		else
			if (stricmp (cmd, "LANG") == 0
				|| stricmp (cmd, "LANGUAGE") == 0) {
				S ("PRIVMSG %s :%s, %s\n", target, source, I_SPEAK);
			}
			else if (stricmp (cmd, "CHANINFO") == 0) {
				show_chaninfo (source, target);
			}


        else if (stricmp(cmd, "RU") == 0 || stricmp(cmd, "EN") == 0 || stricmp(cmd, "UA") == 0) {
        s2 = bufCopy+4;
            if (s2 == NULL)
                R;

            wt_trimRight(s2);
                        if (stricmp(cmd, "EN") == 0) {
                            snprintf(temp, sizeof(temp), "/home/olivka/bot/2en.sh  %s\n", s2);
                        } else if (stricmp(cmd, "RU") == 0) {
                            snprintf(temp, sizeof(temp), "/home/olivka/bot/2ru.sh  %s\n", s2);
                        } else {
                            snprintf(temp, sizeof(temp), "/home/olivka/bot/2ua.sh  %s\n", s2);
                        }
                        const char *res = run_program(temp);
                        int ll2to = strlen(res);
                        if ((ll2to > 5)) {
                            S("PRIVMSG %s :%s, %s\n", target, source, res);
                    }


        }

			else if (stricmp (cmd, "IDLE") == 0 || stricmp (cmd, "сплюшка") == 0) {
				s2 = strtok (NULL, " ");
				if (s2 == NULL)
					R;
				if (stricmp (s2, source) == 0) {
					S ("PRIVMSG %s :%s, don't be lame.\n", target, source);
					R;
				}
				unixtime = return_useridle (target, s2, 0);
				if (unixtime == 0) {
					S
						("PRIVMSG %s :%s, I do not see %s in %s.\n",
						target, source, s2, target); return;
				}
				unixtime = time (NULL) - unixtime;
				if (unixtime > 86400)
					S
					("PRIVMSG %s :%s, %s has been idle %d day%s, %02d:%02d\n",
					target, source, s2, unixtime / 86400,
					(unixtime / 86400 == 1) ? "" : "s",
					(unixtime / 3600) % 24, (unixtime / 60) % 60);
				else if (unixtime > 3600)
					S
					("PRIVMSG %s :%s, %s has been idle %d hour%s, %d min%s\n",
					target, source, s2, unixtime / 3600,
					unixtime / 3600 == 1 ? "" : "s",
					(unixtime / 60) % 60,
					(unixtime / 60) % 60 == 1 ? "" : "s");
				else
					S ("PRIVMSG %s :%s, %s has been idle %d minute%s, %d sec%s\n",
					target, source, s2, unixtime / 60,
					unixtime / 60 == 1 ? "" : "s", unixtime % 60,
					unixtime % 60 == 1 ? "" : "s");
			}
			else if (stricmp (cmd, "N") == 0 || stricmp (cmd, "NICK") == 0) {
				if (check_access (userhost, target, 0, source) >= ADMIN_LEVEL) {
					s = strtok (NULL, " ");
					if (s == NULL) {
						L036 (source);
						R;
					}
					strncpy (Mynick, s, sizeof(Mynick));
#ifdef WIN32
					sprintf (NICK_COMMA,"%s,", Mynick);
					sprintf (COLON_NICK,"%s:", Mynick);
					sprintf (BCOLON_NICK,"%s\2:\2", Mynick);
#else
					snprintf (NICK_COMMA,sizeof(NICK_COMMA), "%s,", Mynick);
					snprintf (COLON_NICK,sizeof(COLON_NICK), "%s:", Mynick);
					snprintf (BCOLON_NICK,sizeof(BCOLON_NICK), "%s\2:\2", Mynick);

					snprintf (ANICK_COMMA,sizeof(ANICK_COMMA), "%s,", MyAnick);
					snprintf (COLON_ANICK,sizeof(COLON_ANICK), "%s:", MyAnick);
					snprintf (BCOLON_ANICK,sizeof(BCOLON_ANICK), "%s\2:\2", MyAnick);
#endif
					L037 (source, Mynick);
					S ("NICK %s\n", Mynick);
				}
				else
					L038 (source, source);
			}
			else
				if (stricmp (cmd, "свалить") == 0
					|| stricmp (cmd, "PART") == 0
					|| stricmp (cmd, "LEAVE") == 0 || stricmp (cmd, "P") == 0) {
					if (check_access (userhost, target, 0, source) >= JUMP_LEVEL) {
						s = strtok (NULL, "");
						if (s == NULL) {
							S ("PART %s\n", target);
						}
						else {
							S ("PART %s\n", s);
							L039 (target, s);
						}
					}
					else
						do_randomtopic(target, "fuckoff", source, buf);
						// S ("PRIVMSG %s : %s, обломись\n", target, source);
				}
				else if (stricmp (cmd, "VARIABLES") == 0) {
					S ("PRIVMSG %s :%s, %s\n", target, source, myVariables);
				}
				else if (stricmp (cmd, "JOIN") == 0 || stricmp (cmd, "J") == 0) {
					if (check_access (userhost, target, 0, source) >= USER_LEVEL) {
						s = strtok (NULL, "");
						if (s == NULL) {
							S ("JOIN %s\n", target);
						}
						else {
							S ("JOIN %s\n", s);
							L040 (target, s);
						}
					}
#if DO_CHANBOT_CRAP == 1
				}
				else if (stricmp (cmd, "OP") == 0 || stricmp (cmd, "наградить") == 0) {
					if (check_access (userhost, target, 0, source) >= OP_LEVEL) {
						s = strtok (NULL, "");
						if (s == NULL) {
							S ("PRIVMSG %s :Specify a nick!\n", target);
							return;
						}
						else {
							S ("MODE %s +oooooo %s\n", target, s);
						}
					}
					else
						do_randomtopic(target, "fuckoff", source, buf);
						//S ("PRIVMSG %s : %s, обломись\n", target, source);
				}
				else if (stricmp (cmd, "DEOP") == 0) {
					if (check_access (userhost, target, 0, source) >= OP_LEVEL) {
						s = strtok (NULL, "");
						if (s == NULL) {
							S ("PRIVMSG %s :Specify a nick!\n", target);
							return;
						}
						else {
							S ("MODE %s -oooooo %s\n", target, s);
						}
					}
					else
						do_randomtopic(target, "fuckoff", source, buf);
						//S ("PRIVMSG %s : %s, обломись\n", target, source);
				}
				else if (stricmp (cmd, "DOWN") == 0) {
					if (check_access (userhost, target, 0, source) >= OP_LEVEL)
						S ("MODE %s -o %s\n", target, source);
				}
				else if (stricmp (cmd, "UP") == 0) {
					if (check_access (userhost, target, 0, source) >= OP_LEVEL)
						S ("MODE %s +o %s\n", target, source);
				}
				else
					if (
						(stricmp (cmd, "KICK") == 0
						|| stricmp (cmd, "WACK") == 0
						|| stricmp (cmd, "K") == 0 || stricmp (cmd, "NAIL") == 0)) {
						if (check_access (userhost, target, 0, source) >= KICK_LEVEL) {
							s = strtok (NULL, " ");
							if (s == NULL) {
								S ("PRIVMSG %s :Specify a nick/chan!\n", target);
								return;
							}
							else {
								if (*s != '#' && *s != '&') {
									s2 = strtok (NULL, "");
									if (s2 == NULL) {
										if (stricmp (s, Mynick) == 0) {
											S ("KICK %s %s :hah! As *IF*\n", target, source);
										}
										else
										{
											char kickMsg[1024] = "suck";
											get_randomtopic(target, "kick_psalter", source, target, kickMsg);
											S ("KICK %s %s :\2%s\2'ed: %s\n", target, s, cmd, kickMsg);}
											// S ("KICK %s %s :\2%s\2'ed: %s\n", target, s, cmd, DEFAULT_KICK);}
										}
									else if (stricmp (s, Mynick) == 0) {
										S ("KICK %s %s :%s\n", target, s, s2);
									}
									else
										S ("KICK %s %s :\2%s\2'ed: %s\n", target, s, cmd, s2);
								}
								else {
									s2 = strtok (NULL, " ");
									if (s2 == NULL) {
										S
											("NOTICE %s :You must specify a nick to kick from %s!\n",
											source, s);
									}
									else {
										s3 = strtok (NULL, "");
										if (s3 == NULL) {
											if (stricmp (s2, Mynick) == 0) {
												S ("KICK %s %s :hah! As *IF*\n", s, source);
											}
											else
												S ("KICK %s %s :\2%s\2ed: %s\n", s, s2,
												cmd, DEFAULT_KICK);}
										else {
											
											if (stricmp (s2, Mynick) == 0) {
												S ("KICK %s %s :hah! As *IF* (%s)\n", s, source);
											}
											else
												S ("KICK %s %s :\2%s\2ed: %s\n", s, s2, cmd, s3);
										}
									}
								}
							}
						}
						else
							do_randomtopic(target, "fuckoff", source, buf);
							//S ("PRIVMSG %s : %s, обломись\n", target, source);
#endif
					}
					else if (stricmp (cmd, "CYC") == 0 || stricmp (cmd, "CYCLE") == 0) {
						if (check_access (userhost, target, 0, source) >= JUMP_LEVEL) {
							s = strtok (NULL, "");
							if (s == NULL) {
								S ("PART %s\n", target);
								S ("JOIN %s\n", target);
							}
							else {
								S ("PART %s\n", s);
								S ("JOIN %s\n", s);
								S ("PRIVMSG %s :Cycling %s\n", target, s);
							}
						}
					}
					else if (stricmp (cmd, "DIE") == 0 || stricmp (cmd, "QUIT") == 0) {
						s = strtok (NULL, "");
						level = check_access (userhost, target, 0, source);
						//
						// S ("PRIVMSG %s :2: userhost=%s, target=%s, source=%s, level=%d\n", target, userhost, target, source, level);
						//
						if (level >= DIE_LEVEL) {
							if (s == NULL) {
								L032 (source);
							}
							else
								Snow ("QUIT :%s\n", s);
							db_sleep (1);
#ifdef	WIN32
							printf ("\n\nGood-bye! %s (c) Jason Hamilton\n\n", dbVersion);
							uptime = time (NULL) - uptime;
							printf
								("Time elapsed: %d hour%s, %d min%s\n\n",
								uptime / 3600,
								uptime / 3600 == 1 ? "" : "s",
								(uptime / 60) % 60, (uptime / 60) % 60 == 1 ? "" : "s");
							db_sleep (5);
#endif
							exit (0);
						}
						else
							do_randomtopic(target, "fuckoff", source, buf);
							//do_randomtopic(target, DONNO_RDB, source, buf);
							// s ("PRIVMSG %s : %s, хрен тебе!\n", target, source);
#if DO_CHANBOT_CRAP == 1
					}
					else
						if (stricmp (cmd, "DEV") == 0
							|| stricmp (cmd, "DV") == 0
							|| stricmp (cmd, "DEVOICE") == 0
							|| stricmp (cmd, "DVOICE") == 0) {
							if (check_access (userhost, target, 0, source) >= VOICE_LEVEL) {
								s = strtok (NULL, "");
								if (s == NULL) {
									L041 (target);
									R;
								}
								else
									S ("MODE %s -vvvvvvv %s\n", target, s);
							}
						}
						else if (stricmp (cmd, "VOICE") == 0 || stricmp (cmd, "V") == 0) {
							if (check_access (userhost, target, 0, source) >= VOICE_LEVEL) {
								s = strtok (NULL, "");
								if (s == NULL) {
									L041 (target);
									R;
								}
								else
									S ("MODE %s +vvvvvvv %s\n", target, s);
							}
						}
						else if (stricmp (cmd, "T") == 0 || stricmp (cmd, "TOPIC") == 0) {
							if (check_access (userhost, target, 0, source) >= TOPIC_LEVEL) {
								s = strtok (NULL, "");
								if (s == NULL) {
									S ("TOPIC %s :\n", target);
									R;
								}
								else {
									S ("TOPIC %s :%s\n", target, s);
								}
							}
#endif
						}
						else if (stricmp (cmd, "JUMP") == 0
							|| stricmp (cmd, "SERVER") == 0) {
							if (check_access (userhost, target, 0, source) >= JUMP_LEVEL) {
								s = strtok (NULL, " ");
								if (s == NULL) {
									S ("NOTICE %s :Syntax: JUMP <server> [port]\n", source);
									R;
								}
								s2 = strtok (NULL, " ");
								if (s2 == NULL) {
									sn = 6667;
								}
								else
									sn = atoi (s2);
								S ("QUIT :Jumping to %s:%d\n", s, sn);
								db_sleep (1);
								socketfd = get_connection (s, VHOST, sn);
								init_bot ();
							}
#if DO_CHANBOT_CRAP == 1
						}
						else if (stricmp (cmd, "DELBAN") == 0) {
							if (check_access (userhost, target, 0, source) >= 3) {
								s = strtok (NULL, " ");
								if (s == NULL) {
									L042 (source);
									R;
								}
								if (del_permban (source, s) == 1)
									S ("MODE %s -b %s\n", target, s);
								else
									L043 (source);
							}
#endif
						}
						else if (stricmp (cmd, "DELUSER") == 0) {
							if (check_access (userhost, target, 0, source) >= 3) {
								s = strtok (NULL, " ");
								if (s == NULL) {
									L044 (source);
									R;
								}
								delete_user_ram (source, s);
							}
#if DO_CHANBOT_CRAP == 1
						}
						else if (stricmp (cmd, "TEASEOP") == 0
							|| stricmp (cmd, "TO") == 0) {
							if (check_access (userhost, target, 0, source) >= 2) {
								s = strtok (NULL, " ");
								if (s == NULL) {
									L036 (target);
									R;
								}
								if (stricmp (s, Mynick) == 0) {
									L045 (source);
								}
								else
									S
									("MODE %s +o-o+o-o+o-o %s %s %s %s %s %s\n",
									target, s, s, s, s, s, s);
							}
#endif
#ifndef WIN32
						}
						else if (stricmp (cmd, "BACKUP") == 0) {
							if (check_access (userhost, target, 0, source) >= 3) {
#ifdef WIN32
								sprintf (temp,"/bin/cp -rf %s \"%s.bak @ `date`\"\n",
#else
									snprintf (temp,sizeof(temp),
									"/bin/cp -rf %s \"%s.bak @ `date`\"\n",
#endif
									URL2, URL2); system (temp);
								L046 (target);
							}
#endif
						}
						
						/*
						#ifdef	ENABLE_VERA
						else if (stricmp (cmd, "DICT") == 0 || stricmp (cmd, "VERA") == 0)
						{
						s = strtok (NULL, "");
						
						  if (s == NULL)
						  R;
						  
							if (strlen (s) > 200)
							s[200] = '\0';
							
							  dict(target, source, s, "vera", false);
							  }
							  #endif
						*/
						
						else if ((stricmp (cmd, "AUTOTOPIC") == 0) ||
							(stricmp (cmd, "АВТОТОПЕК") == 0) ||
							(stricmp (cmd, "автотопек") == 0)) {
							if (check_access (userhost, target, 0, source) >= ADMIN_LEVEL) {
								s = strtok (NULL, "");
								if (s == NULL) {
									L047 (source, *CMDCHAR);
									R;
								}
								set_autotopic (source, target, s);
							}
						}
						else if (stricmp (cmd, "SETCHAN") == 0) {
							if (check_access (userhost, target, 0, source) >= ROOT_LEVEL) {
								s = strtok (NULL, " ");
								if (s == NULL) {
									L048 (source);
									R;
								}
								strncpy (CHAN, s, sizeof(CHAN));
								L049 (source, CHAN);
								save_setup ();
							}
						}
						else if (stricmp (cmd, "SETCHAR") == 0) {
							if (check_access (userhost, target, 0, source) >= ROOT_LEVEL) {
								s = strtok (NULL, " ");
								if (s == NULL) {
									L050 (source);
									R;
								}
								*CMDCHAR = *s;
								L051 (source, *CMDCHAR);
								save_setup ();
							}
						}
						else if (stricmp (cmd, "SETUSER") == 0) {
							if (check_access (userhost, target, 0, source) >= ROOT_LEVEL) {
								s = strtok (NULL, " ");
								if (s == NULL) {
									L052 (source);
									R;
								}
								strncpy (UID, s, sizeof(UID));
								L053 (source, UID);
								save_setup ();
							}
						}
						else if (stricmp (cmd, "VHOST") == 0) {
							if (check_access (userhost, target, 0, source) >= ROOT_LEVEL) {
								s = strtok (NULL, " ");
								if (s == NULL) {
									L091 (source);
									R;
								}
								strncpy (VHOST, s, sizeof(VHOST));
								L092 (source, VHOST);
								save_setup ();
							}
						}
						else if (stricmp (cmd, "SETNICK") == 0) {
							if (check_access (userhost, target, 0, source) >= ROOT_LEVEL) {
								s = strtok (NULL, " ");
								if (s == NULL) {
									L054 (source);
									R;
								}
								S ("NICK %s\n", s);
								strncpy (s_Mynick, s, sizeof(s_Mynick));
								strncpy (Mynick, s, sizeof(Mynick));
#ifdef WIN32
								sprintf (NICK_COMMA,"%s,", Mynick);
								sprintf (COLON_NICK,"%s:", Mynick);
								sprintf (BCOLON_NICK,"%s\2:\2", Mynick);
#else
								snprintf (NICK_COMMA,sizeof(NICK_COMMA), "%s,", Mynick);
								snprintf (COLON_NICK,sizeof(COLON_NICK), "%s:", Mynick);
								snprintf (BCOLON_NICK,sizeof(BCOLON_NICK), "%s\2:\2", Mynick);

								snprintf (ANICK_COMMA,sizeof(ANICK_COMMA), "%s,", MyAnick);
								snprintf (COLON_ANICK,sizeof(COLON_ANICK), "%s:", MyAnick);
								snprintf (BCOLON_ANICK,sizeof(BCOLON_ANICK), "%s\2:\2", MyAnick);
#endif
								save_setup ();
							}
						}
						else if (stricmp (cmd, "RAW") == 0) {
							if (check_access (userhost, target, 0, source) >= ROOT_LEVEL) {
								s = strtok (NULL, "");
								if (s != NULL)
									S ("%s\n", s);
							}
						}
						else if (stricmp (cmd, "SEEN") == 0 && SeeN == 1) {
							s = strtok (NULL, " ");
							if (s == NULL) {
								count_seen (source, target);
								R;
							}
							if (return_useridle (target, s, 1) == 1) {
								S ("PRIVMSG %s :%s is right here in the channel!\n", target,
									s);
								R;
							}
							show_seen (s, source, target);
#if STATUS == 1
						}
						else if (stricmp (cmd, "LUSERS") == 0) {
							if (check_access (userhost, target, 0, source) >= ADMIN_LEVEL)
								S ("LUSERS\n");
#endif
						}
						else if (stricmp (cmd, "ADDUSER") == 0 || stricmp (cmd, "кросавчег") == 0) {
							if (check_access (userhost, target, 0, source) >= ADMIN_LEVEL) {
								s4 = strtok (NULL, " ");
								s = strtok (NULL, " ");
								s2 = strtok (NULL, " ");
								s5 = strtok (NULL, " ");
								if (s == NULL || s4 == NULL || s2 == NULL || s5 == NULL) {
									L055 (source);
									R;
								}
								sn = atoi (s2);
								if (sn > 10 || sn <= 0)
									R;
								if (strlen (s) < 7)
									R;
								L056 (*CMDCHAR);
								add_helper (s4, s, sn, 0, temp, s5);
								L057 (source, s, sn);
								save_changes ();
							}
#if DO_CHANBOT_CRAP == 1
						}
						else
							if (stricmp (cmd, "PERMBAN") == 0
								|| stricmp (cmd, "SHITLIST") == 0) {
								if (check_access (userhost, target, 0, source) >= ROOT_LEVEL) {
									s = strtok (NULL, " ");
									if (s == NULL) {
										L058 (source, *CMDCHAR, cmd);
										R;
									}
									s2 = strtok (NULL, "");
									if (s2 == NULL)
										s2 = "Permbanned!";
									add_permban (s, 0, s2);
									L059 (source, PERMBAN_counter, s, s2);
									save_permbans ();
									S ("MODE %s +b %s\n", target, s);
								}
#endif
							}
							else
								if (stricmp (cmd, "ALARM") == 0
									|| stricmp (cmd, "ALARMCLOCK") == 0) {
									if (check_access (userhost, target, 0, source) >= ROOT_LEVEL) {
										s = strtok (NULL, " ");
										s2 = strtok (NULL, "");
										if (s == NULL || s2 == NULL) {
											S
												("NOTICE %s :Syntax: <time type: d/h/m><time> <text to say>\n",
												source); R;
										}
										if (strlen (s) < 2) {
											S
												("NOTICE %s :Syntax: <time type: d/h/m><time> <text to say>\n",
												source); R;
										}
										if (*s == 'd') {
											sn = 86400;
											s++;
										}
										else if (*s == 'h') {
											sn = 3600;
											s++;
										}
										else if (*s == 'm') {
											sn = 60;
											s++;
										}
										else {
											S
												("NOTICE %s :Syntax: <time type: \2d/h/m\2><time> <text to say>\n",
												source); R;
										}
										if (strspn (s, NUMBER_LIST) != strlen (s)) {
											S ("NOTICE %s :Time must be a number.\n", source);
											R;
										}
										i = (atoi (s) * sn) + time (NULL);
#ifdef WIN32
										sprintf (temp,"%s/%d", DBTIMERS_PATH, (int) i);
#else
										snprintf (temp,sizeof(temp), "%s/%d", DBTIMERS_PATH, (int) i);
#endif
										fslog (temp,
											"PRIVMSG %s :\2ALARMCLOCK\2 by %s!%s: %s\n",
											target, source, userhost, s2);
										unixtime = atoi (s) * sn;
										if (unixtime > 86400)
											S
											("PRIVMSG %s :%s, alarmclock set to go off in %d day%s, %02d:%02d\n",
											target, source, unixtime / 86400,
											(unixtime / 86400 == 1) ? "" : "s",
											(unixtime / 3600) % 24, (unixtime / 60) % 60);
										else if (unixtime > 3600)
											S
											("PRIVMSG %s :%s, alarmclock set to go off in %d hour%s, %d min%s\n",
											target, source, unixtime / 3600,
											unixtime / 3600 == 1 ? "" : "s",
											(unixtime / 60) % 60,
											(unixtime / 60) % 60 == 1 ? "" : "s");
										else
											S
											("PRIVMSG %s :%s, alarmclock set to go off in %d minute%s, %d sec%s\n",
											target, source, unixtime / 60,
											unixtime / 60 == 1 ? "" : "s",
											unixtime % 60, unixtime % 60 == 1 ? "" : "s");
									}
								}
								else
									if (stricmp (cmd, "REPEAT") == 0
										|| stricmp (cmd, "TIMER") == 0) {
										if (check_access (userhost, target, 0, source) >= USER_LEVEL) {
											s = strtok (NULL, " ");
											s2 = strtok (NULL, " ");
											s3 = strtok (NULL, "");
											if (s == NULL || s2 == NULL || s3 == NULL) {
												L060 (source);
												R;
											}
											sn = atoi (s);
											sn2 = atoi (s2);
											while (sn > 0) {
												S ("%s\n", s3);
												sn--;
												db_sleep (sn2);
											}
										}
#ifndef WIN32
									}
									else
										if (stricmp (cmd, "REHASH") == 0
											|| stricmp (cmd, "RESTART") == 0 
											|| stricmp (cmd, "ануб") == 0) {
											if (check_access (userhost, target, 0, source) >= ROOT_LEVEL) {
												L062 (dbVersion);
#ifdef WIN32
												sprintf (temp,"sleep 2; %s", DARKBOT_BIN);
#else
												snprintf (temp,sizeof(temp), "sleep 2; %s", DARKBOT_BIN);
#endif
												system (temp);
												db_sleep (1);
												exit (0);
											}
											else
											    do_randomtopic(target, DONNO_RDB, source, buf);
#endif
										}
										else if (stricmp (cmd, "PING") == 0) {
											if (check_access (userhost, target, 0, source) == 0) {
												S ("NOTICE %s PONG!\n", source);
											}
											else
												S ("PRIVMSG %s :PONG!\n", target);
										}
										else if (stricmp (cmd, "HELP") == 0) {
											L100 (source, NICK_COMMA, COLON_NICK,
												BCOLON_NICK, Mynick, NICK_COMMA, NICK_COMMA);
											db_sleep (3);
											if (cf (userhost, source, target))
												R;
											L101 (source, NICK_COMMA, NICK_COMMA, NICK_COMMA);
											db_sleep (2);
										}
										else if (stricmp (cmd, "SETINFO") == 0) {
											if (check_access (userhost, target, 0, source) >= 1) {
												s = strtok (NULL, "");
												if (s == NULL) {
													S ("NOTICE %s :%s\n", source, mySetinfo);
													R;
												}
												update_setinfo (userhost, s, source);
												save_changes ();
											}
										}
#if CTCP == 1
  }
  else if (stricmp (cmd, "\1VERSION\1") == 0) { /* these are #chan
	  * ctcp's */
	  if (cf (userhost, source, target))
		  R;
	  if (cf (userhost, source, target))
		  R;
	  S ("NOTICE %s :\1VERSION %s\1\n", source, VERSION_REPLY);
  }
  else if (stricmp (cmd, "\1PING") == 0) {
	  if (cf (userhost, source, target))
		  R;
	  if (cf (userhost, source, target))
		  R;
	  s2 = strtok (NULL, "");
	  if (s2 != NULL) {
		  if (strlen (s2) > 21)
			  s2[21] = '\0';
		  S ("NOTICE %s :\1PING %s\n", source, s2);
	  }
#endif
  }
  else if (stricmp (cmd, "\2\2DARKBOT") == 0) {
	  if (Sleep_Toggle == 1)
		  R;
	  if (cf (userhost, source, target))
		  R;
	  S
		  ("PRIVMSG %s :%s reporting! My cmdchar is %c\n",
		  target, dbVersion, *CMDCHAR);}
  else
	  if (stricmp (cmd, NICK_COMMA) == 0
		  || stricmp (cmd, COLON_NICK) == 0
		  || stricmp (cmd, BCOLON_NICK) == 0
		  || stricmp (cmd, ANICK_COMMA) == 0
		  || stricmp (cmd, COLON_ANICK) == 0
		  || stricmp (cmd, BCOLON_ANICK) == 0
		  || stricmp (cmd, Mynick) == 0
		  || stricmp (cmd, MyAnick) == 0) {
		  s = strtok (NULL, " ");
		  if (s != NULL) {
			  if (stricmp (s, "WAKEUP") == 0 || stricmp (s, "проснись") == 0) {
				  if (Sleep_Toggle == 0)
					  R;
				  // 28/10/2006
				  if (true) { //check_access (userhost, target, 0, source) >= SLEEP_LEVEL) {
				  
					  Sleep_Toggle = 0;
					  AIL4 = 0;
					  S ("PRIVMSG %s :%s\n", target, WAKEUP_ACTION);
					  if (stricmp (sleep_chan, target) != 0)
						  S ("PRIVMSG %s :%s\n", sleep_chan, WAKEUP_ACTION);
					  R;
				  }
			  }
		  }
		  if (Sleep_Toggle == 1)
			  R;
		  if (cf (userhost, source, target))
			  R;
		  if (s != NULL) {
#ifdef	RANDOM_STUFF
			  if (stricmp (s, "RANDOMSTUFF") == 0
				  || stricmp (s, "RANDSTUFF") == 0) {
				  if (check_access (userhost, target, 0, source) >= RAND_LEVEL) {
					  s2 = strtok (NULL, "");
					  if (s2 == NULL) {
						  L064 (target, source);
						  R;
					  }
					  add_randomstuff (source, target, s2);
				  }
			  }
			  else
#endif
				  // <zloy>
#ifdef	PARSE_NICKNAMES
				  if (stricmp (s, "NICKNAMES") == 0)
				  {
					  if (check_access (userhost, target, 0, source) >= ADMIN_LEVEL)
					  {
						  sprintf(textBuffer10240, "men (%d): ", num_male_nicks);
						  
						  for (index = 0; index < num_male_nicks; index++)
						  {
							  strcat(textBuffer10240, male_nicks[index]);
							  if (index != num_male_nicks-1)
								  strcat(textBuffer10240, ", ");
						  }
						  
						  S("PRIVMSG %s :%s\n", target, textBuffer10240);
						  
						  sprintf(textBuffer10240, "women (%d): ", num_female_nicks);
						  
						  for (index = 0; index < num_female_nicks; index++)
						  {
							  strcat(textBuffer10240, female_nicks[index]);
							  if (index != num_female_nicks-1)
								  strcat(textBuffer10240, ", ");
						  }
						  
						  S("PRIVMSG %s :%s\n", target, textBuffer10240);
					  }
				  }
				  else
#endif
					  // </zloy>
					  if (stricmp (s, "ADD") == 0
						  || stricmp (s, "REMEMBER") == 0
						  || stricmp (s, "SAVE") == 0 || stricmp (s, "STORE") == 0) {
#ifdef	REQ_ACCESS_ADD
						  if (check_access (userhost, target, 0, source) >= ADMIN_LEVEL) {
#endif
							  s2 = strtok (NULL, " ");
							  if (s2 == NULL) {
								  L065 (target, source);
								  R;
							  }
							  if (strlen (s2) > MAX_TOPIC_SIZE) {
								  s2[MAX_TOPIC_SIZE] = '\0';
								  S
									  ("PRIVMSG %s :%s, topic is over the limit, and has characters truncated.\n",
									  target, source);
							  }
							  s3 = strtok (NULL, "");
							  if (s3 == NULL) {
								  L066 (target, source, s2);
								  R;
							  }
							  if (strlen (s3) > MAX_DATA_SIZE)
								  s3[MAX_DATA_SIZE] = '\0';
							  strlwr (s2);
							  if (*s2 == '~') {
								  S
									  ("PRIVMSG %s :%s, rdb files can only be called from the data of a topic, they cannot be used in the topic itself.\n",
									  target, source);
								  R;
							  }
							  if (check_existing_url (source, s2, target) == 1) {
								  S ("PRIVMSG %s :%s \37%s\37\n", target, EXISTING_ENTRY,
									  s2);
								  R;
							  }
#ifdef	LOG_ADD_DELETES
							  fslog (ADD_DELETES,
								  "[%s] %s!%s ADD %s %s\n", date (), source, userhost,
								  s2, s3);
#endif
							  ADDITIONS++;
							  if (s2[0] == 'i' && s2[1] == 'l' && s2[2] == 'c') {
								  fslog (URL2, "%s ([%s] %s!%s): %s\n", s2,
									  date (), source, userhost, s3);
							  }
							  else
								  fslog (URL2, "%s %s\n", s2, s3);
							  L067 (target, source);
#ifdef	REQ_ACCESS_ADD
						  }
#endif
					  }
					  else if (stricmp (s, "DATE") == 0 || stricmp (s, "TIME") == 0) {
						  S ("PRIVMSG %s :%s, %s\n", target, source, date ());
					  }
					  
					  else if (stricmp (s, "ADDDATE") == 0)
					  {
						  s2 = strtok (NULL, "");
						  
						  if (s2 == NULL)
						  {
							  do_randomtopic (target, DONNO_RDB, source, buf);
							  R;
						  }
						  else
						  {
							  if (addDateRecordFromQuery(s2, &id, &di, source))
							  {
								  if (di.type == MATCH_MONTH)
									  sprintf(textBuffer1024, "%d/%d/%d",  di.day, di.month, di.year);
								  else
									  getDayName(di.weekDay, textBuffer1024);
								  
								  S ("PRIVMSG %s :%s, added [\2%d\2] %s.\n", target, source, id, textBuffer1024);
							  }
							  else
								  S ("PRIVMSG %s :%s, invalid command format.\n", target, source);
						  }
					  }
					  
					  else if (stricmp (s, "SETDATE") == 0)
					  {
						  if (check_access (userhost, target, 0, source) < ADMIN_LEVEL)
							  R;

						  s2 = strtok (NULL, "");
						  
						  if (s2 == NULL)
						  {
							  do_randomtopic (target, DONNO_RDB, source, buf);
							  R;
						  }
						  else
						  {
							  if (updateDateRecordFromQuery(s2, &id, source))
							  {
								/*
								  if (di.type == MATCH_MONTH)
									  sprintf(textBuffer1024, "%d/%d/%d",  di.day, di.month, di.year);
								  else
									  getDayName(di.weekDay, textBuffer1024);
								*/
								S ("PRIVMSG %s :%s, updated [\2%d\2]\n", target, source, id);
							  }
							  else
								  S ("PRIVMSG %s :%s, invalid date.\n", target, source);
						  }
					  }

					  else if (stricmp (s, "DELDATE") == 0)
					  {
						  if (check_access (userhost, target, 0, source) < ADMIN_LEVEL)
							  R;
						  
						  s2 = strtok (NULL, " ");
						  
						  if (s2 == NULL)
						  {
							  do_randomtopic (target, DONNO_RDB, source, buf);
							  R;
						  }
						  else
						  {
							  id = atoi(s2);
							  
							  if (delDateRecord(id))
								  S ("PRIVMSG %s :%s, done.\n", target, source, id);
							  else
								  S ("PRIVMSG %s :%s, invalid record ID.\n", target, source, id);
						  }
					  }
					  
					  else if (stricmp (s, "SEARCHDATE") == 0 || stricmp (s, "празднеки") == 0)
					  {
						  if (check_access (userhost, target, 0, source) < ADMIN_LEVEL)
							  R;
						  
						  s2 = strtok (NULL, " ");
						  
						  if (s2 == NULL)
						  {
							  do_randomtopic (target, DONNO_RDB, source, buf);
							  R;
						  }
						  else
						  {
							  if (parseDate(s2, &di))
							  {
								  localTime = time(NULL);
								  timeStruct = *localtime(&localTime);
								  
								  timeStruct.tm_mday = di.day;
								  timeStruct.tm_mon = di.month;
								  // :)
								  if (timeStruct.tm_mon > 0) timeStruct.tm_mon--;
								  timeStruct.tm_year = di.year-1900;
								  
								  localTime = mktime(&timeStruct);
								  
								  showDateDirect(target, source, localTime);
							  }
							  else
								  S ("PRIVMSG %s :%s, invalid date format.\n", target, source);
						  }
					  }
					  
					  else if (stricmp(s, "ОБОСРИ") == 0 || stricmp(s, "ЗАЖОПЬ") == 0 ||
						  stricmp(s, "обосри") == 0 || stricmp(s, "зажопь") == 0 ||
						  stricmp(s, "обругай") == 0 || stricmp(s, "ОБРУГАЙ") == 0)
					  {
						  s2 = strtok (NULL, "");
						  
						  if (!inv_vars[INTV_ASSHOLE])
							  R;
						  
						  if (s2 == NULL)
						  {
							  do_randomtopic (target, DONNO_RDB, source, buf);
							  R;
						  }
						  else
						  {
							  if (assholize(s2, textBuffer10240))
								  S ("PRIVMSG %s :%s:  %s\n", target, source, textBuffer10240);
							  else
								  do_randomtopic (target, DONNO_RDB, source, buf);
						  }
					  }
					  
					  else if (stricmp(s, "РАСКРАСЬ") == 0 || stricmp(s, "раскрась") == 0 ||
						  stricmp(s, "ПОКРАСЬ") == 0 || stricmp(s, "покрась") == 0)
					  {
						  s2 = strtok (NULL, "");
						  
						  if (s2 == NULL || strlen(s2) > 128)
						  {
							  do_randomtopic (target, DONNO_RDB, source, buf);
							  R;
						  }
						  else
						  {
							  strcpy(textBuffer10240, s2);
							  color(textBuffer10240);
							  S ("PRIVMSG %s :%s:  %s\n", target, source, textBuffer10240);
						  }
					  }
					  
					  else if (stricmp(s, "засобачь") == 0 || stricmp(s, "ЗАСОБАЧЬ") == 0 ||
						  stricmp(s, "засоб@чь") == 0 || stricmp(s, "ЗАСОБ@ЧЬ") == 0)
					  {
						  s2 = strtok (NULL, "");
						  
						  if (s2 == NULL)
						  {
							  do_randomtopic (target, DONNO_RDB, source, buf);
							  R;
						  }
						  else
						  {
							  if (puppylize(s2))
								  S ("PRIVMSG %s :%s:  %s\n", target, source, s2);
							  else
								  do_randomtopic (target, DONNO_RDB, source, buf);
						  }
					  }
					  
					  else if (stricmp (s, "REPLACE") == 0) {
#ifdef	REQ_ACCESS_ADD
						  if (check_access (userhost, target, 0, source) >= 1) {
#endif
							  s2 = strtok (NULL, " ");
							  if (s2 == NULL) {
								  L068 (target, source);
								  R;
							  }
							  if (strlen (s2) > MAX_TOPIC_SIZE)
								  s2[MAX_TOPIC_SIZE] = '\0';
							  s3 = strtok (NULL, "");
							  if (s3 == NULL) {
								  L069 (target, source, s2);
								  R;
							  }
							  if (strlen (s3) > MAX_DATA_SIZE)
								  s3[MAX_DATA_SIZE] = '\0';
							  strlwr (s2);
							  if (check_existing_url (source, s2, target) != 1) {
								  S ("PRIVMSG %s :%s \37%s\37\n", target, NO_ENTRY, s2);
								  R;
							  }
							  delete_url (source, s2, target);
#ifdef	LOG_ADD_DELETES
							  fslog (ADD_DELETES,
								  "[%s] %s!%s REPLACE %s %s\n",
								  date (), source, userhost, s2, s3);
#endif
							  ADDITIONS++;
							  fslog (URL2, "%s %s\n", s2, s3);
							  L070 (target, source, s2);
#ifdef	REQ_ACCESS_ADD
						  }
#endif
#if DO_CHANBOT_CRAP == 1
					  }
					  else if (stricmp (s, "PERMBANS?") == 0) {
						  L071 (target,
							  (PERMBAN_counter ==
							  1) ? "is" : "are",
							  PERMBAN_counter, (PERMBAN_counter == 1) ? "" : "s");
#endif
#ifdef	RANDOM_STUFF
					  }
					  else
						  if (stricmp (s, "RANDOMSTUFF?") == 0
							  || stricmp (s, "RANDSTUFF?") == 0) {
							  L073 (target, source, Rand_Stuff);
#endif
						  }
						 
						  else if (stricmp (s, "STRLENGTH") == 0) {
							  s2 = strtok (NULL, "");
							  if (s2 == NULL)
								  R;
							  L074 (target, source, strlen (s2));
						  }
						  else if (stricmp (s, "CHAR") == 0) {
							  s2 = strtok (NULL, " ");
							  if (s2 == NULL)
								  R;
							  S ("PRIVMSG %s :%s, %c -> %d\n", target, source, s2[0],
								  s2[0]);
						  }
						  else if (stricmp (s, "SEEN") == 0 && SeeN == 1) {
							  s2 = strtok (NULL, " ");
							  if (s2 == NULL)
								  R;
							  show_seen (s2, source, target);
						  }
						  else if (stricmp (s, "SENDQ?") == 0 || stricmp (s, "QUE?") == 0) {
							  L075 (target, source,
								  get_sendq_count (2),
								  (get_sendq_count (2) == 1) ? "" : "s");
						  }
						  else if (stricmp (s, "JOINS?") == 0) {
							  L076 (target, JOINs);
						  }
						  else if (stricmp (s, "LOCATION?") == 0) {
							  L077 (target, (snr == 1) ? "is" : "are",
								  snr, (snr == 1) ? "" : "s", spr);
						  }
						  else if (stricmp (s, "CMDCHAR?") == 0) {
							  L078 (target, source, *CMDCHAR);
						  }
						  else
							  if (stricmp (s, "DATASEARCH") == 0
								  || stricmp (s, "DSEARCH") == 0 ||
								  stricmp (s, "DFIND") == 0) {
								  s2 = strtok (NULL, "");
								  if (s2 == NULL) {
									  L079 (target, s, source);
									  R;
								  }
								  datasearch (source, s2, target);
							  }
							  else
								  if (stricmp (s, "SEARCH") == 0
									  || stricmp (s, "поищи") == 0 || stricmp (s, "найди") == 0) {
									  s2 = strtok (NULL, " ");
									  if (s2 == NULL) {
										  if (stricmp (s, "FIND") == 0) {
											  S ("PRIVMSG %s :%s, %s?\n", target, TRY_FIND, source);
										  }
										  else
											  L079 (target, s, source);
										  R;
									  }
									  
									  // ???
									  {
										  int userLevel = check_access (userhost, target, 0, source);
										  
										  if (userLevel >= 5)
											  find_url (source, s2, target);
										  else
										  {
											  S ("PRIVMSG %s :%s, сам ищи!\n", target, source);
											  // S ("PRIVMSG %s : %LID: [%d]. Higher level required.\n", source, userLevel);
										  }
									  }
									  
								  }
								  else if (stricmp (s, "INFO2") == 0) {
									  show_info2 (target, source);
								  }
								  else if (stricmp (s, "INFO") == 0) {
									  info (source, target);
								  }
								  else if (stricmp (s, "гудок") == 0 || strcmp(s, "глоток") == 0) {
									  t_glotokInfo(target, source);
								  }
								  else if (stricmp (s, "ковток") == 0) {
									  t_glotokInfoUa(target, source);
								  }
								  else if (stricmp (s, "топек") == 0 || strcmp(s, "топик") == 0) {
									  t_topekInfo(target, source);
								  }
								  else if (stricmp (s, "топег") == 0 || stricmp (s, "топег?") == 0) {
									  t_topegInfo(target, source);
								  }
								  
#ifdef	DO_MATH_STUFF
								  else if (stricmp (s, "CALC") == 0 || stricmp (s, "MATH") == 0) {
									  s2 = strtok (NULL, "");
									  if (s2 == NULL)
										  R;
									  if (strlen (s2) > 200)
										  s2[200] = '\0';
									  do_math (source, target, s2);
								  }
#endif
								  
#ifdef	ENABLE_VERA
								  else if (stricmp (s, "DICT") == 0 || stricmp (s, "VERA") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 100)
										  s2[100] = '\0';
									  
									  dict(target, source, s2, "vera", false);
								  }
#endif
								  else if (stricmp (s, "НУЧТО") == 0 || stricmp (s, "празднек") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 100)
										  s2[100] = '\0';
									  
									  showDateByIndex(target, source, s2);
								  }
								  
#ifdef	ENABLE_LATIN
								  else if (stricmp (s, "LAT") == 0 || stricmp (s, "LATINA") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 100)
										  s2[100] = '\0';
									  
									  dictLatin(source, target, s2);
								  }

								  else if (stricmp (s, "LAT2") == 0 || stricmp (s, "LATINA2") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 100)
										  s2[100] = '\0';
									  
									  dictLatinList(source, target, s2);
								  }
#endif
								  
#ifdef	ENABLE_NETPACK
								  else if (stricmp (s, "NET") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 100)
										  s2[100] = '\0';
									  
									  t_netpack(target, source, s2);
								  }
#endif
								  
								  // INT VARS
								  else if (stricmp(s, "SET") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  if (s2 == NULL)
										  R;
									  
									  level = check_access(userhost, target, 0, source);
									  // S ("PRIVMSG %s :3: userhost=%s, target=%s, source=%s, level=%d\n", target, userhost, target, source, level);
									  
									  if (level < INTVAR_LEVEL)
										  R;
									  
									  if (strlen (s2) > 1024)
										  s2[1024] = '\0';
									  
									  s3 = wt_tokenize(s2);
									  
									  setIntVar(target, source, s2, s3);
								  }
								  
								  else if (stricmp(s, "GET") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  if (s2 == NULL)
										  R;
									  
									  getIntVar(target, source, s2);
								  }


// Курсы говновалют
								  else if (stricmp (s, "нефть") == 0 || stricmp (s, "oil") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/oil.sh", s2);
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 1)
									  {
									    S("PRIVMSG %s :%s, %s\n", target, source, res);
									    R;
									  }
									  sleep(5);
								  }
								  else if (stricmp (s, "бакс") == 0 || stricmp (s, "доллар") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/rubl.sh", s2);
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 1)
									  {
									    S("PRIVMSG %s :%s, %s російських рублів\n", target, source, res);
									    R;
									  }
									  sleep(5);
								  }
								  else if (stricmp (s, "биткоин") == 0 || stricmp (s, "btc") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/btc.sh", s2);
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 1)
									  {
									    S("PRIVMSG %s :%s, %s\n", target, source, res);
									    R;
									  }
									  sleep(5);
								  }
								  else if (stricmp (s, "гривна") == 0 || stricmp (s, "грювня") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/grivna.sh", s2);
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 1)
									  {
									    S("PRIVMSG %s :%s, %s російських рублів\n", target, source, res);
									    R;
									  }
									  sleep(5);
								  }
								  else if (stricmp (s, "евро") == 0 || stricmp (s, "евра") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/euro.sh", s2);
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 1)
									  {
									    S("PRIVMSG %s :%s, %s російських рублів\n", target, source, res);
									    R;
									  }
									  sleep(5);
								  }

// Matershinnik N
								  else if (stricmp (s, "fucker") == 0 || stricmp (s, "доходяга") == 0 || stricmp (s, "матершинник") == 0 || stricmp (s, "синеносый") == 0)
								  {
								    s2 = strtok (NULL, "");
								    char *ms2 = s2;
								    trim(&ms2);
								    printf("BAD {%s}\n", ms2);
									  if (ms2 == NULL || strpbrk(s2, ",\t\r\n|\'\"`><&^*?/;:~^"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									else
									{
									    char RRR[4096];
									    bc1_infoU(ms2, RRR);
									    S ("PRIVMSG %s :%s, %s\n", target, source, RRR);
									}
								}

								  else if (stricmp (s, "wanker") == 0 || stricmp (s, "шулер") == 0 || stricmp (s, "жулик") == 0)
								  {
								    s2 = strtok (NULL, "");
									  if (s2 == NULL || strpbrk(s2, ", \t\r\n|\'\"`><&^*?/;:~^"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									else
									{
									    char RRR[4096];
									    bc_infoU(s2, RRR);
									    S ("PRIVMSG %s :%s, %s\n", target, source, RRR);
									}
								}

// Game
								  else if (stricmp (s, "вращаю") == 0 || stricmp (s, "кручу") == 0 || stricmp (s, "ставлю") == 0 || stricmp (s, "поиграем") == 0 || stricmp (s, "играю") == 0)
								  {
								    s2 = strtok (NULL, "");
								    if (s2 != 0)
								    {
								    long sum = atol(s2);
								    
									  if (sum < 1 || sum > 10000)
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									else
									{
									    int res = bc_dice(source, sum);
									    if (res == 0)
										S ("PRIVMSG %s :%s, лови +%d\n", target, source, sum);
									    else if (res == 1)
										S ("PRIVMSG %s :%s, ты проиграл %d\n", target, source, sum);
									    else if (res == 2)
										do_randomtopic(target, DONNO_RDB, source, buf);
									}
								    }
								    else
									do_randomtopic(target, DONNO_RDB, source, buf);
								}

// PING!
								  else if (stricmp (s, "ping") == 0 || stricmp (s, "пинг") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);

//									  if (level < 4 || s2 == NULL || strlen (s2) > 40 || strpbrk(s2, ", \t\r\n|\'\"><&^*?;:"))
									  if (s2 == NULL || strlen(s2) < 3 || strpbrk(s2, ", \t\r\n|\'\"`><&^*?/;:~^"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									  snprintf (temp,sizeof(temp), "ping %s -c 3 2>&1 | grep \'icmp_seq\\=2\\|ping\\:\\|packet\\ loss\\|bytes\\ of\\ data.\'", s2);
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 10)
									  {
										  char *ll2 = strchr((char *)res, '\n');
										  if (ll2 && ll2to-(ll2-res) > 5)
										  {
											*ll2 = 0;										    
											ll2++;
											S("PRIVMSG %s :\2%s\2, %s\n", target, source, res);
											S("PRIVMSG %s :%s\n", target, ll2);
										  }
										  else
										    S("PRIVMSG %s :%s\n", target, res);
										    
										  R;
									  }
									  sleep(5);
								  }


								  else if (stricmp (s, "ебеня") == 0 || stricmp (s, "geoip") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);

//									  if (level < 4 || s2 == NULL || strlen (s2) > 40 || strpbrk(s2, ", \t\r\n|\'\"><&^*?;:"))
									  if (s2 == NULL || strlen(s2) < 3 || strpbrk(s2, ", \t\r\n|\'\"`><&^*?/;:~^"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									  snprintf (temp,sizeof(temp), "geoiplookup %s | head -n 1", s2);
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 10)
									  {
											S("PRIVMSG %s :\2%s\2, %s\n", target, source, res);
									  }
									    else do_randomtopic(target, DONNO_RDB, source, buf);
									  sleep(5);
								  }

// XMMS info
								  else if (stricmp (s, "xmms") == 0 || stricmp (s, "играет?") == 0  || stricmp (s, "тишина") == 0)
								  {
									  level = check_access(userhost, target, 0, source);
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/music_zloy.sh");
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 10)
									  {
									  		if (strstr(res, "XMMS is not running"))
									  		{
												S("PRIVMSG %s :%s, XMMS is not running\n", target, source);
											}
											else
									  		{
												S("PRIVMSG %s :%s, %s\n", target, source, res);
											}
										 }
										else
											do_randomtopic(target, DONNO_RDB, source, buf);
									  sleep(5);
								  }

// SQUID info
								  else if (stricmp (s, "squid-info") == 0 || stricmp (s, "кашка") == 0)
								  {
									  level = check_access(userhost, target, 0, source);
									  snprintf (temp,sizeof(temp), "cachesize_2");
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 10 && ll2to < 50)
									  {
										S("PRIVMSG %s :%s, %s\n", target, source, res);
										 }
										else
											do_randomtopic(target, DONNO_RDB, source, buf);
									  sleep(5);
								  }

// SEEN/NICKS
								  else if (stricmp (s, "nicks") == 0 || stricmp (s, "ники") == 0 || stricmp (s, "nix") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);

//									  if (level < 4 || s2 == NULL || strlen (s2) > 40 || strpbrk(s2, ", \t\r\n|\'\"><&^*?;:"))
									  if (s2 == NULL || strlen(s2) < 3 || strpbrk(s2, ", \t\r\n|\'\"`><&^*?/;:~^"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									  char hRes[2048];
									  int nhRes = nicksByAddrEx(s2, hRes);
									  if (nhRes > 0)
									  {
										S("PRIVMSG %s :%s: %s\n", target, source, hRes);
									}
										else
										{
										S("PRIVMSG %s :%s, %s not found :/\n", target, source, s2);
										}
									  sleep(5);
								  }

// DEF codes lookup
								  else if (stricmp (s, "phoneid") == 0 || stricmp (s, "пробей") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);

//									  if (level < 4 || s2 == NULL || strlen (s2) > 40 || strpbrk(s2, ", \t\r\n|\'\"><&^*?;:"))
									  if (s2 == NULL || strlen(s2) < 3 || strpbrk(s2, ", \t\r\n|\'\"`><&^*?/;:~^"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									  char *ss2 = s2;
									  trim(&ss2);
									  
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/def_info.sh %s", ss2);
									  const char *res = run_program(temp);
									  if (res && strlen(res) > 5)
									  {
									  	char res_t2[1024];
									  	res_t2[0] = 0;
									  	bool defres = get_def_info((char *)res, res_t2, 1023);
									  	//if (defres)
									  	{
									  		S("PRIVMSG %s :%s, \2%s\2: %s\n", target, source, ss2, res_t2);
									  	}
									  }
									  else
									  {
									  	S("PRIVMSG %s :%s, not found :/\n", target, source);
									  	sleep(5);
									  }
									  sleep(5);
								  }

// OS/NMAP lookup

								  else if (stricmp (s, "os") == 0 || stricmp (s, "винда") == 0 || stricmp (s, "линух") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);

//									  if (level < 4 || s2 == NULL || strlen (s2) > 40 || strpbrk(s2, ", \t\r\n|\'\"><&^*?;:"))
									  if (level < 4 || s2 == NULL || strlen(s2) < 3 || strpbrk(s2, ", \t\r\n|\'\"`><&^*?/;:~^"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									  char *ss2 = s2;
									  trim(&ss2);
									  
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/os_finger.sh %s", ss2);
									  const char *res = run_program(temp);
									  if (res && strlen(res) > 5)
									  {
									  	S("PRIVMSG %s :%s, %s\n", target, source, res);
									  }
									  else
									  {
									  	S("PRIVMSG %s :%s, donno :/\n", target, source);
									  }
									  sleep(5);
								  }

// DNS lookup
								  
								  else if (stricmp (s, "dns") == 0 || stricmp (s, "кто") == 0 || stricmp (s, "kto") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);

//									  if (level < 4 || s2 == NULL || strlen (s2) > 40 || strpbrk(s2, ", \t\r\n|\'\"><&^*?;:"))
//									  if (s2 == NULL || strlen(s2) < 3 || strpbrk(s2, ", \t\r\n|\'\"><&^*?/;:~^"))
									  if (s2 == NULL || strlen(s2) < 3 || strpbrk(s2, ",\t\r\n`\'\"><&^*/;~^"))// hmmm...
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									  
									  // ? mark
									  char *ss2 = s2;
									  trim(&ss2);
									  int ddh = strlen(ss2);
									  if (ddh > 0 && ss2[ddh-1] == '?') ss2[ddh-1] = 0;
									  trim(&ss2);
									  //
									  
									  //NMAP mode ////////////////////////////////
									  char *pPort = strchr(ss2, ':');
									  if (!pPort) pPort = strchr(ss2, ' ');
									  do {
									  
									  if (!pPort) break;									  
									  int nPort = atoi(pPort+1);
									  if (nPort <= 0 || nPort >= 65536) break;
									  *pPort = 0; pPort++;
									  trim(&ss2);
									  
									  if (strstr(ss2, "192.168.1") || strstr(ss2, "127.0.0.1") || strstr(ss2, "localhost")  || strstr(ss2, "acidflash.org")) break;
									  
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/chkport.sh %s %d", ss2, nPort);
									  const char *res = run_program(temp);
									  if (res && strlen(res) > 10)
									  {
									  	S("PRIVMSG %s :\2%s\2, %s\n", target, source, res);
									  	sleep(5);
									  	R;
									  }
									  
									  } while (false);
									  ////////////////////////////////////////////
									  
									  // Sheck in seen
									char seenres[10][256];
									char seenres1[10][256];
									int nseenres = 10;
									int nseenres1 = 10;
									char *ex_ss2 = ss2;
									int nseenret = seenByNick(ss2, seenres, &nseenres, true);
									if (nseenret > 0)
									{
										ss2 = seenres[0];
									}
									int nseenret1 = seenByNick(ex_ss2, seenres1, &nseenres1, false);
									if (nseenret <= 0 && nseenret > 0)
									{
										strcpy(seenres[0], seenres1[0]);
										nseenret = nseenret1;
										ss2 = seenres[0];
									}
									
									if (nseenret <= 0)
									{
										nseenret = seenByAddr(ss2, seenres, &nseenres, true);
										if (nseenret > 0)
										{
											ss2 = seenres[0];
										}
										nseenret1 = seenByAddr(ex_ss2, seenres1, &nseenres1, false);
										if (nseenret <= 0 && nseenret > 0)
										{
											strcpy(seenres[0], seenres1[0]);
											nseenret = nseenret1;
											ss2 = seenres[0];
										}
									}
									  
									  snprintf (temp,sizeof(temp), "host -W 3 %s | grep \'\\ domain\\ name\\ pointer\\ \\|not\\ found\\|\\ has\\ address\\ \'", ss2);
									  const char *res = run_program(temp);
									  const char tplx2[] = " domain name pointer ";
									  const char tplx3[] = " has address ";
									  int ll2to = strlen(res);
									  int dnsCnt = 0;
									  if (ll2to > 3)
									  {
										char *ll2;
										int tpls = sizeof(tplx2);
										
									  	do
									  	{
										  ll2 = strstr((char *)res, tplx2);
										  if (!ll2) { ll2 = strstr((char *)res, (char *)tplx3); tpls = sizeof(tplx3); }
										  
										  if (ll2)
										  {
											char *ll2p = ll2 + tpls-1;
											
											// NL
											char *nl9 = strchr(ll2p, '\n');
											if (nl9) { *nl9 = 0; res = nl9+1; } else { ll2 = NULL; }
											
											trim(&ll2p);
											char *ll2pe = ll2p + strlen(ll2p)-1;
											if (ll2pe && *ll2pe == '.') { *ll2pe = 0; }
											
											if (dnsCnt == 0)
											{
												if (nseenret > 0)
												{
													if (nseenret1 > 0)
													{
													S("PRIVMSG %s :\2%s\2, %s (%s)\n", target, source, ll2p, seenres1[0]);
													}
													else
													{
													S("PRIVMSG %s :\2%s\2, %s (%s)\n", target, source, ll2p, ss2);
													}
												//S("PRIVMSG %s :\2%s\2, %s (%s)\n", target, source, ll2p, ss2);
												}
												else
												{
												S("PRIVMSG %s :\2%s\2, %s\n", target, source, ll2p);
												}
											}
											else
											{
												S("PRIVMSG %s :%s, %s  (%d)\n", target, source, ll2p, (dnsCnt+1));
											}
											dnsCnt++;
										  }
										  else
										  {
										  	if (dnsCnt == 0)
										  	{
										  		if (nseenret > 0)
										  		{
										  			if (nseenret1 > 0)
										  			{
										    			S("PRIVMSG %s :%s, %s\n", target, source, seenres1[0]);
										  			}
										  			else
										  			{
										    			S("PRIVMSG %s :%s, %s\n", target, source, seenres[0]);
										  			}
										  		}
										  		else
										  		{
										    	S("PRIVMSG %s :%s, not found :/\n", target, source);
										  		}
										    }
										    break;
										  }
										 }
										 while (dnsCnt < 6 && ll2);
									  }
									  else
									  S("PRIVMSG %s :%s, :(((\n", target, source);
									  
									  sleep(5);
								  }

// < SET "WHO?" :) 04.2007
				  else if (stricmp (s, "marry") == 0 || stricmp (s, "полюби") == 0)
								  {
												      s2 = strtok (NULL, "");
												      wt_trimRight(s2);
												    				      
												    				      level = check_access(userhost, target, 0, source);
																    				      int msl = strlen(s2);
																				      
																				    				      if (level < 5 || s2 == NULL || msl < 1 || msl > 30 || strpbrk(s2, ", \t\r\n|\'\"><&^*?/;:~^"))
																								    				      {
																												    					  do_randomtopic(target, DONNO_RDB, source, buf);
																																						  R;
																																										      }
																																										    				    strncpy(g_rnick_cache, s2, 1023);
																																																			g_lnow = time(0);
																																																			
																																																							    S("PRIVMSG %s :OK, %s :)\n", target, source);
																																																											      }
																																																											      // SET "WHO? />
																																																											      
																																																											     // LENGTH!
								  else if (stricmp (s, "length") == 0 || stricmp (s, "попробуй") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);

//									  if (level < 4 || s2 == NULL || strlen (s2) > 40 || strpbrk(s2,", \t\r\n|\'\"><&^*"))
									  if (s2 == NULL || strlen(s2) < 3 || strlen(s2) > 1023 || strpbrk(s2, ",\t\r\n|\'\"`><$^*&;^"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/length.sh  %s\n", s2);
									  // snprintf (temp,sizeof(temp), "wget 2>&1 --spider --tries=1 --timeout=5 %s | grep \'Length\\:\\|Длина\\:\'\n", s2);
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 5)
									  {
									    S("PRIVMSG %s :\2%s\2: %s\n", target, source, res);
									    R;
									  }
									  else
//									    S("PRIVMSG %s :\2%s\2: облом-с\n", target, source);
									    do_randomtopic(target, DONNO_RDB, source, buf);
									  sleep(5);
								  }
								  
// Base64
								  else if (stricmp (s, "закодируй") == 0 || stricmp (s, "зокодируй") == 0)
								  {
									  s2 = strtok (NULL, "");
									  if (s2 == NULL || strlen(s2) < 3 || strlen(s2) > 1023 || strpbrk(s2, ",\t\r\n|\'\"`^;"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/b64e.sh  %s\n", s2);
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 3)
									  {
									    S("PRIVMSG %s :\2%s\2: %s\n", target, source, res);
									    R;
									  }
									  else
//									    S("PRIVMSG %s :\2%s\2: облом-с\n", target, source);
									    do_randomtopic(target, DONNO_RDB, source, buf);
									  sleep(5);
								  }
								  else if (stricmp (s, "декодируй") == 0 || stricmp (s, "раскодируй") == 0)
								  {
									  s2 = strtok (NULL, "");
									  if (s2 == NULL || strlen(s2) < 3 || strlen(s2) > 1023 || strpbrk(s2, ",\t\r\n|\'\"`^;"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/b64d.sh  %s\n", s2);
									  const char *res = run_program(temp);
									  int ll2to = strlen(res);
									  if (ll2to > 3)
									  {
									    S("PRIVMSG %s :\2%s\2: %s\n", target, source, res);
									    R;
									  }
									  else
//									    S("PRIVMSG %s :\2%s\2: облом-с\n", target, source);
									    do_randomtopic(target, DONNO_RDB, source, buf);
									  sleep(5);
								  }
								  
// >>
// SHOWPKG!
								  else if (stricmp (s, "showpkg") == 0 || stricmp (s, "покажи") == 0)
								  {
									  s2 = strtok (NULL, "");
									  level = check_access(userhost, target, 0, source);
									  if (s2 == NULL || strlen(s2) < 3 || strlen(s2) > 64 || strpbrk(s2, ",\t\r\n|\'\"`><$^*&;:~^"))
									  {
										  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
									  }
									  snprintf (temp,sizeof(temp), "/home/olivka/bot/showpkg.sh  %s\n", s2);
									  char *res = (char *)run_program(temp);
									  int ll2to = strlen(res);
									  int __i1 = 0;
									  if (ll2to > 5)
									  {
									    // If "W:"/not found, search in cache
									    // apt-cache search 3dd | head -n3
									
									    if (*res != 'V')
									    {
										snprintf (temp,sizeof(temp), "apt-cache search %s | head -n3\n", s2);
										res = (char *)run_program(temp);
										int ll3to = strlen(res);
										int _nCnt1 = 0;
										bool aptDone = false;
										
										if (ll3to > 5)
										{
										    char *lstCm = NULL;
										    for (__i1 = 0; __i1 < ll3to; __i1++)
										    {
											if (res[__i1] == '\n')
											{
											    res[__i1] = ',';
											    lstCm = res+__i1;;
											}
										    }
										    /*
										    if (_nCnt1 > 1)
										    {
											for (__i1 = 0; __i1 < ll3to; __i1++)
											{
											    if (res[__i1] == '\n' && (__i1 < _nCnt1-1))
												res[__i1] = '$';
											}
										    */
										if (lstCm)
										    *lstCm = '.';
										
										S("PRIVMSG %s :\2apt\2-search: %s\n", target, res);
										aptDone = true;
										    //}
										}
										
										if (!aptDone)
										    do_randomtopic(target, DONNO_RDB, source, buf);
    									}
									    else
									    {									    
										char *res11 = strchr(res,'\n');
										if (res11 && (*res == 'V')) *res11 = ',';
										S("PRIVMSG %s :\2%s\2: %s\n", target, s2, res);
										R;
									    }
									  }
									  else
//									    S("PRIVMSG %s :\2%s\2: облом-с\n", target, source);
									    do_randomtopic(target, DONNO_RDB, source, buf);
									  // sleep(5);
								  }
								  
// >>

#ifdef	ENABLE_DICTIONARY
								  else if (stricmp (s, "translate") == 0 || stricmp (s, "переведи") == 0 || stricmp (s, "wtf") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 512)
										  s2[512] = '\0';
									  
									  // 28/10/2006
									  wt_trimRight(s2);
									  if (!t_translate2(target, source, s2, false))
									  {
										if (!dict(target, source, s2, "vera", true))
										{
										// 03/02/2007
										snprintf (temp,sizeof(temp), "/home/olivka/bot/showpkg.sh  %s\n", s2);
										const char *res = run_program(temp);
										int ll2to = strlen(res);
										if ((ll2to > 5) && (*res == 'V'))
										{
										    char *res11 = strchr((char *)res,'\n');
										    if (res11) *res11 = ',';
										    S("PRIVMSG %s :%s, \2apt\2 sais: %s\n", target, source, res);
										}
										
										}
									  }
									  // ####
								  }
								  else if (stricmp (s, "translate2") == 0 || stricmp (s, "переведи2") == 0 || stricmp (s, "wtf2") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 512)
										  s2[512] = '\0';

									  wt_trimRight(s2);
									  if (!t_translate2(target, source, s2, true))
										dict(target, source, s2, "vera", true);
								  }
								  
								  else if (stricmp (s, "addword") == 0 || stricmp (s, "запомни") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);
									  
									  if (!inv_vars[INTV_DICT] && level < ROOT_LEVEL)
										  R;
									  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 1024)
										  s2[1024] = '\0';
									  
									  char *_s2 = s2;
									  wt_trimRight(_s2);
									  s3 = wt_tokenize2(&_s2);
									  
									  if (s3)
									    t_addWord2(target, source, _s2, s3);
								  }
#endif


            // 2018 Aug - add Google translate : )
             else if (stricmp(s, "ru") == 0 || stricmp(s, "rus") == 0) {
                s2 = strtok(NULL, "");

                if (s2 == NULL)
                    R;

                if (strlen(s2) > 512)
                    s2[512] = '\0';
                wt_trimRight(s2);
                        snprintf(temp, sizeof(temp), "/home/olivka/bot/2ru.sh  %s\n", s2);
                        const char *res = run_program(temp);
                        int ll2to = strlen(res);
                        if ((ll2to > 5)) {
                            S("PRIVMSG %s :%s, %s\n", target, source, res);
                        }

            } else if (stricmp(s, "en") == 0 || stricmp(s, "eng") == 0) {
                s2 = strtok(NULL, "");

                if (s2 == NULL)
                    R;

                if (strlen(s2) > 512)
                    s2[512] = '\0';
                wt_trimRight(s2);
                        snprintf(temp, sizeof(temp), "/home/olivka/bot/2en.sh  %s\n", s2);
                        const char *res = run_program(temp);
                        int ll2to = strlen(res);
                        if ((ll2to > 5)) {
                            S("PRIVMSG %s :%s, %s\n", target, source, res);
                        }
            
            } else if (stricmp(s, "ua") == 0 || stricmp(s, "ukr") == 0) {
                s2 = strtok(NULL, "");

                if (s2 == NULL)
                    R;

                if (strlen(s2) > 512)
                    s2[512] = '\0';
                wt_trimRight(s2);
                        snprintf(temp, sizeof(temp), "/home/olivka/bot/2ua.sh  %s\n", s2);
                        const char *res = run_program(temp);
                        int ll2to = strlen(res);
                        if ((ll2to > 5)) {
                            S("PRIVMSG %s :%s, %s\n", target, source, res);
                        }
            }


// <!-- SENDMAIL
								  else if (stricmp (s, "sendmail") == 0 || stricmp (s, "напиши") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);
									  
									  if (level < USER_LEVEL){
									  do_randomtopic(target, DONNO_RDB, source, buf);
										  R;
										  }
									  /*
									  if (!inv_vars[INTV_DICT] && level < ROOT_LEVEL)
										  R;
									  */
									  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 1024)
										  s2[1024] = '\0';

									// check time
									time_t w_now = time(NULL);
									if (w_now - g_last_sendmail_time <= SENDMAIL_TIME)
									{
									    S("PRIVMSG %s :%s, please wait \2%d\2s more\n", target, source, SENDMAIL_TIME-(w_now-g_last_sendmail_time)+1);
									    //do_randomtopic(target, DONNO_RDB, source, buf);
									    R;
									}									//
									
									  
									  char *_s2 = s2;
									  wt_trimRight(_s2);
									  s3 = wt_tokenize2(&_s2);
									  
									  // Alias
									  char *sendTo = _s2;
									  _hashfile alias(CDICT_SENDMAIL);
									  DICTREC dr4;
									  char subj[256];
									  sprintf(subj, "%s from %s", source, target);
									  if (alias.open())
									  {
									    if (alias.find(_s2, &dr4))
										sendTo = dr4.destination;
									    alias.close();
									  }
									  
									  if (s3)
									  {
									    if (isdigit(sendTo[0]))
										sprintf(subj, "%s", source);
									// convert to KOI8
									    iconv_t cnv = iconv_open("KOI8-R", "CP1251");
									    char s3_koi[1024] = "";
									    if (cnv != (iconv_t)(-1))
									    {
								    		char *ad = s3, *od = s3_koi;
								    		size_t as = strlen(s3), os = 1023;
										int as2 = as;
								    		if (iconv(cnv, &ad, &as, &od, &os) != -1)
										    s3 = s3_koi;
										s3[as2] = 0;
										iconv_close(cnv);
									    }									
										
									    int smRes = net_sendmail(sendTo, subj, s3);
									    if (smRes)
									    {
										S("PRIVMSG %s :%s, cannot send message to \2%s\2\n", target, source, _s2);
										if (level >= ADMIN_LEVEL)
										    S("PRIVMSG %s :Cannot send message to \2%s\2\n", source, sendTo);
									    }
									    else
									    {
										S("PRIVMSG %s :%s, sent to \2%s\2 OK :)\n", target, source, _s2);
										if (level >= ADMIN_LEVEL)
										    S("PRIVMSG %s :Sent to \2%s\2 OK :)\n", source, sendTo);
										g_last_sendmail_time = w_now;
									    }
									  }
									  else
									    do_randomtopic(target, DONNO_RDB, source, buf);
								  }
// SENDMAIL -->

// <!-- ALIASES
								  else if (stricmp (s, "alias") == 0 || stricmp (s, "мыло") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);
									  
									
									  if (level < USER_LEVEL)
										  R;				  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 1024)
										  s2[1024] = '\0';

									  char *_s2 = s2;
									  wt_trimRight(_s2);
									  s3 = wt_tokenize2(&_s2);
									  
									  // Alias
									  _hashfile alias(CDICT_SENDMAIL);
									  DICTREC dr4;
									  if (s3 && _s2)
									  {
									    bool openOK = alias.openRW();
									    if (!openOK)
									    openOK = alias.create();
									    
									    if (level < ADMIN_LEVEL)
									    {
										bool overW = alias.find(_s2, &dr4);
										if (overW)
										{
										    S("PRIVMSG %s :%s, \2%s\2  already defined :)\n", target, source, _s2);
										    alias.close();
										    R;
										}
									    }
									    
									    if (openOK)
									    {									    
    										if (alias.add(_s2, s3))
										    S("PRIVMSG %s :%s, ok I wil use \2%s\2 instead of \2%s\2 :)\n", target, source, s3, _s2);
										else
										    do_randomtopic(target, DONNO_RDB, source, buf);
										    //S("PRIVMSG %s :%s, failed :/\n", target, source);
										alias.close();
									    }
									    else
										do_randomtopic(target, DONNO_RDB, source, buf);
									  }
									  else
									    do_randomtopic(target, DONNO_RDB, source, buf);
								  }



								  else if (stricmp (s, "getalias") == 0 || stricmp (s, "запали") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);
									  if (level < ADMIN_LEVEL)  R;
									  if (s2 == NULL)  R;
									  
									  if (strlen (s2) > 1024)
										  s2[1024] = '\0';

									  char *_s2 = s2;
									  wt_trimRight(_s2);
									  
									  // Alias
									  _hashfile alias(CDICT_SENDMAIL);
									  DICTREC dr4;
									  if (_s2 && alias.open())
									  {
										if (alias.find(_s2, &dr4))
										{
										    S("PRIVMSG %s :%s, \2%s\2: %s :)\n", target, source, _s2, dr4.destination);
										    alias.close();
										    R;
										}
										else
										    S("PRIVMSG %s :%s, no math for \2%s\2 :/\n", target, source, _s2);
										alias.close();
									    }
									    else
										do_randomtopic(target, DONNO_RDB, source, buf);
								  }


								  else if (stricmp (s, "delalias") == 0)
								  {
									  s2 = strtok (NULL, "");
									  
									  level = check_access(userhost, target, 0, source);
									  
									
									  if (level < ROOT_LEVEL)
										  R;
									  
									  
									  if (s2 == NULL)
										  R;
									  
									  if (strlen (s2) > 1024)
										  s2[1024] = '\0';

									  char *_s2 = s2;
									  wt_trimRight(_s2);
									  s3 = wt_tokenize2(&_s2);
									  
									  // Alias
									  _hashfile alias(CDICT_SENDMAIL);
									  DICTREC dr4;
									  if (s3 && _s2 && alias.openRW())
									  {
									    if (alias.del(_s2))
										S("PRIVMSG %s :%s, ok I deleted \2%s\2 :)\n", target, source, _s2);
									    else
										S("PRIVMSG %s :%s, failed :))\n", target, source);
									    alias.close();
									  }
									  else
									    do_randomtopic(target, DONNO_RDB, source, buf);
								  }
// SENDMAIL -->

// Enable/disable silent channels
								  else if (stricmp (s, "смирно") == 0 || stricmp (s, "смирно!") == 0 || stricmp (s, "молчи") == 0 || stricmp (s, "закройся") == 0)
								  {
								  	if (!noChanEnable)
								  	{
										S("PRIVMSG %s :OK, %s :(\n", target, source);
										noChanEnable = true;
									}
								  }
								  else if (stricmp (s, "разрешаю") == 0 || stricmp (s, "вольно") == 0 || stricmp (s, "вольно!") == 0 || stricmp (s, "говори") == 0)
								  {
								  	if (noChanEnable)
								  	{
										S("PRIVMSG %s :OK, %s :)\n", target, source);
										noChanEnable = false;
									}
								  }

								  else if (stricmp (s, "SLEEP") == 0 || stricmp (s, "HUSH") == 0 || stricmp (s, "захлопнись") == 0) {
									  // 28/10/2006
									  if (true) { //check_access (userhost, target, 0, source) >= SLEEP_LEVEL) {
										  Sleep_Toggle = 1;

										  // 28/10/2006
										  // S ("PRIVMSG %s :%s\n", target, GOSLEEP_ACTION);
										  do_randomtopic(target, SLEEP_RDB, source, buf);
										  
										  strncpy (sleep_chan, target, sizeof(sleep_chan));
									  }
								  }
								  else if (stricmp (s, "UNIXTIME") == 0) {
									  s2 = strtok (NULL, " ");
									  if (s2 == NULL)
										  R;
									  unixtime = atoi (s2) - time (NULL);
									  if (unixtime > 86400)
										  S
										  ("PRIVMSG %s :%s, %d day%s, %02d:%02d\n",
										  target, source, unixtime / 86400,
										  (unixtime / 86400 == 1) ? "" : "s",
										  (unixtime / 3600) % 24, (unixtime / 60) % 60);
									  else if (unixtime > 3600)
										  S
										  ("PRIVMSG %s :%s, %d hour%s, %d min%s\n",
										  target, source, unixtime / 3600,
										  unixtime / 3600 == 1 ? "" : "s",
										  (unixtime / 60) % 60,
										  (unixtime / 60) % 60 == 1 ? "" : "s");
									  else
										  S
										  ("PRIVMSG %s :%s, %d minute%s, %d sec%s\n",
										  target, source, unixtime / 60,
										  unixtime / 60 == 1 ? "" : "s",
										  unixtime % 60, unixtime % 60 == 1 ? "" : "s");
								  }
								  else if (stricmp (s, "CPU?") == 0 || stricmp (s, "горячий?") == 0) {
									  getrusage (RUSAGE_SELF, &r_usage);
									  S
										  ("PRIVMSG %s :CPU usage: %ld.%06ld, System = %ld.%06ld\n",
										  target, r_usage.ru_utime.tv_sec, r_usage.ru_utime.tv_usec);
								  }
								  else if (stricmp (s, "DISPLAY") == 0) {
									  s2 = strtok (NULL, " ");
									  if (s2 == NULL)
										  R;
									  display_url (target, source, s2);
#ifndef WIN32
								  }
								  // <NEW!> 29/03/2006 2:52 am // Песдец ночка блиа 8-)
								  
								  else
									  if (stricmp (s, "ебут?") == 0 || stricmp (s, "сосуны") == 0 || stricmp (s, "suckers") == 0)
									  {
										  do
										  {
											  if (check_access (userhost, target, 0, source) >= ROOT_LEVEL)
											  {
												  snprintf (temp,sizeof(temp), "w | grep ppp\n");
												  const char *res = run_program(temp);
												  if (strlen(res) > 10)
												  {
													  S("PRIVMSG %s :%s, %s\n", target, source, res);
													  break;
												  }
											  }
											  do_randomtopic(target, DONNO_RDB, source, buf);
										  }
										  while (0);
									  }
								  else
								    // 2017 Aug - hebrew
									  if (stricmp (s, "жиды") == 0 || stricmp (s, "евреи") == 0)
									  {
										  do
										  {
											  {
												  snprintf (temp,sizeof(temp), "/home/olivka/bot/heb.sh\n");
												  const char *res = run_program(temp);
												  if (strlen(res) > 6)
												  {
													  S("PRIVMSG %s :%s, %s\n", target, source, res);
													  break;
												  }
											  }
											  do_randomtopic(target, DONNO_RDB, source, buf);
										  }
										  while (0);
									  }
								  else
									  // Следующий
									  if (stricmp (s, "следующий") == 0)
									  {
										  do
										  {
											  // NON-FREE :-/
											  //if (check_access (userhost, target, 0, source) >= ADMIN_LEVEL)
											  {
												  g_lnow = 0; //time(0)-RNICK_CACHE_TIME;
												  S("PRIVMSG %s :%s :)\n", target, source);
												  break;
											  }
											  do_randomtopic(target, DONNO_RDB, source, buf);
										  }
										  while (0);
									  }
									  

								  else
									  // NETSTAT (2006)
									  if (stricmp (s, "всосалось") == 0 || stricmp (s, "сколько?") == 0)
									  {
										  do
										  {
											  // FREE :)
											  // if (check_access (userhost, target, 0, source) >= ROOT_LEVEL)
											  {
												  snprintf (temp,sizeof(temp), "netstat -s | grep total\\ packets\\ received | sed 's/^ *//'\n");
												  const char *res = run_program(temp);
												  char *pDatN = (char *)res, *p4 = NULL;
												  char bytes_val[128];
												  if (strlen(res) > 5)
												  {
													  trim(&pDatN);
													  //S("PRIVMSG zloy :[%s], [%s]\n", res, pDatN);
													  //printf("GET1:[%s]\n", pDatN);
													  p4 = strchr(pDatN, ' ');
													  if (p4 && (p4-pDatN >= 1))
													  {
														  *p4 = 0;
														  unsigned long nbr = (unsigned long)atol(pDatN)/2;
													          //S("PRIVMSG zloy :[%s], [%lu]\n", pDatN, nbr);
														  format_bytes(bytes_val, nbr, true);
														  S("PRIVMSG %s :%s, %s\n", target, source, bytes_val);
														  break;
													  }
												  }
											  }
										  }
										  while (0);
									  }
									  else

									  // PWGEN (2006)
									  
									  // dec 2007
									  // int g_pwLen = 13;
									  
									  if (stricmp (s, "пароль") == 0 || stricmp (s, "пороль") == 0)
									  {
										  snprintf (temp,sizeof(temp), "pwgen %d 1 -n -c %s", g_pwLen, (s[1] == 'о')?"-s\n":"\n");
										  const char *res = run_program(temp);
										  if (strlen(res) > 5)
        									  {
											  S("PRIVMSG %s :%s: \2%s\n", target, source, res);
										  }
									  }
									  else

										  if (stricmp (s, "красавчег") == 0 || stricmp (s, "ебали?") == 0)
										  {
											  do
											  {
												  if (check_access (userhost, target, 0, source) >= ROOT_LEVEL)
												  {
													  snprintf (temp,sizeof(temp), "who | tail -n 1\n");
													  const char *res = run_program(temp);
													  if (strlen(res) > 10)
													  {
															S("PRIVMSG %s :%s, %s\n", target, source, res);
															break;
													  }
												  }
												  do_randomtopic(target, DONNO_RDB, source, buf);
											  }
											  while (0);
										  }
										  
										  else								  
											  if (stricmp (s, "моск") == 0 || stricmp (s, "свободно?") == 0 || stricmp (s, "free") == 0)
											  {
												  do
												  {
													  //        if (check_access (userhost, target, 0, source) >= ROOT_LEVEL)
													  //	{
													  snprintf (temp,sizeof(temp), "vmstat -s -S M | grep free\n");
													  const char *res = run_program(temp);
													  int nres = 0;
													  while (*res == ' ' && ++nres < 10) res++;
													  if (strlen(res) > 10)
													  {
														  S("PRIVMSG %s :%s\n", target, res);
														  break;
													  }
													  //	}
													  do_randomtopic(target, DONNO_RDB, source, buf);
												  }
												  while (0);
											  }
											  
										  else								  
											  if (stricmp (s, "активной") == 0 || stricmp (s, "влезло?") == 0 || stricmp (s, "тяжко?") == 0)
											  {
												  do
												  {
													  //        if (check_access (userhost, target, 0, source) >= ROOT_LEVEL)
													  //	{
													  snprintf (temp,sizeof(temp), "vmstat -s -S M | grep active\n");
													  const char *res = run_program(temp);
													  int nres = 0;
													  while (*res == ' ' && ++nres < 10) res++;
													  if (strlen(res) > 10)
													  {
														  S("PRIVMSG %s :%s\n", target, res);
														  break;
													  }
													  //	}
													  do_randomtopic(target, DONNO_RDB, source, buf);
												  }
												  while (0);
											  }
											  
										  else								  
											  if (stricmp (s, "пассивной") == 0 || stricmp (s, "войдет?") == 0 || stricmp (s, "влезет?") == 0)
											  {
												  do
												  {
													  //        if (check_access (userhost, target, 0, source) >= ROOT_LEVEL)
													  //	{
													  snprintf (temp,sizeof(temp), "vmstat -s -S M | grep inactive\n");
													  const char *res = run_program(temp);
													  int nres = 0;
													  while (*res == ' ' && ++nres < 10) res++;
													  if (strlen(res) > 10)
													  {
														  S("PRIVMSG %s :%s\n", target, res);
														  break;
													  }
													  //	}
													  do_randomtopic(target, DONNO_RDB, source, buf);
												  }
												  while (0);
											  }
											  
											  /*								  
											  else								  
											  if (stricmp (s, "ping") == 0 || stricmp (s, "пинг") == 0)
											  {
											  do
											  {
											  if (check_access (userhost, target, 0, source) >= CMD_LEVEL)
											  {
											  snprintf (temp,sizeof(temp), "ping -c 1 www.tula.net\n");
											  const char *res = run_program(temp);
											  int nres = 0;
											  while (*res == ' ' && ++nres < 10) res++;
											  if (strlen(res) > 10)
											  {
											  S("PRIVMSG %s :%s\n", target, res);
											  break;
											  }
											  }
											  do_randomtopic(target, DONNO_RDB, source, buf);
											  }
											  while (0);
											  }
											  */
											  
											  // </NEW!> 29/03/2006
											  
/*
											  // 13/11/2006
											  else if (stricmp (s, "погода") == 0 || stricmp (s, "холодно?") == 0 || stricmp (s, "wz") == 0) {
												  snprintf (temp,sizeof(temp), "/home/olivka/bot/weather.sh");
												  S ("PRIVMSG %s :%s, %s\n", target, source, run_program (temp));
											  }
*/


											  
											  // URL cache
											  else if (stricmp (s, "URL") == 0 || stricmp (s, "боян") == 0 || stricmp (s, "бойан") == 0) {
											  	  url_cache url4(UCN);
											  	  URL uux;
											  	  if (url4.getLast(&uux))
											  	  {
												  	S ("PRIVMSG %s :%s, \37%s\37  (%s/%s, %s)\n", target, source, uux.href, uux.nick, uux.chan, nocrlf(ctime(&uux.date)));
												  }
												  else
												  	do_randomtopic(target, DONNO_RDB, source, buf);
												  
											  }
											  
											  else if (stricmp (s, "UPTIME") == 0 || stricmp (s, "живче?") == 0) {
												  snprintf (temp,sizeof(temp), "uptime\n");
												  S ("PRIVMSG %s :Uptime: %s\n", target, run_program (temp));
											  }
											  else if (stricmp (s, "какдела?") == 0 || stricmp (s, "кагдила?") == 0) {
												  snprintf (temp,sizeof(temp), "uptime\n");
												  S ("PRIVMSG %s :Дела такие, %s: localtime %s\n", target, source, run_program (temp));
											  }
											  else if (stricmp (s, "вилки") == 0) {
												  snprintf (temp,sizeof(temp), "vmstat -f | sed \'s/^ *//\'\n");
												  S ("PRIVMSG %s :%s, %s\n", target, source, run_program (temp));
											  }
											  else if (stricmp (s, "OSVER") == 0 || stricmp (s, "винда?") == 0) {
												  snprintf (temp,sizeof(temp), "/home/olivka/bot/sysinfo1.sh");
												  S ("PRIVMSG %s :I am running \2Debian GNU/Linux 4.0\2/%s\n", target,
													  run_program (temp));
											  }
											  else if (stricmp (s, "MEM") == 0 || stricmp (s, "занято?") == 0 || stricmp (s, "зохавано") == 0) {
												  snprintf (temp, sizeof(temp),"/home/olivka/bot/meminfo1.sh");
												  S ("PRIVMSG %s :%s\n", target, run_program (temp));
											  }
											  else if (stricmp (s, "RDB") == 0) {
												  s2 = strtok (NULL, "");
												  if (s2 == NULL) {
													  snprintf (temp,sizeof(temp), "ls dat/*.rdb | wc\n");
													  S ("PRIVMSG %s :RDB: %s\n", target, run_program (temp));
												  }
												  else {
													  if (strspn (s2, SAFE_LIST) != strlen (s2)) {
														  S
															  ("PRIVMSG %s :%s, rdb files are made up of letters and or numbers, no other text is accepted.\n",
															  target, source);
														  R;
													  }
													  snprintf (temp,sizeof(temp),"ls -la dat/*.rdb | grep %s | tail 3\n", s2);
													  S ("PRIVMSG %s :%s\n", target, run_program (temp));
												  }
#endif
											  }
											  else
												  if (stricmp (s, "DELETE") == 0
													  || stricmp (s, "REMOVE") == 0
													  || stricmp (s, "FORGET") == 0 || stricmp (s, "DEL") == 0) {
#ifdef  REQ_ACCESS_DEL
													  if (check_access (userhost, target, 0, source) >= 1) {
#endif
														  s2 = strtok (NULL, " ");
														  if (s2 == NULL) {
															  S ("PRIVMSG %s :%s what, %s?\n", target, s, source);
															  R;
														  }
														  if (strlen (s2) > MAX_TOPIC_SIZE)
															  s2[MAX_TOPIC_SIZE] = '\0';
#ifdef  LOG_ADD_DELETES
														  fslog (ADD_DELETES, "[%s] %s!%s DEL %s\n",
															  date (), source, userhost, s2);
#endif
														  if (*s2 == '~') {     /* need level 2 to delete .rdb files */
															  if (check_access (userhost, target, 0, source) >= 2) {
																  delete_url (source, s2, target);
															  }
															  R;
														  }
														  delete_url (source, s2, target);
#ifdef  REQ_ACCESS_DEL
													  }
#endif
												  }
												  else if (stricmp (s, "TELL") == 0) {
													  s2 = strtok (NULL, " ");
													  if (s2 == NULL) {
														  L085 (target, source);
														  R;
													  }
													  s3 = strtok (NULL, " ");
													  if (s3 == NULL) {
														  L083 (target, source, s2);
														  R;
													  }
													  if (stricmp (s3, Mynick) == 0 || stricmp (s3, MyAnick) == 0)
													  R;                    /* don't bother telling
													  * myself about stuff */
													  if (stricmp (s3, "ABOUT") == 0) {
														  s4 = strtok (NULL, " ");
														  if (s4 == NULL) {
															  L084 (target, source, s2);
															  R;
														  }
														  strlwr (s4);
														  show_url (s2, get_multiword_topic (s4), target, 1, 0,
															  userhost, 1);
													  }
													  else {
														  strlwr (s3);
														  show_url (s2, get_multiword_topic (s3), target, 1, 0,
															  userhost, 1);
													  }
												  }
												  else
													  if (stricmp (s, "WHERE") == 0
														  || stricmp (s, "WHO") == 0 || stricmp (s, "WHAT") == 0) {
														  s2 = strtok (NULL, " ");
														  if (s2 == NULL) {
															  L086 (target, source);
															  R;
														  }
														  s3 = strtok (NULL, " ");
														  if (s3 == NULL)
															  R;
														  strlwr (s3);
														  ptr3 = strchr (s3, '?');
														  if (ptr3 != NULL)
															  memmove (ptr3, ptr3 + 1, strlen (ptr3 + 1) + 1);
														  ptr3 = strchr (s3, '!');
														  if (ptr3 != NULL)
															  memmove (ptr3, ptr3 + 1, strlen (ptr3 + 1) + 1);
														  if (stricmp (s3, "A") == 0 || stricmp (s3, "AN") == 0) {
															  s4 = strtok (NULL, " ");
															  if (s4 == NULL) {
																  L087 (target, s, s2, s3, *CMDCHAR);
																  R;
															  }
															  show_url (source,
																  get_multiword_topic (s4), target, 1, 0, userhost,
																  0);
														  }
														  else
															  show_url (source,
															  get_multiword_topic (s3), target, 1, 0, userhost,
															  0);
													  }
													  else
														  show_url (source,
														  get_multiword_topic (s), target, 1, 0, userhost, 0);
    }
    else
		// S ("PRIVMSG %s :%s\n", target, WHUT);
		do_randomtopic (target, WHUT_RDB, source, buf);
	
#if GENERAL_QUESTIONS == 1
  }
  else {
	  if (Sleep_Toggle == 1)
		  R;
		  
		  // URL check
		  url_cache uc(UCN);
		  URL uu, uu2;
//		  S ("PRIVMSG йопт :BUF: %s\n", buf);
//		  S ("PRIVMSG йопт :S: %s\n", bufCopy);
		  if (uc.findUrl(bufCopy, &uu))
		  {
		  	if (uc.find(uu.href, &uu2))
		  	{
				// Ignore some :)
//int aaa=stricmp(uu.href, "http://temp.2396.ru/");
//S ("PRIVMSG zloy :S: [%s] [%s] [%d] [%s]\n", bufCopy, uu.href, aaa, uu2.href);
				if (stricmp(uu.href, "http://temp.2396.ru/") != 0)
				    return;
				if (stricmp(uu.href, "http://newtemp.redip.ru/") != 0)
				    return;
		  		S ("PRIVMSG %s :%s, это уже курили (%s/%s, %s)\n", target, source, uu2.nick, uu2.chan, nocrlf(ctime(&uu2.date)));
		  		db_sleep(5);
		  		return;
		  	}
		  	else
		  	{
		  		uu.nick = source;
		  		uu.chan = target;
		  		uc.add(&uu);
		  		return;
		  	}
		  }
		  /////////////
		  
	  show_url (source, get_multiword_topic (cmd), target, 0, 1,
		  userhost, 0);
#endif
	  i = 0;
  }
}

void
gs26 ()
{
	long i = 0;
	struct sl124 *c;
	c = sh124;
	spr++;
	if (spr > snr)
		spr = 1;
	while (c != NULL) {
		i++;
		if (i == spr) {
			strncpy (BS, c->name, sizeof(BS));
			BP = c->port;
		}
		c = c->next;
	}
}

void
add_s25 (char *server, long port)
{
	struct sl124 *n;
	n = (struct sl124 *)
		malloc (sizeof (struct sl124));
	if (n == NULL) {
		fslog ("error.log", "AHHH! No ram left! in add_s25!\n");
		R;
	}
	memset (n, 0, sizeof (struct sl124));
	snr++;
	if (n != NULL) {
		strncpy (n->name, server, sizeof(n->name));
		n->port = port;
		n->next = sh124;
		sh124 = n;
	}
}

void
show_url (char *nick, char *topic,
          char *target, long donno,
          long floodpro, char *uh, long no_pipe)
{
	FILE *fp;
	long x = 0, length = 0, toggle = 0, A =
		0, gotit = 0, D = 0, F = 0, Tog = 0;
	char b[STRING_LONG], *subj, *ptr,
		temp[STRING_LONG] = "", Data[STRING_LONG] = "", *ptr8 = "";
	char crm1[STRING_LONG], crm2[STRING_LONG], bFirst = 1;
	bool rare = false;
	int mDelay = 0;
	
	strlwr (topic);
	
	/* removes the question mark */
	if ((ptr = strchr (topic, '?')) != NULL)
		memmove (ptr, ptr + 1, strlen (ptr + 1) + 1);
	
	if ((fp = fopen (URL2, "r")) == NULL) {
		if (donno == 1)
			L003 (nick, URL2);
		R;
	}
	while (fgets (b, STRING_LONG, fp)) {
		x++;
		stripline (b);
		subj = strtok (b, " ");
		if (subj == NULL)
			continue;
		ptr = strtok (NULL, "");
		if (ptr == NULL)
			continue;
		if (stricmp (subj, topic) == 0 || !match_wild (subj, topic) == 0) {
			QUESTIONS++;
			if (floodpro == 1)
				if (cf (uh, nick, nick)) {
					fclose (fp);
					R;
				}
				gotit = 1;

				// 21.jan.2008 Delay
				if (ptr[0] == '{' && ptr[1] == '{')
				{
				    char *pEnd = strstr(ptr, "}}");
				    if (pEnd)
				    {
					*pEnd = 0;
					//useconds_t mDelay = 10000000;
					mDelay = atoi(ptr+2);
					sec_sleep(mDelay);
					ptr = pEnd+2;
					//usleep(mDelay);
					//sleep(10000000);
//					sleep(10000);
					//pthread_delay_np(10000);
				    }
				}
				
				if (*ptr == '+') {
					ptr++;
					A = 1;
				}
				else if (*ptr == '-') {
					if (strstr (nick, "|") != NULL)
						R;
					if (no_pipe == 1) {
						fclose (fp);
						R;
					}
					ptr++;
					D = 1;
				}
				// 1.2007
				else if ((*ptr == '~') || (*ptr == '^')) {
					fclose (fp);
					rare = *ptr == '^';
					ptr++;
   
					do_randomtopic2 (target, ptr, nick, topic, rare?1:0);
					return;
				}
				else if (*ptr == '$') {
					ptr++;
					fclose (fp);
					do_pluginCall(target, ptr, nick, topic);
					return;
				}
				length = strlen (ptr);
				if (length > 3) {
					if (ptr[0] == 'i' && ptr[1] == 'l' && ptr[2] == 'c') {
						toggle++;
					}
				}
				while (length > 0) {
					length--;
					if (Tog == 1) {
						Tog = 0;
						if (ptr[length] == 'N') {     /* nick */
							toggle++;
#ifdef WIN32
							sprintf (temp,"%s%s", nick, Data);
						}
						else if (ptr[length] == 'C') {        /* chan */
							toggle++;
							sprintf (temp,"%s%s", target, Data);
#else
							snprintf (temp,sizeof(temp), "%s%s", nick, Data);
						}
						else if (ptr[length] == 'C') {        /* chan */
							toggle++;
							snprintf (temp,sizeof(temp), "%s%s", target, Data);
#endif
						}
						else
							if (ptr[length] == '1'
								|| ptr[length] == '2'
								|| ptr[length] == '3'
								|| ptr[length] == '4'
								|| ptr[length] == '5'
								|| ptr[length] == '6'
								|| ptr[length] == '7'
								|| ptr[length] == '8' || ptr[length] == '9') {
								toggle++;
								/* The first time around, we just store the topic in a
								"safe place" */
								if (bFirst == 1) {
									strncpy (crm1, topic, sizeof(crm1));
									bFirst = 0;
								}
								/* Each time around, get a new copy from the "safe place." */
								strncpy (crm2, crm1, sizeof(crm2));
#ifdef WIN32
								sprintf (temp,"%s%s", get_word (ptr[length], crm2),
									Data);
							}
							else if (ptr[length] == 'H') {        /* u@h of user */
								toggle++;
								sprintf (temp,"%s%s", uh, Data);
							}
							else if (ptr[length] == 'h') {        /* u@h (no ident) */
								toggle++;
								if (*uh == '~') {
									*uh++;
								}
								sprintf (temp,"%s%s", uh, Data);
							}
							else if (ptr[length] == 'T') {        /* time */
								toggle++;
								sprintf (temp,"%s%s", date (), Data);
							}
							else if (ptr[length] == 't') {        /* unixtime */
								toggle++;
								sprintf (temp,"%d%s", time (NULL), Data);
							}
							else if (ptr[length] == 'W') {        /* WWW page */
								toggle++;
								sprintf (temp,"http://darkbot.net%s", Data);
							}
							else if (ptr[length] == 'S') {        /* server */
								toggle++;
								sprintf (temp,"%s%s", BS, Data);
							}
#else
							snprintf (temp, sizeof(temp), "%s%s", get_word (ptr[length], crm2),
								Data);
					}
					else if (ptr[length] == 'H') {        /* u@h of user */
						toggle++;
						snprintf (temp, sizeof(temp), "%s%s", uh, Data);
					}
					else if (ptr[length] == 'h') {        /* u@h (no ident) */
						toggle++;
						if (*uh == '~') {
							*uh++;
						}
						snprintf (temp, sizeof(temp), "%s%s", uh, Data);
					}
					else if (ptr[length] == 'T') {        /* time */
						toggle++;
						snprintf (temp,sizeof(temp), "%s%s", date (), Data);
					}
					else if (ptr[length] == 't') {        /* unixtime */
						toggle++;
						snprintf (temp,sizeof(temp), "%d%s", time (NULL), Data);
					}
					else if (ptr[length] == 'W') {        /* WWW page */
						toggle++;
						snprintf (temp,sizeof(temp), "http://darkbot.net%s", Data);
					}
					else if (ptr[length] == 'S') {        /* server */
						toggle++;
						snprintf (temp,sizeof(temp), "%s%s", BS, Data);
					}
#endif
					else if (ptr[length] == 'R') {        /* rand */
						toggle++;
						/* The first time around, we just store the topic in a
						"safe place" */
						if (bFirst == 1) {
							strncpy (crm1, topic, sizeof(crm1));
							bFirst = 0;
						}
#ifdef Win32
						sprintf (temp,"%s%s", get_rand_nick (target),
							Data);
					}
					else if (ptr[length] == 'P') {        /* port */
						toggle++;
						sprintf (temp,"%d%s", (int) BP, Data);
					}
					else if (ptr[length] == 'Q') {        /* question */
						toggle++;
						sprintf (temp,"%s%s", revert_topic (topic),
							Data);
					}
					else if (ptr[length] == 'V') {        /* version */
						toggle++;
						sprintf (temp,"%s%s", dbVersion, Data);
					}
					else if (ptr[length] == '!') {        /* cmdchar */
						toggle++;
						sprintf (temp,"%c%s", *CMDCHAR, Data);
					}
					else if (ptr[length] == 'B') {        /* mynick */
						toggle++;
						sprintf (temp,"%s%s", Mynick, Data);
					}
					else
						sprintf (temp,"%c~%s", ptr[length], Data);
        }
        else if (ptr[length] == '~') {
			Tog = 1;
        }
        else
			sprintf (temp,"%c%s", ptr[length], Data);
#else
		snprintf (temp,sizeof(temp), "%s%s", get_rand_nick (target),
			Data);
          }
          else if (ptr[length] == 'P') {        /* port */
			  toggle++;
			  snprintf (temp,sizeof(temp), "%d%s", (int) BP, Data);
          }
          else if (ptr[length] == 'Q') {        /* question */
			  toggle++;
			  snprintf (temp,sizeof(temp), "%s%s", revert_topic (topic),
				  Data);
          }
          else if (ptr[length] == 'V') {        /* version */
			  toggle++;
			  snprintf (temp,sizeof(temp), "%s%s", dbVersion, Data);
          }
          else if (ptr[length] == '!') {        /* cmdchar */
			  toggle++;
			  snprintf (temp,sizeof(temp), "%c%s", *CMDCHAR, Data);
          }
          else if (ptr[length] == 'B') {        /* mynick */
			  toggle++;
			  snprintf (temp,sizeof(temp), "%s%s", Mynick, Data);
          }
          else
			  snprintf (temp,sizeof(temp), "%c~%s", ptr[length], Data);
        }
        else if (ptr[length] == '~') {
			Tog = 1;
        }
        else
			snprintf (temp,sizeof(temp), "%c%s", ptr[length], Data);
#endif
        strncpy (Data, temp, sizeof(Data));
      }                         /* While */
      if (D == 1) {
		  ptr8 = strtok (Data, "|");
		  while (ptr8 != NULL) {
			  if (ptr8[0] == ' ')
				  ptr8++;
			  if (ptr8[0] == 'B' && ptr8[1] == 'A' && ptr8[2] == 'N') {     /* ban user */
				  S ("MODE %s +b *%s\n", target, uh);
			  }
			  if (ptr8[0] == 'T' && ptr8[1] == 'E' && ptr8[2] == 'M' &&
				  ptr8[3] == 'P' && ptr8[4] == 'B' && ptr8[5] == 'A' &&
				  ptr8[6] == 'N') { /* temp ban user for 60 sec */
				  S ("MODE %s +b *%s\n", target, uh);
#ifdef WIN32
				  sprintf (temp,"%s/%d", DBTIMERS_PATH, time (NULL) + 60);
#else
				  snprintf (temp, sizeof(temp),"%s/%d", DBTIMERS_PATH, time (NULL) + 60);
#endif
				  fslog (temp, "MODE %s -b *%s\n", target, uh);
			  }
			  ptr8[0] = tolower (ptr8[0]);
			  ptr8[1] = tolower (ptr8[1]);
			  if (ptr8[0] == 'p' && ptr8[1] == 'r')
				  F = 1;
			  if (ptr8[0] == 'k' && ptr8[1] == 'i')
				  F = 1;
			  if (ptr8[0] == 'n' && ptr8[1] == 'o')
				  F = 1;
			  if (ptr8[0] == 't' && ptr8[1] == 'o')
				  F = 1;
			  if (ptr8[0] == 'm' && ptr8[1] == 'o')
				  F = 1;
			  if (F == 1)
				  S ("%s\n", ptr8);
			  F = 0;
			  ptr8 = strtok (NULL, "|");
		  }
		  fclose (fp);
		  R;
      }
      if (toggle == 0) {
		  if (A == 0) {
			  S ("PRIVMSG %s :%s%s\n", target, rand_reply (nick), Data);
		  }
		  else
			  S ("PRIVMSG %s :\1ACTION %s\1\n", target, Data);
      }
      else if (A == 0) {
		  
		  // Display single info2.db message
		  S ("PRIVMSG %s :%s\n", target, Data);
      }
      else
		  S ("PRIVMSG %s :\1ACTION %s\1\n", target, Data);
      fclose (fp);
      R;
    }                           /* Subject match */
  }
  fclose (fp);
  if (donno == 1) {
	  if (strlen (topic) > 3) {
		  strlwr (topic);
		  if (topic[0] == 'i' && topic[1] == 'l' && topic[2] == 'c') {
			  S
				  ("PRIVMSG %s :%s, I found no matching ILC for that channel.\n",
				  target, nick);
			  R;
		  }
	  }
	  // S ("PRIVMSG %s :%s, %s\n", target, nick, DONNO_Q);
	  mapString(nick);
	  do_randomtopic (target, DONNO_RDB, nick, Data);
  }
}

/**
* 6/23/00 Dan:
* - Moved declaration of newact into #ifdef
*/
int
main (int argc, char **argv)
{
	char temp[STRING_SHORT];
	struct timeval timeout;
	fd_set fdvar;
#if (SGI == 1) || (NEED_LIBC5 == 1)
	struct sigaction newact;
#endif
#ifdef  DEBUG
	DebuG = 1;
#endif
#ifndef WIN32                   /* not win32 */
#endif

bc_reset();
bc1_reset();

// 2017
start_telegram_reader();

	g_lnow = 0; //time(0)-RNICK_CACHE_TIME;
	get_s ();
	
	srand (time (0));
	
	uptime = time (NULL);
	if (argv[1] != NULL) {
		if (stricmp (argv[1], "-SEEN") == 0) {
			SeeN = 1;
			printf ("\nSEEN ENABLED.\n\n");
		}
		else if (stricmp (argv[1], "-DEBUG") == 0) {
			DebuG = 1;
			printf ("\nDEBUG ENABLED.\n\n");
		}
		else {
			printf ("\n\n%s HELP:\n\n", dbVersion);
			printf ("%s          (Launches Darkbot to IRC)\n", argv[0]);
			printf
				("%s -SEEN    (Enables SEEN [Even if SEEN is undefined])\n",
				argv[0]);
			printf ("%s -DEBUG   (Launch in debug mode)\n", argv[0]);
			exit (0);
		}
	}
	strncpy (DARKBOT_BIN, argv[0], sizeof(DARKBOT_BIN));
#if (SGI == 1) || (NEED_LIBC5 == 1)
	newact.sa_handler = sig_alrm;
	sigemptyset (&newact.sa_mask);
	newact.sa_flags = 0;
	sigaction (SIGALRM, &newact, NULL);
	newact.sa_handler = sig_segv;
	sigemptyset (&newact.sa_mask);
	newact.sa_flags = 0;
	sigaction (SIGSEGV, &newact, NULL);
	newact.sa_handler = sig_hup;
	sigemptyset (&newact.sa_mask);
	newact.sa_flags = 0;
	sigaction (SIGHUP, &newact, NULL);
#else /* ----------------------- */
	signal (SIGALRM, sig_alrm);
	signal (SIGSEGV, sig_segv);
	signal (SIGHUP, sig_hup);
#endif
#ifndef WIN32
#ifdef  FORK
	if (fork ())
		exit (0);
#endif
#endif
#ifdef  RANDOM_STUFF
	get_rand_stuff_time ();
#endif
/*
	printf ("\n\n\n\n\n");
	printf (" ,-----------------------------------------------,\n");
	printf (" | Darkbot (C) Jason Hamilton http://darkbot.net |\n");
	printf (" `-----------------------------------------------'\n\n");
*/
#ifndef WIN32
#ifndef DISALLOW_COUNT
	snprintf (temp,sizeof(temp), "lynx -source http://darkbot.net/cgi/laun.cgi?%s &",
		dbVersion);
	//system (temp);
#endif
	sprintf (temp,"echo \"%d\" > %s.pid", getpid (),
		DARKBOT_BIN);
	//system (temp);
#endif
#ifndef WIN32
	db_sleep (2);
#endif
#ifndef WIN32
#ifdef  SORT
	printf ("\nSorting database...\n");
	snprintf (temp, sizeof(temp), "sort %s -o %s\n", URL2, URL2);
	// system (temp);
#endif
#endif
	load_helpers ();
	raw_now ("SERVERS");
	raw_now ("SETUP");
	raw_now ("PERMBAN");
	gs26 ();
#ifndef WIN32
#endif
	alarm (AIL);
	
	num_male_nicks = 0;
	num_female_nicks = 0;
	
	loadKnownNames();
	
RETRY_CONN:
	
	printf ("\nConnecting to %s:%d ...\n", BS, (int) BP);
	socketfd = get_connection (BS, VHOST, BP);
	if (socketfd == -1) {
		//exit (0);
		printf("Socket error\n");
		db_sleep(30);
		goto RETRY_CONN;
	}
	init_bot ();
	while (1) {
		timeout.tv_sec = WSEC;
		timeout.tv_usec = USEC;
		FD_ZERO (&fdvar);
		FD_SET (socketfd, &fdvar);
		switch (select
            (NFDBITS, &fdvar, (fd_set *) 0, (fd_set *) 0, &timeout)) {
		case 0:
			break;
		case -1:
			if (!alarmed) {
				db_sleep (RECHECK);
			}
			else
				alarmed = 0;
			break;
		default:
			parse_server_msg (&fdvar);
			break;
		}
	}
	
	// EOL, now sleep and reconnect
	db_sleep(10);
	printf("Disconnected\n");
	goto RETRY_CONN;
}

void
sig_hup (int notUsed)
{
	char temp[STRING_LONG];
#ifdef  WIN32
	S ("QUIT :Window killed - %s terminating\n", dbVersion);
	sprintf (temp,"sleep 2; %s", DARKBOT_BIN);
#else
	S ("QUIT :SIGHUP - Restarting %s ...\n", dbVersion);
	snprintf (temp, sizeof(temp), "sleep 2; %s", DARKBOT_BIN);
#endif
	system (temp);
	sec_sleep (1);
//	db_sleep (1);
	exit (0);
}

void
sig_alrm (int notUsed)
{
	alarmed = 1;
	alarm (AIL);
	check_dbtimers ();            /* timers :) */
	AIL8 += AIL;
	if (AIL8 >= SEND_DELAY) {
		AIL8 = 0;
		Send ();
	}
	LastInput += AIL;
	if (LastInput >= 500) {
		LastInput = 0;
#if CHECK_STONED == 1
		L088 (BS);
#ifdef  WIN32
		printf ("\nNo responce from %s in 5 mins, reconnecting...\n", BS);
#endif
		gs26 ();
		socketfd = get_connection (BS, VHOST, BP);
		init_bot ();
#endif
	}
	AIL10 += AIL;
	if (AIL10 >= 900) {           /* 15 mins */
		AIL10 = 0;
		if (MARK_CHANGE == 1) {
			MARK_CHANGE = 0;
			save_setup ();            /* save settings */
		}
	}
	AIL666 += AIL;
	if (AIL666 >= 60) {           /* 60 sec timer */
		AIL666 = 0;
		S ("PING :%s\n", BS);
	}
	AIL9 += AIL;
	if (AIL9 >= 30) {
		AIL9 = 0;
		if (stricmp (s_Mynick, Mynick) != 0) {
			S ("NICK %s\n", s_Mynick);
			strncpy (Mynick, s_Mynick, sizeof(Mynick));
#ifdef WIN32
			sprintf (NICK_COMMA,"%s,", Mynick);
			sprintf (COLON_NICK,"%s:", Mynick);
			sprintf (BCOLON_NICK,"%s\2:\2", Mynick);
#else
			snprintf (NICK_COMMA,sizeof(NICK_COMMA), "%s,", Mynick);
			snprintf (COLON_NICK,sizeof(COLON_NICK), "%s:", Mynick);
			snprintf (BCOLON_NICK,sizeof(BCOLON_NICK), "%s\2:\2", Mynick);

			snprintf (ANICK_COMMA,sizeof(ANICK_COMMA), "%s,", MyAnick);
			snprintf (COLON_ANICK,sizeof(COLON_ANICK), "%s:", MyAnick);
			snprintf (BCOLON_ANICK,sizeof(BCOLON_ANICK), "%s\2:\2", MyAnick);
#endif
		}
	}
	if (Sleep_Toggle == 1) {
		AIL4 += AIL;
		if (AIL4 >= SLEEP_TIME) {
			Sleep_Toggle = 0;
			AIL4 = 0;
			L089 (sleep_chan);
		}
	}
	AIL2 += AIL;
	AIL3 += AIL;
#ifdef  RANDOM_STUFF
	Rand_Idle++;
	if (RAND_IDLE <= Rand_Idle) {
		Rand_Idle = 0;
		do_random_stuff ();
		get_rand_stuff_time ();
	}
	Rand_Stuff -= AIL;
	if (Rand_Stuff <= 0) {
		if (Sleep_Toggle != 1)
			do_random_stuff ();
		get_rand_stuff_time ();
	}
#endif
	if (AIL3 >= AUTOTOPIC_TIME) {
		AIL3 = 0;
		do_autotopics ();
	}
	AIL5 += AIL;
	if (AIL5 >= 600) {
#ifdef  ANTI_IDLE
		S ("PRIVMSG ! :\2\n");
#endif
		AIL5 = 0;
	}
	if (AIL2 >= 300) {
		AIL2 = 0;
#if STATUS == 1
		S ("LUSERS\n");
#endif
		S ("JOIN %s\n", CHAN);
		S ("MODE %s %s\n", Mynick, DEFAULT_UMODE);
		reset_ ();
		save_changes ();
	}
}

void
sig_segv (int notUsed)
{
	long uptime2 = 0, p = 0;
	uptime2 = time (NULL) - uptime;
	printf
		("ERROR! Aborting program. (SIG_SEGV) Uptime: %d hour%s, %d min%s\n",
		(int) (uptime2 / 3600),
		(uptime2 / 3600 == 1) ? "" : "s",
		(int) ((uptime2 / 60) % 60),
		((uptime2 / 60) % 60) == 1 ? "" : "s");
	Snow
		("QUIT :Caught SIG_SEGV! Aborting connection. Uptime: %d hour%s, %d min%s\n",
		uptime2 / 3600,
		uptime2 / 3600 == 1 ? "" : "s",
		(uptime2 / 60) % 60, (uptime2 / 60) % 60 == 1 ? "" : "s");
	db_sleep (2);
	p = getpid ();
	if (fork () > 0) {
		fslog ("error.log",
			"Caught SIGSEGV.. Sent kill -3 and kill -9...\n");
		kill (p, 3);
		kill (p, 9);
	}
	db_sleep (1);
	exit (0);
}

int
get_sendq_count (long toggle)
{
	struct sendq *c;
	long i = 0, x = 0;
	c = sendqhead;
	while (c != NULL) {
		i++;
		if (c->data[0] == 'P' && c->data[1] == 'R' && c->data[2] == 'I')
			x++;
		else
			if (c->data[0] == 'N'
				&& c->data[1] == 'O' && c->data[2] == 'T') x++;
			c = c->next;
	}
	if (toggle == 1)
		clear_sendq (i, 1);
	if (toggle == 2)
		R i;
	if (i < OUTPUT1_COUNT)
		SEND_DELAY = OUTPUT1_DELAY;
	else if (i < OUTPUT2_COUNT)
		SEND_DELAY = OUTPUT2_DELAY;
	else
		SEND_DELAY = OUTPUT3_DELAY;
	if (x > OUTPUT_PURGE_COUNT)
		clear_sendq (x, 0);
	R i;
}

void
clear_sendq (long count, long toggle)
{
	long i = 0;
	i = count;
	while (i > 1) {
		i--;
		del_sendq (1);
	}
	send_tog = 1;
	if (toggle != 1)
		L090 (CHAN, count);
}

void
init_bot ()
{
	get_sendq_count (1);
	Snow ("NICK %s\n", Mynick);
	strlwr (UID);
	Snow ("USER %s %d %d :%s\n", UID,
        time (NULL), time (NULL), REALNAME);
}

/**
* TODO: No function should ever be this long...move to the Command Pattern
*/
void
parse (char *line)
{
	char *s, *s1, *s2, *s3, *cmd, *ptr, *s4;
	long TOG = 0, seen_value = 0;
	LastInput = 0;
	if (DebuG == 1)
		printf ("IN :%s\n", line);
#ifdef  DEBUG2
	fslog ("darkbot_debug.log", "IN :%s\n", line);
#endif
	stripline (line);
	s = strtok (line, " ");
	if (stricmp (s, "PING") == 0) {
		s1 = strtok (NULL, " ");
		Snow ("PONG %s\n", s1);
	}
	else if (stricmp (s, "ERROR") == 0) {
		s1 = strtok (NULL, "");
		if (s1 != NULL) {
			if (strstr (s1, "Excess Flood") != NULL) {
				socketfd = get_connection (BS, VHOST, BP);
				init_bot ();
			}
			else if (strstr (s1, "throttled") != NULL) {
				gs26 ();
				socketfd = get_connection (BS, VHOST, BP);
				init_bot ();
			}
			else if (strstr (s1, "oo many c") != NULL) {
				gs26 ();
				socketfd = get_connection (BS, VHOST, BP);
				init_bot ();
			}
			else if (strstr (s1, "o more c") != NULL) {
				gs26 ();
				socketfd = get_connection (BS, VHOST, BP);
				init_bot ();
			}
			else {
				S ("QUIT :Caught ERROR from %s :(\n", BS);
				db_sleep (5);
				gs26 ();
				socketfd = get_connection (BS, VHOST, BP);
				init_bot ();
			}
		}
	}
	else if (strstr (s, "!") == NULL) {   /* From Server */
		cmd = strtok (NULL, " ");
		if (stricmp (cmd, "004") == 0) {    /* Connected! */
			save_changes ();
			s2 = strtok (NULL, " ");  /* Copy the current nick */
			strncpy (Mynick, s2, sizeof(Mynick));
#ifdef WIN32
			sprintf (NICK_COMMA, "%s,", Mynick);
			sprintf (COLON_NICK,"%s:", Mynick);
			sprintf (BCOLON_NICK,"%s\2:\2", Mynick);
#else
			snprintf (NICK_COMMA,sizeof(NICK_COMMA), "%s,", Mynick);
			snprintf (COLON_NICK,sizeof(COLON_NICK), "%s:", Mynick);
			snprintf (BCOLON_NICK,sizeof(BCOLON_NICK), "%s\2:\2", Mynick);

			snprintf (ANICK_COMMA,sizeof(ANICK_COMMA), "%s,", MyAnick);
			snprintf (COLON_ANICK,sizeof(COLON_ANICK), "%s:", MyAnick);
			snprintf (BCOLON_ANICK,sizeof(BCOLON_ANICK), "%s\2:\2", MyAnick);
#endif
			S ("JOIN %s\n", CHAN);
			db_sleep (2);
			s2 = strtok (NULL, " ");  /* Got server name */
			printf
				("%s has connected to %s! [%d pid]\n", Mynick, s2, getpid ());}
		else if (stricmp (cmd, "315") == 0) {
#if DISPLAY_SYNC == 1
			s2 = strtok (NULL, " ");  /*mynick */
			strncpy (Mynick, s2, sizeof(Mynick));
			s2 = strtok (NULL, " ");  /* chan */
			S ("PRIVMSG %s :Sync with %s completed.\n", s2, s2);
#endif
		}
		else if (stricmp (cmd, "311") == 0) {
			s1 = strtok (NULL, " ");
			s1 = strtok (NULL, " ");
			s1 = strtok (NULL, " ");
			s1 = strtok (NULL, " ");
			strncpy (g_host, s1, sizeof(g_host));
		}
		else if (stricmp (cmd, "319") == 0) {
			s1 = strtok (NULL, " ");
			s1 = strtok (NULL, " ");
			s2 = strtok (NULL, "");
			if (*s2 == ':')
				s2++;
			strlwr (s2);
			if (strstr (s2, "arez") != NULL)
				TOG = 1;
			if (strstr (s2, "kidd") != NULL)
				TOG = 1;
			if (strstr (s2, "hack") != NULL)
				TOG = 1;
			if (strstr (s2, "sex") != NULL)
				TOG = 1;
			if (strstr (s2, "fuck") != NULL)
				TOG = 1;
			if (strstr (s2, "porn") != NULL)
				TOG = 1;
			if (strstr (s2, "pic") != NULL)
				TOG = 1;
			if (TOG == 1) {
				S ("NOTICE @%s :%s is on \2%s\2\n", g_chan, s1, s2);
				R;
			}
		}
		else
			if (stricmp (cmd, "432") == 0
				|| stricmp (cmd, "468") == 0) {  /* Invalid nick/user */
				s2 = strtok (NULL, "");
				printf ("Server Reported error %s\n\nDarkbot exiting.\n", s2);
				db_sleep (2);
				exit (0);
			}
			else if (stricmp (cmd, "376") == 0) {
			raw_now ("PERFORM");      /* End of MOTD, run
			* perform_now() */
			}
			else if (stricmp (cmd, "482") == 0) {
#if BITCH_ABOUT_DEOP == 1
				s2 = strtok (NULL, " ");  /* mynick */
				strncpy (Mynick, s2, sizeof(Mynick));
				s2 = strtok (NULL, " ");  /* chan */

// 2006
				if (nrand(5) == 2)
				    do_randomtopic(s2, "gimmeops", Mynick, s2);
				    
				//S ("PRIVMSG %s :%s\n", s2, BITCH_DEOP_REASON);
///
				
#endif
				raw_now ("DEOP");         /* Deoped, run list of
				* commands.. */
			}
			else if (stricmp (cmd, "352") == 0) {
				s2 = strtok (NULL, "");
				parse_who (s2);
#if STATUS == 1
			}
			else if (stricmp (cmd, "252") == 0) {
				s2 = strtok (NULL, "");
				parse_252 (s2);
			}
			else
				if (stricmp (cmd, "404") == 0
					|| stricmp (cmd, "475") == 0
					|| stricmp (cmd, "474") == 0 || stricmp (cmd, "473") == 0) {  /* Can't join? */
					s2 = strtok (NULL, " ");
					s2 = strtok (NULL, " ");
					db_sleep (5);
					S ("JOIN %s\n", s2);
				}
				else if (stricmp (cmd, "251") == 0) {
					s2 = strtok (NULL, "");
					parse_251 (s2);
				}
				else if (stricmp (cmd, "255") == 0) {
					s2 = strtok (NULL, "");
					parse_255 (s2);
#endif
				}
				else if (stricmp (cmd, "433") == 0) {
					s2 = strtok (NULL, " ");
					if (*s2 != '*') {
						strncpy (Mynick, s2, sizeof(Mynick));
#ifdef WIN32
						sprintf (NICK_COMMA, "%s,", Mynick);
						sprintf (COLON_NICK,"%s:", Mynick);
						sprintf (BCOLON_NICK,"%s\2:\2", Mynick);
#else
						snprintf (NICK_COMMA,sizeof(NICK_COMMA), "%s,", Mynick);
						snprintf (COLON_NICK,sizeof(COLON_NICK), "%s:", Mynick);
						snprintf (BCOLON_NICK,sizeof(BCOLON_NICK), "%s\2:\2", Mynick);

						snprintf (ANICK_COMMA,sizeof(ANICK_COMMA), "%s,", MyAnick);
						snprintf (COLON_ANICK,sizeof(COLON_ANICK), "%s:", MyAnick);
						snprintf (BCOLON_ANICK,sizeof(BCOLON_ANICK), "%s\2:\2", MyAnick);
#endif
						s3 = strtok (NULL, " ");
					}
					else {
						Snow ("NICK %s%d\n", Mynick, xtried);
						xtried++;
						if (xtried > 15)
							Snow ("NICK _`^%s%d\n", Mynick, xtried);
						if (xtried > 5)
							Snow ("NICK _%s%d\n", Mynick, xtried);
					}
				}
  }
  else {                        /* Info from user */
	  if (*s == ':')              /* Remove the colon prefix */
		  s++;
	  cmd = strtok (NULL, " ");   /* Read in command  */
	  if (stricmp (cmd, "NOTICE") == 0) {
		  s2 = strtok (NULL, " ");  /* target */
#if KICK_ON_CHANNEL_NOTICE == ON
		  if (*s2 == '#') {
			  s3 = strtok (s, "!");
#if BAN_ON_CHANNEL_NOTICE == ON
#if BAN_BY_HOST == ON
			  s4 = strtok (NULL, "@");
			  s4 = strtok (NULL, "");
			  S ("MODE %s +b *!*@%s\n", s2, s4);
#else /* ban just by u@h */
			  S ("MODE %s +b *%s\n", s2, strtok (NULL, ""));
#endif
#endif
			  S ("KICK %s %s :Punt\n", s2, s3);
		  }
#endif
	  }
	  else if (stricmp (cmd, "PRIVMSG") == 0) {   /* PRIVMSG  */
		  //trace("Channel message:");
		  //trace(s);
		  //trace("\n");
		  s1 = strtok (NULL, " ");  /* Target */
		  s2 = strtok (NULL, "");   /* Rest  */
#if     LOG_PRIVMSG == 1
		  if (*s1 != '#' && *s1 != '&') {
			  fslog (privmsg_log, "[%s] %s %s %s\n", date (), s, s1, s2);
		  }
#endif

// Telegram echo - 2017
// tgecho.sh
/*
if (!strcmp(s1, "#irc"))
{
    S ("PRIVMSG ncuxonycbka :<%s> %s\n", s, s2);
    char temp1[10240];
    snprintf (temp1,sizeof(temp1), "/home/olivka/bot/tgecho.sh \"<%s> %s\"", s, s2);
    const char *res = run_program(temp1);
}
*/
///


		  if (*s1 == '#' || *s1 == '&' || *s1 == '+')
			  if (do_lastcomm (s, s1, s2)
				  == 1)
				  R;
			  if (!my_chanserv (s, s1, s2))     /* Process spec. PRIVMSG commands */
			  chanserv (s, s1, s2);     /* Process PRIVMSG commands */
	  }
	  else if (stricmp (cmd, "KILL") == 0) {
		  s1 = strtok (NULL, " ");  /* Kill nick */
		  if (stricmp (s1, Mynick) == 0) {
			  do_quit (s1, 3);        /* delete all users from ram since I'm gone */
			  gs26 ();
			  socketfd = get_connection (BS, VHOST, BP);
			  init_bot ();
		  }
	  }
	  else if (stricmp (cmd, "KICK") == 0) {
		  s1 = strtok (NULL, " ");  /* #chan */
		  s2 = strtok (NULL, " ");  /* Who got kicked? */
									if (stricmp (s2, Mynick) == 0) {  /* Rejoin if I was
										* kicked */
										do_quit (s1, 2);
										db_sleep (5);
										S ("JOIN %s\n", s1);
										S ("PRIVMSG %s :%s\n", s1, COMPLAIN_REASON);
									}
									else
									{
										delete_user (s2, s1);
										s3 = strtok (NULL, " ");
										verify_kick_protect(s2, s1, s, s3);
									}
	  }
	  else if (stricmp (cmd, "INVITE") == 0) {
		  s1 = strtok (NULL, " ");  /* Mynick */
		  s2 = strtok (NULL, " ");  /* Target */
		  if (*s2 == ':')
			  s2++;
		  if (stricmp (s2, CHAN) == 0)
			  S ("JOIN %s\n", s2);
	  }
	  else if (stricmp (cmd, "PART") == 0) {
		  s1 = strtok (NULL, " ");  /* Target */
		  if ((ptr = strchr (s, '!')) != NULL)
			  *ptr++ = '\0';
		  strlwr (ptr);
		  if (stricmp (s, Mynick) != 0)
			  delete_user (s, s1);
		  else                      /* I left, so delete all */
			  do_quit (s1, 2);
	  }
	  else if (stricmp (cmd, "QUIT") == 0) {
		  if ((ptr = strchr (s, '!')) != NULL)
			  *ptr++ = '\0';
		  do_quit (s, 1);
	  }
	  else if (stricmp (cmd, "MODE") == 0) {
		  {
			  //S ("NOTICE zloy [%s]\n", s);
			  //trace("Mode change:");
			  //trace(s);
			  //trace("\n");
			  do_modes (s, strtok (NULL, ""));
		  }
	  }
	  else if (stricmp (cmd, "NICK") == 0) {
		  if ((ptr = strchr (s, '!')) != NULL)
			  *ptr++ = '\0';
		  s1 = strtok (NULL, " ");
		  process_nick (s, s1);
	  }
	  else if (stricmp (cmd, "JOIN") == 0) {
		  JOINs++;
		  s1 = strtok (NULL, " ");  /* TARGET */
		  if (*s1 == ':')
			  s1++;
		  if ((ptr = strchr (s, '!')) != NULL)
			  *ptr++ = '\0';
		  strlwr (ptr);
		  if (SeeN == 1 && *s1 == '#')
			  seen_value = save_seen (s, ptr, s1);
		  if (stricmp (s, Mynick) != 0) {
			  if (check_permban (ptr, s1, s) == 1)
				  R;
			  add_user (s1, s, ptr, 1);
#if DO_WHOIS == 1
			  strncpy (g_chan, s1, sizeof(g_chan));
			  S ("WHOIS %s\n", s);
#endif
			  if (check_access (ptr, s1, 0, s) >= 4)
			  {
				  if (inv_vars[INTV_AOP])
					  S("MODE %s +o %s\n", s1, s);
			  }
			  else
			  {
				  int aLevel = check_access(ptr, s1, 1, s);
				  
				  // Admin Users
				  if (aLevel >= 9)
				  {
					  if (inv_vars[INTV_AOP] && inv_vars[INTV_AVOICE])
						  S("MODE %s +vho %s %s %s\n", s1, s, s, s);
//						  S("MODE %s +vhoa %s %s %s %s\n", s1, s, s, s, s);
					  else if (inv_vars[INTV_AOP])
						  S("MODE %s +ho %s %s\n", s1, s, s);
//						  S("MODE %s +hoa %s %s %s\n", s1, s, s, s);
					  else if (inv_vars[INTV_AVOICE])
						  S("MODE %s +vh %s %s\n", s1, s, s);
				  }
				  // Moderator Users
				  if (aLevel >= 5)
				  {
					  if (inv_vars[INTV_AOP] && inv_vars[INTV_AVOICE])
						  S("MODE %s +vo %s %s %s\n", s1, s, s, s);
					  else if (inv_vars[INTV_AOP])
						  S("MODE %s +o %s %s\n", s1, s, s);
					  else if (inv_vars[INTV_AVOICE])
						  S("MODE %s +v %s\n", s1, s);
				  }
				  // Oper Users
				  if (aLevel >= 4)
				  {
					  if (inv_vars[INTV_AOP] && inv_vars[INTV_AVOICE])
						  S("MODE %s +vo %s %s\n", s1, s, s);
					  else if (inv_vars[INTV_AOP])
						  S("MODE %s +h %s\n", s1, s);
					  else if (inv_vars[INTV_AVOICE])
						  S("MODE %s +v %s\n", s1, s);
				  }
				  else
					  if (aLevel >= 1 && inv_vars[INTV_AVOICE])
						  S ("MODE %s +v %s\n", s1, s);
#if HELP_GREET == 1
			  }
			  else if (check_access (ptr, s1, 0, s) >= 1) {
			  return;               /* don't greet if the guy has
			  * access (and no setinfo) */
			  }
			  else if (stricmp (s1, CHAN) == 0) {
				  if (SeeN == 1) {
				  if (seen_value == 0)        /* don't show people the
					  * notice every join */
					  if (setinfo_lastcomm (s1) == 0)   /* don't do it if you just did it! */
						  L102 (s, s1, s, *CMDCHAR);
				  }
				  else {
					  if (setinfo_lastcomm (s1) == 0)
					  {
						  if (inv_vars[INTV_GREET])
							  L102 (s, s1, s, *CMDCHAR);
					  }
				  }
			  }
#else
		  }
#endif
      }
      else
	  S ("WHO %s\n", s1);     /* Check who is in the
	  * chan */
    }                           /* JOIN */
  }
}


long
setinfo_lastcomm (char *rest)
{                               /* Disallows joins to more than one channel (or the same chan)
	* to artifically raise join counters */
	long c_uptime = 0;
	if (stricmp (rest, slc1) == 0)
		R 1;                        /* don't reply if already asked in LASTCOMM_TIME sec */
	if (stricmp (rest, slc2) == 0)
		R 1;
	if (stricmp (rest, slc3) == 0)
		R 1;
	if (stricmp (rest, slc4) == 0)
		R 1;
	if (*slc1 == '0') {           /* init lastcomms */
		strncpy (slc1, rest, sizeof(slc1));
		slcn1 = time (NULL);
	}
	if (*slc2 == '0') {
		strncpy (slc2, rest, sizeof(slc2));
		slcn2 = time (NULL);
	}
	if (*slc3 == '0') {
		strncpy (slc3, rest, sizeof(slc3));
		slcn3 = time (NULL);
	}
	if (*slc4 == '0') {
		strncpy (slc4, rest, sizeof(slc4));
		slcn4 = time (NULL);
	}
	if ((c_uptime = time (NULL) - slcn1) > SLASTCOMM_TIME) {      /* reinit old data */
		slcn1 = 0;
		*slc1 = '0';
	}
	if ((c_uptime = time (NULL) - slcn2) > SLASTCOMM_TIME) {
		slcn2 = 0;
		*slc2 = '0';
	}
	if ((c_uptime = time (NULL) - slcn3) > SLASTCOMM_TIME) {
		slcn3 = 0;
		*slc3 = '0';
	}
	if ((c_uptime = time (NULL) - slcn4) > SLASTCOMM_TIME) {
		slcn4 = 0;
		*slc4 = '0';
	}
	strncpy (slc4, slc3, sizeof(slc4));           /* no matches, move em on
	down */
	strncpy (slc3, slc2, sizeof(slc3));
	strncpy (slc2, slc1, sizeof(slc2));
	strncpy (slc1, rest, sizeof(slc1));
	slcn4 = slcn3;
	slcn3 = slcn2;
	slcn2 = slcn1;
	slcn1 = time (NULL);
	R 0;
}

long
do_lastcomm (char *nick, char *target, char *rest)
{
	long c_uptime = 0;
	if (stricmp (rest, lc1) == 0)
		R 1;                        /* don't reply if already asked in LASTCOMM_TIME sec */
	if (stricmp (rest, lc2) == 0)
		R 1;
	if (stricmp (rest, lc3) == 0)
		R 1;
	if (stricmp (rest, lc4) == 0)
		R 1;
	if (*lc1 == '0') {            /* init lastcomms */
		strncpy (lc1, rest, sizeof(lc1));
		lcn1 = time (NULL);
	}
	if (*lc2 == '0') {
		strncpy (lc2, rest, sizeof(lc2));
		lcn2 = time (NULL);
	}
	if (*lc3 == '0') {
		strncpy (lc3, rest, sizeof(lc3));
		lcn3 = time (NULL);
	}
	if (*lc4 == '0') {
		strncpy (lc4, rest, sizeof(lc4));
		lcn4 = time (NULL);
	}
	if ((c_uptime = time (NULL) - lcn1) > LASTCOMM_TIME) {        /* reinit old data */
		lcn1 = 0;
		*lc1 = '0';
	}
	if ((c_uptime = time (NULL) - lcn2) > LASTCOMM_TIME) {
		lcn2 = 0;
		*lc2 = '0';
	}
	if ((c_uptime = time (NULL) - lcn3) > LASTCOMM_TIME) {
		lcn3 = 0;
		*lc3 = '0';
	}
	if ((c_uptime = time (NULL) - lcn4) > LASTCOMM_TIME) {
		lcn4 = 0;
		*lc4 = '0';
	}
	strncpy (lc4, lc3, sizeof(lc4));              /* no matches, move em on
	down */
	strncpy (lc3, lc2, sizeof(lc3));
	strncpy (lc2, lc1, sizeof(lc2));
	strncpy (lc1, rest, sizeof(lc1));
	lcn4 = lcn3;
	lcn3 = lcn2;
	lcn2 = lcn1;
	lcn1 = time (NULL);
	R 0;
}

#if STATUS == 1
void
parse_252 (char *s)
{
	char *tmp;
	int numb = 0;
	tmp = strtok (s, " ");
	tmp = strtok (NULL, " ");
	sscanf (tmp, "%d", &numb);
	IRCOPS = numb;
}
#endif

#if STATUS == 1
void
parse_251 (char *s)
{
	char *tmp;
	int numb = 0, r = 0, i = 0;
	/*- Read and chuck useless data from line 'b' -*/
	tmp = strtok (s, " ");
	tmp = strtok (NULL, " ");
	tmp = strtok (NULL, " ");
	tmp = strtok (NULL, " ");
	sscanf (tmp, "%d", &r);
	tmp = strtok (NULL, " ");
	tmp = strtok (NULL, " ");
	tmp = strtok (NULL, " ");
	sscanf (tmp, "%d", &i);
	tmp = strtok (NULL, " ");
	tmp = strtok (NULL, " ");
	tmp = strtok (NULL, " ");
	sscanf (tmp, "%d", &numb);
	NUM_SERV = numb;
	G_USERS = r + i;
}
#endif

#if STATUS == 1
void
parse_255 (char *s)
{
	char *tmp, Stat[1];
	int numb = 0, pre_CLIENTS = 0;
	/* test321 :I have 1313 clients and 1 servers */
	strlwr (s);
	tmp = strtok (s, " ");
	tmp = strtok (NULL, " ");
	tmp = strtok (NULL, " ");
	tmp = strtok (NULL, " ");
	numb = atoi (tmp);
	pre_CLIENTS = L_CLIENTS;
	L_CLIENTS = numb;
	if (L_CLIENTS < pre_CLIENTS) {
		strncpy (Stat, "-", sizeof(Stat));
		pre_CLIENTS = pre_CLIENTS - L_CLIENTS;
	}
	else {
		strncpy (Stat, "+", sizeof(Stat));
		pre_CLIENTS = L_CLIENTS - pre_CLIENTS;
	}
#ifdef WIN32
	sprintf (tmp,"%3.2f",
#else
		snprintf (tmp, sizeof(tmp),"%3.2f",
#endif
		(float) (((float) L_CLIENTS / (float) G_USERS) * 100));
#if PLAY == 1
	if (pre_CLIENTS == 0 || pre_CLIENTS == L_CLIENTS) {
		S
			("PRIVMSG %s :!SENDQ %d srvs, %d ops, %d users (%s%% of %d, %ld avg)\n",
			PBOT, NUM_SERV, IRCOPS, L_CLIENTS, tmp,
			G_USERS, G_USERS / NUM_SERV);}
	else
		S
		("PRIVMSG %s :!SENDQ %d srvs, %d ops, %d users [%c%2d] (%s%% of %d, %ld avg)\37\n",
		PBOT, NUM_SERV, IRCOPS, L_CLIENTS,
		Stat[0], pre_CLIENTS, tmp, G_USERS, G_USERS / NUM_SERV);
	fslog (".ubcount", "%d\n%d\n0\n0\n", L_CLIENTS, L_CLIENTS);
	rename (".ubcount",
		"/usr/local/apache/htdocs/usage/userbase/userbase.dat");
	fslog (".glcount", "%d\n%d\n0\n0\n", G_USERS, G_USERS);
	rename (".glcount",
		"/usr/local/apache/htdocs/usage/global/global.dat");
#else
	if (pre_CLIENTS == 0 || pre_CLIENTS == L_CLIENTS) {
		S
			("PRIVMSG %s :\1ACTION \37(\37%2d servers\37)\37: %2d opers + \2%4d\2 users \37(\37%s%% %5d global \2!\2 %3ld avg\37)\37\1\n",
			CHAN, NUM_SERV, IRCOPS, L_CLIENTS, tmp,
			G_USERS, G_USERS / NUM_SERV);}
	else
		S
		("PRIVMSG %s :\1ACTION \37(\37%2d servers\37)\37: %2d opers + \2%4d\2 users [\37%c%2d\37] \37(\37%s%% %5d global \2!\2 %3ld avg\37)\37\1\n",
		CHAN, NUM_SERV, IRCOPS, L_CLIENTS,
		Stat[0], pre_CLIENTS, tmp, G_USERS, G_USERS / NUM_SERV);
#endif
}
#endif

void
load_helpers ()
{
	FILE *fp;
	char b[STRING_LONG], *user_host,
		*greetz, *numb_join, *chan, *w_level, *pass;
	long num_join = 0, i = 0, level = 0;
	if ((fp = fopen (HELPER_LIST, "r")) == NULL) {
		printf ("Unable to open %s! Aborting connection.\n", HELPER_LIST);
		printf ("Please run ./configure to setup your darkbot.\n");
		exit (0);
	}
#ifndef WIN32
	printf ("Loading %s file ", HELPER_LIST);
#endif
	while (fgets (b, STRING_LONG, fp)) {
		if (b == NULL)
			continue;
		stripline (b);
		if (*b == '/')
			continue;
		i++;
		printf (".");
		fflush (stdout);
		chan = strtok (b, " ");
		if (chan == NULL)
			continue;
		user_host = strtok (NULL, " ");
		if (user_host == NULL)
			continue;
		w_level = strtok (NULL, " ");
		if (w_level == NULL)
			continue;
		numb_join = strtok (NULL, " ");
		if (numb_join == NULL)
			continue;
		pass = strtok (NULL, " ");
		if (pass == NULL) {
			pass = "0";               /* duh */
		}
		greetz = strtok (NULL, "");
		if (greetz == NULL)
			greetz = ":)";
//			greetz = "I haven't used \2SETINFO\2 yet!";
		if (w_level != NULL)
			level = atoi (w_level);
		else
			level = 1;
		if (numb_join != NULL)
			num_join = atoi (numb_join);
		else
			num_join = 0;
		if (strlen (pass) > 25)
			pass[25] = '\0';
		if (DebuG == 1)
			printf
			("loading helperlist: %s %s l:%d j:%d %s\n",
			chan, user_host, (int) level, (int) num_join, greetz);
		add_helper (chan, user_host, level, num_join, greetz, pass);
	}
	printf ("done(%d).\n", (int) i);
	fclose (fp);
	save_changes ();
	if (DebuG == 1)
		db_sleep (2);
}

/**
* Add a channel helper.
* 6/22/00 Dan
* n now initialized where declared
* All pointer arguments now received as pointer to const data.
*/
void
add_helper (const char *chan,
            const char *uh, long level,
            size_t num_join, const char *greetz, const char *pass)
{
	struct helperlist *n = 0;
	n = (struct helperlist *)
		malloc (sizeof (struct helperlist));
	if (n == NULL) {
		fslog ("error.log", "AHHH! No ram left! in add_helper!\n");
		R;
	}
	
	memset (n, 0, sizeof (struct helperlist));
	NUM_HELPER++;
	if (chan[0] == '#') {
		strncpy (n->chan, chan, sizeof(n->chan));
	}
	else {
		strncpy (n->chan, "#*", sizeof(n->chan));
	}
	
	strncpy (n->uh, uh, sizeof(n->uh));
	strlwr (n->uh);
	strncpy (n->pass, pass, sizeof(n->pass));
	n->num_join = num_join;
	n->level = level;
	strncpy (n->greetz, greetz,
		min (sizeof (n->greetz) - 1, strlen (greetz)));
	n->next = helperhead;
	helperhead = n;
}


/**
* 6/22 Dan
* - Changed DATA to be 512 bytes, a power of 2
* - DATA now initialized properly
* - c is now a pointer to const, this is a read only method
* - c is now initialized where declared
* - Changed type of i, x to size_t, these variables should be
*   unsigned.
* - Changed while loop to for loop.
* - Changed reinitialization of DATA to use memset()
*/
void
show_banlist (const char *nick)
{
	char DATA[STRING_SHORT] = {
		0
	};
	size_t i = 0;
	size_t x = 0;
	const struct permbanlist *c = 0;
	for (c = permbanhead; c != NULL; c = c->next) {
		i++;
		++x;
#ifdef WIN32
		sprintf (DATA,"%s %s:%d", DATA, c->uh, (int)
#else
			snprintf (DATA,sizeof(DATA), "%s %s:%d", DATA, c->uh, (int)
#endif
			c->counter);
		if (i > 8) {
			S ("NOTICE %s :%s\n", nick, DATA);
			i = 0;
			memset (DATA, 0, sizeof (DATA));
			db_sleep (2);
		}
	}
	S ("NOTICE %s :%s\n", nick, DATA);
	S
		("NOTICE %s :End of PERMBAN list; %d ban%s found.\n",
		nick, x, (x == 1) ? "" : "s");
}

/**
* Output the helper list to a nickname.
* 6/22, Dan:
* - Changed helperlist* c to be a pointer to const data
* - Changed initialization of DATA, and size to be a
*   power of 2
* - Added initialization of c
* - Changed while loop to for loop
* - Changed types of i, x to size_t since they should be
*   unsigned.
* - Added reinitialization of DATA using memset() (changed from
*   strcpy(DATA,""))
*/
void
show_helper_list (const char *nick, long level)
{
	char DATA[STRING_SHORT] = {
		0
	};
	size_t i = 0, x = 0;
	const struct helperlist *c = 0;
	for (c = helperhead; c != NULL; c = c->next) {
		i++;
		x++;
		if (level == 0) {
			i++;
#ifdef   LINUX
			sprintf (DATA, "%s %s[%s:%d:%d]", DATA, c->uh, c->chan,
				c->level, c->num_join);
#else
			snprintf (DATA, sizeof(DATA), "%s %s[%s:%d:%d]", DATA,
                c->uh, c->chan, c->level,  c->num_join);
#endif
		}
		else {
			if (level == c->level) {
#ifdef   LINUX
				sprintf (DATA, "%s %s[%s:%d:%d]", DATA, c->uh, c->chan,
					c->level, c->num_join);
#else
				snprintf (DATA, sizeof(DATA), "%s %s[%s:%d:%d]", DATA,
					c->uh, c->chan, c->level, c->num_join);
#endif
			}
		}
		if (i > 6) {
			S ("NOTICE %s :%s\n", nick, DATA);
			i = 0;
			memset (DATA, 0, sizeof (DATA));
			db_sleep (2);
		}
	}                             /* for() */
	
	S ("NOTICE %s :%s\n", nick, DATA);
	S
		("NOTICE %s :End of Helper Userlist; %d user%s found.\n",
		nick, x, (x == 1) ? "" : "s");
}

int
match_wild (const char *pattern, const char *str)
{
	char c;
	const char *s;
	for (;;) {
		switch (c = *pattern++) {
		case 0:
			if (!*str)
				R 1;
			R 0;
		case '?':
			++str;
			break;
		case '*':
			if (!*pattern)
				R 1;
			s = str;
			while (*s) {
				if (*s == *pattern && match_wild (pattern, s))
					R 1;
				++s;
			}
			break;
		default:
			if (*str++ != c)
				R 0;
			break;
		}                           /* switch */
	}
}


#ifndef WIN32
int
stricmp (const char *s1, const char *s2)
{
	return strcasecmp (s1, s2);
}
#else
int
stricmp (const char *s1, const char *s2)
{
	register int c;
	while ((c = tolower (*s1)) == tolower (*s2)) {
		if (c == 0)
			return 0;
		s1++;
		s2++;
	}
	if (c < tolower (*s2))
		return -1;
	return 1;
}
#endif


/**
* Removed trailing newline and carriage returns.
* 6/22/00 Dan
* Rewrote to be more efficient, reduced from O(2n) to O(n)
*/
void
stripline (char *ptr)
{
	for (; ptr && *ptr; ++ptr) {
		if ('\r' == *ptr || '\n' == *ptr) {
			*ptr = 0;
			return;
		}
	}
}


int
Send ()
{
	struct sendq *c;
	char output[STRING_LONG];
	c = sendqhead;
	get_sendq_count (0);
	if (c == NULL) {
		send_tog = 0;
		R - 1;
	}
	if (DebuG == 1)
		printf ("OUT: %s\n", c->data);
#ifdef  DEBUG2
	fslog ("darkbot_debug.log", "OUT: %s\n", c->data);
#endif
	strncpy (output, c->data, sizeof(output));
	del_sendq (0);
	R (writeln (output));
}


/**
* Write a character array to a socket connection.
* 6/22/00 Dan
* Method argument now pointer to const data.
*/
int
writeln (const char *b)
{
	return (write (socketfd, b, strlen (b))
		< 0) ? 0 : 1;
}


int
get_connection (const char *hostname, const char *vhostname, int port)
{
	struct sockaddr_in sa;
	struct hostent *hp;
	int sckfd, f = 1;
	if (vhostname == NULL)
		f = 0;
	else if (strlen (vhostname) < 1)
		f = 0;
	if ((sckfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
		R (-1);
	memset (&sa, 0, sizeof (struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = (f ? inet_addr (vhostname) : INADDR_ANY);
	if ((INADDR_NONE == sa.sin_addr.s_addr)
		&& f) {
		hp = gethostbyname (vhostname);
		if (hp) {
			bcopy (hp->h_addr, (char *) &sa.sin_addr, hp->h_length);
		}
		else {
			sa.sin_addr.s_addr = INADDR_ANY;
		}
	}
	if (bind
		(sckfd, (struct sockaddr *) &sa,
		sizeof (struct sockaddr_in)) < 0) {
		R (-9);
	}
	memset (&sa, 0, sizeof (struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_port = htons (port);
	sa.sin_addr.s_addr = inet_addr (hostname);
	setsockopt (sckfd, SOL_SOCKET, SO_LINGER, 0, 0);
	setsockopt (sckfd, SOL_SOCKET, SO_REUSEADDR, 0, 0);
	setsockopt (sckfd, SOL_SOCKET, SO_KEEPALIVE, 0, 0);
	if (INADDR_NONE == sa.sin_addr.s_addr) {
		if ((hp = gethostbyname (hostname)) == NULL) {
			errno = ECONNREFUSED;
			printf ("Can't find hostname: %s\n", hostname);
			R (-1);
		}
		memcpy (&sa.sin_addr, hp->h_addr, hp->h_length);
	}
	if (connect (sckfd, (struct sockaddr *) &sa, sizeof (sa)) < 0) {
		close (sckfd);
		printf
			("\nUnable to make connection to host `%s:%d'\n\n",
			BS, (int) BP); R (-1);
	}
	R (sckfd);
}

int
readln ()
{
	char ch;
	int i = 0;
	do {
		if (read (socketfd, &ch, 1) < 1)
			R (0);
		if (ch >= 32 || ch <= 126)
			if (i < 524 - 1)
				L[i++] = ch;
	}
	while (ch != '\n');
	L[i] = '\0';
	R 1;
}

void
fslog (const char *filename, const char *format, ...)
{
	va_list arglist;
	FILE *fp;
	fp = fopen (filename, "a");
	if (NULL == fp) {
		/* I guess there's no sense in trying to log the error :) */
		return;
	}
	
	va_start (arglist, format);
	vfprintf (fp, format, arglist);
	va_end (arglist);
	fclose (fp);
}

/**
* Convert a character array to all lowercase.
* 6/23/00 Dan:
* - Rewrote to be more compact and a bit more efficient
*/
char *
strlwr (char *buf)
{
	char *ptr = buf;
	for (; ptr && *ptr; ++ptr) {
		*ptr = tolower (*ptr);
	}
	return buf;
}

void
trailing_blanks (char *str)
{
	int i;
	if (str == NULL)
		R;
	for (i = strlen (str); i > 0; i--) {
		if (isspace (str[i - 1]))
			str[i - 1] = '\0';
		else
			R;
	}
}


void
parse_server_msg (fd_set * read_fds)
{
	if (FD_ISSET (socketfd, read_fds)) {
		if (readln () > 0) {
			NUMLINESSEEN++;
			parse (L);
		}
		else {
			close (socketfd);
		}
	}
}


long
f_f (char *host)
{
	int i;
	for (i = 0; i < fc; i++)
		if (!strcasecmp (ood[i].host, host))
			R i;
		R - 1;
}

void
get_s ()
{
	char temp[20];
	long i = 0;
	i = strlen (rp391);
	while (i > 0) {
		i--;
#ifdef WIN32
		sprintf (temp,"%s%c", dbVersion, rp391[i]);
#else
		snprintf (temp,sizeof(temp), "%s%c", dbVersion, rp391[i]);
#endif
		strncpy (dbVersion, temp, sizeof(dbVersion));
	}
}

void
a_f (char *host)
{
	if (++fc > 100)
		fc = 0;
	fc--;
	strncpy (ood[fc].host, host, sizeof(ood[fc].host));
	ood[fc].time = time (NULL);
	ood[fc].count = 0;
	ood[fc].value = false;
	fc++;
}

void
reset_ ()
{
	int i;
	for (i = 0; i < fc; i++) {
		if (ood[i].value && (time (NULL) - ood[i].time) > rt) {
			ood[i].count = 0;
			ood[i].time = time (NULL);
			ood[i].value = false;
			ood[i].kick = 0;
		}
	}
}

char *
date ()
{
    strncpy (strbuff, "date \'+%''D %''B %''e, %''A %''T\'\n", sizeof(strbuff));
    char *res = (char *)run_program(strbuff);
    return res;
	/*
	time_t timer;
	time (&timer);
	strncpy (strbuff, ctime (&timer), sizeof(strbuff));
	stripline (strbuff);
	return strbuff;
	*/
}

/**
* Allocate a new character array.  Copy into it at most
* maxBytes bytes.
*/
char *
db_strndup (const char *dupMe, size_t maxBytes)
{
	char *ptr = 0;
	char *retMe = 0;
	/* Configure maxBytes to be the number of bytes to copy */
	maxBytes = min (strlen (dupMe), maxBytes);
	/* Allocate the return buffer. */
	retMe = (char *) malloc (maxBytes + 1);
	/* Was the allocation successful? */
	if (NULL == retMe) {
		return NULL;
	}
	
	/*
	* ptr will point to the byte to which to copy the next
	* source byte.
	*/
	ptr = retMe;
	/*
	* Continue while dupMe is valid and we are < maxBytes number
	* of bytes copied. This is typecase here because size_t is
	* unsigned, so comparing against > 0 *should* produce a
	* warning :)
	*/
	while (dupMe && (int) maxBytes-- > 0) {
		*ptr++ = *dupMe++;
	}
	
	/* Be sure to NULL terminate the array */
	*ptr = 0;
	return retMe;
}

/**
* Output information about the bot's database to a target.
* 6/22 Dan:
* - Changed both method arguments to be pointers to const data,
*   this is a read only method.
*/
void
show_info2 (const char *target, const char *source)
{
	S
		("PRIVMSG %s :%s, src: %s (%d lines of code), compiled @ %s. "
		"I have processed %ld lines of text since startup...\n",
		target, source, __FILE__, __LINE__, __DATE__, NUMLINESSEEN);
}

// <zloy>
void resetNames()
{
	int i;
	
	for (i = 0; i < num_male_nicks; i++)
		free(male_nicks[i]);
	
	for (i = 0; i < num_female_nicks; i++)
		free(female_nicks[i]);
	
	num_male_nicks = 0;
	num_female_nicks = 0;
}

void loadKnownNames()
{
	FILE *fp;
	char buffer[1024];
	
	resetNames();
	
	if ((fp = fopen (MALE_NICKS, "r")) != NULL)
	{
		while (fgets(buffer, STRING_LONG, fp))
		{
			int nickLen;
			stripline(buffer);
			nickLen = strlen(buffer);
			if (nickLen)
				addNick(buffer, nickLen, MALE);
		}
		fclose(fp);
	}
	
	if ((fp = fopen (FEMALE_NICKS, "r")) != NULL)
	{
		while (fgets(buffer, STRING_LONG, fp))
		{
			int nickLen;
			stripline(buffer);
			nickLen = strlen(buffer);
			if (nickLen)
				addNick(buffer, nickLen, FEMALE);
		}
		fclose(fp);
	}
}

void addNick(char *buffer, int bufLength, int nType)
{
	if ((nType == MALE) && (num_male_nicks < MAX_NICK-1) && (bufLength > 0))
	{
		male_nicks[num_male_nicks] = (char *)malloc(bufLength+1);
		strncpy(male_nicks[num_male_nicks], buffer, bufLength+1);
		num_male_nicks++;
	}
	else
		if ((nType == FEMALE) && (num_female_nicks < MAX_NICK-1) && (bufLength > 0))
		{
			female_nicks[num_female_nicks] = (char *)malloc(bufLength+1);
			strncpy(female_nicks[num_female_nicks], buffer, bufLength+1);
			num_female_nicks++;
		}
}

int getNickType(char *nick)
{
	int i;
	
	for (i = 0; i < num_male_nicks; i++)
		if (!strcmp(male_nicks[i], nick))
			return MALE;
		
		for (i = 0; i < num_female_nicks; i++)
			if (!strcmp(female_nicks[i], nick))
				return FEMALE;
			
			return UNKNOWN;
}

void trace(char *text)
{
	fslog("error.log", text);	
}

int nrand(int max)
{
    return (int)(rand()/(RAND_MAX + 1.0)*max) + 1;
}

bool checkSex(char *buffer, int bufLength, char *nick)
{
	int sex = getNickType(nick);
	char *fPtr = strstr(buffer, SEX_SIGN);
	int offset;
	
	if (fPtr != NULL)
		fPtr[0] = '\x0';
	
	if ((sex == UNKNOWN) || (fPtr == NULL) || (fPtr - buffer >= bufLength + 2))
		return false;
	
	if (sex == FEMALE)
	{
		offset = bufLength - (fPtr-buffer);
		memcpy(buffer, fPtr+2, offset);
	}
	
	
	return true;
}

void do_topicstring(char *_b, char *b2, bool Tog, char *nick, char *target, char *topic)
{
	char temp[STRING_SHORT] = { 0 };
	char Data[STRING_LONG] = { 0 };
	
	long x = 0;
	long y = 0;
	
	char *b = _b;
	int mDelay = 0;
	
	size_t i = 0;
	size_t length = 0;
	
	length = strlen (b);
	
#ifdef	PARSE_NICKNAMES
	checkSex(b2, length, nick);
#endif

// 21.jan.2008 Delay value ( {{xxxsec}}text )
	//
	
	if (b[0] == '{' && b[1] == '{')
	{
	    char *pEnd = strstr(b, "}}");
	    if (pEnd)
	    {
		*pEnd = 0;
		mDelay = atoi(b+2);
		b = pEnd+2;
		sec_sleep(mDelay);
	    }
	}

	char assNick[1024] = "you";
	bool doAss = (inv_vars[INTV_ASSNICK] > 0 && (nrand(10) > 7));

	if (doAss)
	{
		if (!assholize(nick, assNick))
		{
			// inv_vars[INTV_ASSNICK] = 0;
			doAss = false;
		}
	}

	
	length = strlen (b);
	
	while (length > 0)
	{
		length--;
		if (Tog) {
			Tog = false;
			
			if (b[length] == 'N') { 	  /* nick */
				snprintf (temp, sizeof(temp), "%s%s", (doAss? assNick:nick), Data);
			}
			else if (b[length] == 'C') {  /* chan */
				snprintf (temp, sizeof(temp), "%s%s", target, Data);
			}
			else if (b[length] == 'T') {  /* time */
				snprintf (temp, sizeof(temp), "%s%s", date (), Data);
			}
			else if (b[length] == 'R') {  /* rand */
				snprintf (temp, sizeof(temp), "%s%s", get_rand_nick (target),
					Data);
			}
			else if (b[length] == 'S') {  /* serv */
				snprintf (temp, sizeof(temp), "%s%s", BS, Data);
			}
			else if (b[length] == 'P') {  /* port */
				snprintf (temp, sizeof(temp), "%d%s", (int) BP, Data);
			}
			else if (b[length] == 'Q') {  /* question */
				snprintf (temp,sizeof(temp), "%s%s", revert_topic (topic),
					Data);
			}
			else if (b[length] == 'W') {  /* WWW page */
				snprintf (temp,sizeof(temp), "http://darkbot.net%s", Data);
			}
			else if (b[length] == '!') {  /* cmdchar */
				snprintf (temp, sizeof(temp), "%c%s", *CMDCHAR, Data);
			}
			else if (b[length] == 'V') {  /* version */
				snprintf (temp,sizeof(temp), "%s%s", dbVersion, Data);
			}
			else if (b[length] == 'B') {  /* mynick */
				snprintf (temp, sizeof(temp),"%s%s", Mynick, Data);
			}
			else {
				snprintf (temp,sizeof(temp), "%c~%s", b[length], Data);
			}
		}
		else if (b[length] == '~') {
			Tog = true;
		}
		else {
			snprintf (temp, sizeof(temp),"%c%s", b[length], Data);
		}
		strncpy (Data, temp, sizeof(Data));
	}								/* While */
	
	parseMultiLine(target, Data);
	
	/*
	if (A == 0) {
	S ("PRIVMSG %s :%s\n", target, Data);
	}
	else {
	S ("PRIVMSG %s :\1ACTION %s\1\n", target, Data);
	}
	*/
}

void parseMultiLine(char *channel, char *data)
{
	char *chr = data;
	char *start = data;
	bool isAction;
	
	while (*chr)
	{
		chr++;
		
		if ((*chr == '|') || (*chr == 0))
		{
			bool isBreak = (*chr == '|');
			*chr = 0;
			
			isAction = *start == '+';
			
			if (isAction)
				S ("PRIVMSG %s :\1ACTION %s\1\n", channel, ++start);
			else
				S ("PRIVMSG %s :%s\n", channel, start);
			
			if (isBreak)
				chr++;
			
			start = chr;
		}
	}
}

bool mapString(char *text)
{
	int setLength = sizeof(chr_map_eng)/sizeof(char);
	int textLength = strlen(text);
	int i, c;
	char s[1024];
	
	for (i = 0; i < textLength; i++)
	{
		for (c = 0; c < setLength; c++)
		{
			if (text[i] == chr_map_eng[c])
			{
				text[i] = chr_map_rus[c];
				return true;
			}
			if (text[i] == chr_map_rus[c])
			{
				text[i] = chr_map_eng[c];
				return true;
			}
		}
	}
	
	return false;
}

char *wt_tokenize(char *source)
{
	int sourceLen = strlen(source);
	int cnt = 0;
	char *res;
	
	while (cnt++ < sourceLen && source[cnt] != ' ' && source[cnt] != '\t' && source[cnt] != 0);
	
	if (cnt >= sourceLen)
		return NULL;
	
	source[cnt] = 0;
	
	while (cnt++ < sourceLen && (source[cnt] == ' ' || source[cnt] == '\t'));
	
	if (cnt >= sourceLen)
		return NULL;
	
	return source+cnt;
}

char *wt_tokenize2(char **source2)
{
	char *source = *source2;
	int sourceLen = strlen(source);
	int cnt = 0;
	
	while (cnt++ < sourceLen && source[cnt] == ' ');

	if (*source == '\"')
	{
		(*source2)++;
		while (cnt++ < sourceLen && source[cnt] != '\"' && source[cnt] != 0);
	}
	else
		while (cnt++ < sourceLen && source[cnt] != ' ' && source[cnt] != '\t' && source[cnt] != 0);
	
	if (cnt >= sourceLen)
		return NULL;
	
	source[cnt] = 0;
	
	while (cnt++ < sourceLen && (source[cnt] == ' ' || source[cnt] == '\t'));
	
	if (cnt >= sourceLen)
		return NULL;
	
	return source+cnt;
}

char *wt_trimRight(char *source)
{
	if (!source)
		return NULL;
	
	int len = strlen(source);
	while (len > 0 && (source[len-1] == ' ' || source[len-1] == '\t')) source[--len] = 0;
	return source;
}

void do_pluginCall(char *target, char *file, char *nick, char *topic)
{
	char command[1024], line[1024];
	char *pCommand = command;
	char *victim;
	DATE_INFO diSet[OUTPUT_PURGE_COUNT];
	int numItems = 0, i;
	
	strncpy(command, file, 1024);
	command[1023] = 0;
	
	strlwr(command);
	trim(&pCommand);
	
	// "Victorina
	if (!strcmp(pCommand, "viktorina") || !strcmp(pCommand, "victorina"))
	{
		if (inv_vars[INTV_VIKTORINA])
		{
			S ("PRIVMSG #olivka :Game: [%s]\n", topic);			

			char *pq = strstr(topic, "+#");
			if (!pq) return;
			pq = strstr(pq+2, ":03+");
			if (!pq) return;
			pq += 5;
			char *pas = strstr(pq, "+букв07");
			if (!pas) return;
			*pas = 0;
			do
			{
			--pas;
			}
			while (*pas >= '0' && *pas <= '9' && pas > pq);
			*pas = 0;
			pas++;
			int npas = atoi(pas);
			int pqs = strlen(pq);
			for (int i = 0; i < pqs; i++) if (pq[i] == '+') pq[i] = ' ';
			trim2(&pq);
			//do_randomtopic(target, "dateprefix3", nick, topic);
			char pansw[512] = "N/A";
			bool res = findAbyQ(npas, pq, pansw, true);                                                                                                        

			S ("PRIVMSG #olivka :Game Q: {%s} (%d l.) A: {%s}\n", pq, npas, pansw);			
		        
			if (res)
			{                                                                
			    usleep(rand()/6);
			    S ("PRIVMSG %s :%s\n", target, pansw);
			}
		}
		
	       // S ("PRIVMSG %s :%s\n", target, line);
	}
	else
	// "Ну что?"
	if (!strcmp(pCommand, "dateinfo") || !strcmp(pCommand, "dateinfo2"))
	{
		if (!inv_vars[INTV_SOWHAT])
		{
			do_randomtopic(target, "dateprefix3", nick, topic);
			return;
		}
		
		bool full = (!strcmp(pCommand, "dateinfo2"));
		
		numItems = getDateRecords(diSet, OUTPUT_PURGE_COUNT, 0);
		
		if (numItems)
		{
			if (numItems > 1)
			{
				do_randomtopic(target, (char *)(numItems > 3?"dateprefix1":"dateprefix2"), nick, topic);
				
				for (i = 0; i < numItems; i++)
				{
					sprintf(line, "%d. %s", i+1, diSet[i].text); 
					S ("PRIVMSG %s :%s\n", target, line);
					if (!full && i >=4) break;
				}
				if (numItems > 5 && !full)
				{
					int numLeft = numItems-5;
					if (numItems-5 > 1)
						S ("PRIVMSG %s :... %d. ...\n", target, numItems);
					else
						S ("PRIVMSG %s :...\n", target);
				}
			}
			else
			{
				do_randomtopic(target, "dateprefix2", nick, topic);
				
				sprintf(line, "Сегодня \2%s", diSet[0].text); 
				S ("PRIVMSG %s :%s\n", target, line);
			}
		}
		else
			do_randomtopic(target, "dateprefix3", nick, topic);
	}
	else

// 10/2006
	// "Ну что конкретно?"
	if (!strcmp(pCommand, "dateinfoex"))
	{
		if (!inv_vars[INTV_SOWHAT])
		{
			do_randomtopic(target, "dateprefix3", nick, topic);
			return;
		}
		
		numItems = getDateRecords(diSet, OUTPUT_PURGE_COUNT, 0);
		
		if (numItems > 0)
		{
			if (numItems > 1)
			{
				do_randomtopic(target, (char *)(numItems > 3?"dateprefix1":"dateprefix2"), nick, topic);
				
				for (i = 0; i < numItems; i++)
				{
					sprintf(line, "%d. [%d] %s (\2%s\2, %s)", i+1, diSet[i].id, diSet[i].text, diSet[i].nick, nocrlf(ctime(&diSet[i].created))); 
					S ("PRIVMSG %s :%s\n", target, line);
				}
			}
			else
			{
				do_randomtopic(target, "dateprefix2", nick, topic);
				
				sprintf(line, "[%05d] Сегодня \2%s (\2%s\2, %s)", diSet[0].id, diSet[0].text, diSet[0].nick, nocrlf(ctime(&diSet[0].created))); 
				S ("PRIVMSG %s :%s\n", target, line);
			}
		}
		else
			do_randomtopic(target, "dateprefix3", nick, topic);
	}
	else
//
		
// 02/2007
	// "Ну сколько?"
	if (!strcmp(pCommand, "datecnt"))
	{
		if (!inv_vars[INTV_SOWHAT])
		{
			do_randomtopic(target, "dateprefix3", nick, topic);
			return;
		}
		
		numItems = getDateRecords(diSet, OUTPUT_PURGE_COUNT, 0);
		
		if (numItems)
		    S ("PRIVMSG %s :%s, \2%d\2 :)\n", target, nick, numItems);
		else
		    do_randomtopic(target, "dateprefix3", nick, topic);
		return;
	}
	
	else
//
		
// МАТЮГИ
/*
		if (!strcmp(pCommand, "bcx"))
		{
		    bc_update(nick, topic);
		}
	else
*/
		if (!strcmp(pCommand, "bci"))
		{
		    char RRR[4096];
		    bc_info5(RRR);
		    S ("PRIVMSG %s :%s, %s\n", target, nick, RRR);
		}
	else
		if (!strcmp(pCommand, "bci10"))
		{
		    char RRR[4096];
		    bc1_info10(RRR);
		    S ("PRIVMSG %s :%s, %s\n", target, nick, RRR);
		}
	else
		if (!strcmp(pCommand, "bcf"))
		{
		    char RRR[4096];
		    bc_info5_f(RRR);
		    S ("PRIVMSG %s :%s, %s\n", target, nick, RRR);
		}
	else
		if (!strcmp(pCommand, "bcuser"))
		{
		    char RRR[4096];
		    bc_infoU(topic, RRR);
		    S ("PRIVMSG %s :%s, %s\n", target, nick, RRR);
		}
	else

		if (!strcmp(pCommand, "bci1"))
		{
		    char RRR[4096];
		    bc1_info5(RRR);
		    S ("PRIVMSG %s :%s, %s\n", target, nick, RRR);
		}
	else
		if (!strcmp(pCommand, "bci101"))
		{
		    char RRR[4096];
		    bc_info10(RRR);
		    S ("PRIVMSG %s :%s, %s\n", target, nick, RRR);
		}
	else
		if (!strcmp(pCommand, "bcf1"))
		{
		    char RRR[4096];
		    bc1_info5_f(RRR);
		    S ("PRIVMSG %s :%s, %s\n", target, nick, RRR);
		}
	else
		if (!strcmp(pCommand, "bcuser1"))
		{
		    char RRR[4096];
		    bc1_infoU(topic, RRR);
		    S ("PRIVMSG %s :%s, %s\n", target, nick, RRR);
		}
	else
		if ((!strcmp(pCommand, "sun")))
		{
			    char temp[2048];
			  snprintf (temp,sizeof(temp), "/home/olivka/bot/sun.sh");
			  const char *res = run_program(temp);
			  int ll2to = strlen(res);
			  if (ll2to > 10)
			  {
				S("PRIVMSG %s :\2%s\2, %s\n", target, nick, res);
			  }
			    else
			{
			    do_randomtopic(target, "fuckoff", nick, topic);
			}
			
		}
	else
//
		
		// weather
		if (!strcmp(pCommand, "weather"))
		{
		  S ("PRIVMSG %s :%s, %s\n", target, nick, run_program ("/home/olivka/bot/weather.sh"));
		} 
else
		if (!strcmp(pCommand, "wfcast"))
		{
		  S ("PRIVMSG %s :%s, %s\n", target, nick, run_program ("/home/olivka/bot/pywapi/tomorrow.sh"));
		}
	else
//
		
		// "кикбаннах!"
		if (!strcmp(pCommand, "cbn"))
		{
			if (!inv_vars[INTV_KICK])
			{
				do_randomtopic(target, "no_cbn", nick, topic);
				return;
			}
			
			for (i = 0; i < 10; i++)
			{
				victim = get_rand_nick(target);
				
				if (strcmp(victim, "olivka"))
				{
					if (strcmp(victim, nick))
					{
					    char kickMsg[1024];
					    get_randomtopic(target, "kick_psalter", nick, topic, kickMsg);
					    int kl = strlen(kickMsg);
					    if (kl > 10)
					    {
						S("KICK %s %s :%s Кикбаннах!\n", target, victim, kickMsg);						
					    }
					    else
						S("KICK %s %s :\2кикбаннах!\2 by %s\n", target, victim, nick);						
						
					    S("INVITE %s %s\n", victim, target);
					}
					else
					{
					    char kickMsg[1024];
					    get_randomtopic(target, "kick_psalter", nick, topic, kickMsg);
					    int kl = strlen(kickMsg);
					    if (kl > 10)
					    {
						if (kickMsg[kl-1] == '.')
						    kickMsg[kl-1] = 0;
						S("KICK %s %s :%s\n", target, victim, kickMsg);						
					    }
					    else
						S("KICK %s %s :аффтарский \2кикбаннах!\2\n", target, victim);						
						
					    //S("KICK %s %s :%s\n", target, victim, kickMsg);
					    //S("KICK %s %s :аффтарский \2кикбаннах!\2\n", target, victim);
					}
					break;
				}
			}
		}
		
}

void showDateDirect(char *channel, char *nick, time_t time)
{
	char line[1024];
	DATE_INFO diSet[32];
	int numItems = 0, i;
	
	numItems = getDateRecords(diSet, 32, time);
	
	if (numItems)
	{
		if (numItems > 1 && numItems < 100)
		{
			for (i = 0; i < numItems; i++)
			{
				sprintf(line, "%d. [\2%d\2] %s  (\2%s\2, %s)", i+1, diSet[i].id, diSet[i].text, diSet[i].nick, nocrlf(ctime(&diSet[i].created))); 
				S ("PRIVMSG %s :%s\n", channel, line);
			}
		}
		else
		{
			S ("PRIVMSG %s :%s, [%d] %s  (\2%s\2, %s)\n", channel, nick, diSet[0].id, diSet[0].text, diSet[0].nick, nocrlf(ctime(&diSet[0].created)));
		}
	}
	else
		S ("PRIVMSG %s :%s, no records found.\n", channel, nick);
}


bool assholize(char *from, char *to)
{
	char valChars[6] = { 'o', 'O', 'о', 'О', '0', 'i' };
	int i, i1, cnt = 0;
	int nLength = strlen(from);
	bool isValid;
	bool isDone = false;
	
	for (i = 0; i < nLength; i++)
	{
		isValid = false;
		
		for (i1 = 0; i1 < sizeof(valChars); i1++)
		{
			if (from[i] == valChars[i1])
			{
				isValid = true;
				break;
			}
		}
		
		if (isValid)
		{
			to[cnt++] = '(';
			to[cnt++] = '_';
			to[cnt++] = from[i];
			to[cnt++] = '_';
			to[cnt++] = ')';
			
			isDone = true;
		}
		else
			to[cnt++] = from[i];
	}
	
	to[cnt++] = 0;
	
	return isDone;
}

bool puppylize(char *from)
{
	char valChars[4] = { 'a', 'A', 'а', 'А' };
	int i, i1;
	int nLength = strlen(from);
	bool isValid;
	bool isDone = false;
	
	for (i = 0; i < nLength; i++)
	{
		isValid = false;
		
		for (i1 = 0; i1 < 4; i1++)
		{
			if (from[i] == valChars[i1])
			{
				isValid = true;
				break;
			}
		}
		
		if (isValid)
		{
			from[i] = '@';
			isDone = true;
		}
	}
	
	return isDone;
}


void upperCase2(char *text) { case2(text, 1); }
void lowerCase2(char *text) { case2(text, 2); }

void case2(char *text, int mode)
{
	char lChr[] = "abcdefghijklmnopqrstuvwxyzабвгдеёжзийклмнопрстуфхцчшщьыъэюя";
	char uChr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯ";
	
	int i, i1;
	
	int tLen = strlen(text);
	int sLen = strlen(lChr);
	
	for (i = 0; i < tLen; i++)
	{
		for (i1 = 0; i1 < sLen; i1++)
		{
			if (mode == 1 && text[i] == lChr[i1])
				text[i] = uChr[i1];
			if (mode == 2 && text[i] == uChr[i1])
				text[i] = lChr[i1];
		}
	}
}

int varIdByName(char *variable)
{
	char ucVar[4096];
	char *pUcVar = ucVar;
	
	int i;
	
	strncpy(ucVar, variable, 4095);
	upperCase2(ucVar);
	trim(&pUcVar);
	
	for (i = 0; i < MAX_INT_VARS; i++)
	{
		if (!strcmp(pUcVar, inv_var_names[i]))
			return i;
	}
	
	return -1;
}

void getIntVar(char *channel, char *nick, char *variable)
{
	char ucVar[4096];
	char *pUcVar = ucVar;
	int varId;
	
	strncpy(ucVar, variable, 4095);
	upperCase2(ucVar);
	varId = varIdByName(ucVar);
	trim(&pUcVar);
	
	if (varId == -1)
	{
		S("NOTICE %s :$%s is not defined.\n", channel, ucVar);
		return;
	}
	
	S("NOTICE %s :$%s is \2%s\2\n", channel, pUcVar, inv_vars[varId]==1?"on":"off");
}

void setIntVar(char *channel, char *nick, char *variable, char *value)
{
	char ucVar[4096];
	char ucValue[4096];
	char *pUcValue = ucValue;
	
	int varId;
	bool varOk = false;
	
	if (value == NULL)
	{
		S("NOTICE %s :usage: set <variable> [on/off]\n", nick, ucVar);
		return;
	}
	
	strncpy(ucVar, variable, 4095);
	strncpy(ucValue, value, 4095);
	
	upperCase2(ucValue);
	
	varId = varIdByName(ucVar);
	
	if (varId == -1)
	{
		S("NOTICE %s :$%s is not defined.\n", nick, ucVar);
		return;
	}
	
	trim(&pUcValue);
	
	if (!strcmp(pUcValue, "ON") || !strcmp(pUcValue, "TRUE"))
	{
		if (inv_vars[varId] <= 0)
		{
			inv_vars[varId] = 1;
			S("NOTICE %s :$%s is now \2on\2\n", channel, inv_var_names[varId]);
		}
		else
			S("NOTICE %s :$%s is already \2on\2\n", nick, inv_var_names[varId]);
	}
	else
		if (!strcmp(pUcValue, "OFF") || !strcmp(pUcValue, "FALSE"))
		{
			if (inv_vars[varId] > 0)
			{
				inv_vars[varId] = 0;
				S("NOTICE %s :$%s is now \2off\2\n", channel, inv_var_names[varId]);
			}
			else
				S("NOTICE %s :$%s is already \2off\2\n", nick, inv_var_names[varId]);
		}
}

void verify_kick_protect(char *s2, char *s1, char *s, char *s3)
{
	char *nick, *ps;
	FILE *fProtList;
	char line[1024], _s[1024], *pline = line;
	
	strncpy(_s, s, 1023);
	ps = strstr(_s, "!");
	
	if (ps == NULL)
		return;
	
	*ps = 0;
	
	if (!strcmp(Mynick, _s))
		return;
	
	fProtList = fopen(KICK_PROTECT_FILE, "rt");
	
	if (!fProtList)
	{
		S("NOTICE %s :File not found: \2%s\n", MASTER_NICK, KICK_PROTECT_FILE);
		return;
	}
	
	while (fgets(line, 1023, fProtList))
	{
		trim(&pline);
		
		if (!strcmp(line, s2))
		{
			S("NOTICE %s :punishing %s (%s)\n", MASTER_NICK, s, s2);
			S("KICK %s %s :%s\n", s1, _s, KICK_REVENGE);
		}
	}
	
	fclose(fProtList);
}


int t_topekInfo(char *channel, char *nick)
{
    time_t now = time(0);
    time_t tpk = now;
    struct tm *t = localtime(&tpk);
    t->tm_hour = 19;
    t->tm_min = 0;
    t->tm_sec = 0;
    time_t tpk2 = mktime(t);
	
    int diff = 0;
    int dDiff = 5 - t->tm_wday;
    if (dDiff < 0)
		dDiff += 7;
    int tDiff = tpk2 - tpk;
    if (tDiff < 0)
    {
		dDiff += 6;
        tDiff = 86400 + tDiff;
    }
	
    int hDiff = tDiff/3600;
    int mDiff = (tDiff-hDiff*3600)/60;
    int sDiff = tDiff-hDiff*3600-mDiff*60;
    S("PRIVMSG %s :%s, %dd %d:%02d:%02d\n", channel, nick, dDiff, hDiff, mDiff, sDiff);
    return 0;
}

int t_topegInfo(char *channel, char *nick)
{
    int tm_1_wp = 18;
    int rm_2_mb = 19;
    int rm_2_bp = 20;
    time_t now = time(0);
    time_t tpk = now;
    struct tm *t = localtime(&tpk);
    char ts[3][128];

    for (int i = 18; i <= 20; i++)
    {
        t->tm_hour = i;
	t->tm_min = 0;
        t->tm_sec = 0;
	time_t tpk2 = mktime(t);
        int tDiff = tpk2 - tpk;
	if (tDiff < 0)
        {
    	    //S("PRIVMSG %s :%s, топег уже идет\n", channel, nick);
	    sprintf(ts[i-18], "топег уже идет");
    	    //return 0;
	    continue;
	}

	int hDiff = tDiff/3600;
	int mDiff = (tDiff-hDiff*3600)/60;
	int sDiff = tDiff-hDiff*3600-mDiff*60;
	sprintf(ts[i-18], "\2%d:%02d:%02d\2%s", hDiff, mDiff, sDiff, (i == 20? "+":""));
    }
    S("PRIVMSG %s :%s, белые люди: %s, полукровки: %s, афроамериканцы: %s\n", channel, nick, ts[0], ts[1], ts[2]);
    return 0;
}

int t_glotokInfo(char *channel, char *nick)
{
    int tm_1_wp = 18;
    int rm_2_mb = 19;
    int rm_2_bp = 20;
    time_t now = time(0);
    time_t tpk = now;
    struct tm *t = localtime(&tpk);
    char ts[3][128];

    for (int i = 18; i <= 20; i++)
    {
        t->tm_hour = i;
	t->tm_min = 0;
        t->tm_sec = 0;
	time_t tpk2 = mktime(t);
        int tDiff = tpk2 - tpk;
	if (tDiff < 0 && i >= 19)
        {
    	    S("PRIVMSG %s :%s, уже давно пора\n", channel, nick);
    	    return 0;
	    continue;
	}

	int hDiff = tDiff/3600;
	int mDiff = (tDiff-hDiff*3600)/60;
	int sDiff = tDiff-hDiff*3600-mDiff*60;
	sprintf(ts[i-18], "\2%d:%02d\2", hDiff, mDiff);
    }
    S("PRIVMSG %s :%s, до глотка осталось %s\n", channel, nick, ts[1]);
    return 0;
}

int t_glotokInfoUa(char *channel, char *nick)
{
    int tm_1_wp = 18;
    int rm_2_mb = 19;
    int rm_2_bp = 20;
    time_t now = time(0);
    time_t tpk = now;
    struct tm *t = localtime(&tpk);
    char ts[3][128];

    for (int i = 18; i <= 20; i++)
    {
        t->tm_hour = i;
	t->tm_min = 0;
        t->tm_sec = 0;
	time_t tpk2 = mktime(t);
        int tDiff = tpk2 - tpk;
	if (tDiff < 0 && i >= 19)
        {
    	    S("PRIVMSG %s :%s, вже давно пора\n", channel, nick);
    	    return 0;
	    continue;
	}

	int hDiff = tDiff/3600;
	int mDiff = (tDiff-hDiff*3600)/60;
	int sDiff = tDiff-hDiff*3600-mDiff*60;

printf("hDiff%10: %d -> %d\n", hDiff, hDiff/10);
printf("mDiff%10: %d -> %d\n", mDiff, mDiff/10);
fflush(stdout);

	if (hDiff > 0)
	{
	    sprintf(ts[i-18], "\2%d %s %d %s\2", hDiff, ((hDiff == 1 || hDiff == 21)? "година":((hDiff == 2 || hDiff == 3 || hDiff == 4)? "години":"годин")), mDiff,
		((mDiff == 1 || mDiff%10 == 1)? "хвилина":((mDiff%10 == 2 || mDiff%10 == 3 || mDiff%10 == 4)? "хвилини":"хвилин")));
	}
	else
	{
	    sprintf(ts[i-18], "\2%d %s\2", mDiff,
		((mDiff == 1 || mDiff%10 == 1)? "хвилина":((mDiff%10 == 2 || mDiff%10 == 3 || mDiff%10 == 4)? "хвилини":"хвилин")));
//		((mDiff == 1 || mDiff == 21 || mDiff == 31 || mDiff == 41 || mDiff == 51)? "хвилина":((mDiff == 2 || mDiff == 3 || mDiff == 4 || mDiff%10 == 2 || mDiff%10 == 3 || mDiff%10 == 4)? "хвилини":"хвилин")));
	}
    }
    S("PRIVMSG %s :%s, до ковтка залишилось %s\n", channel, nick, ts[1]);
    return 0;
}

void dictLatin(char *nick, char *chan, char *src)
{
	if (isRuAlpha2(*src))
	// RUS -> LAT
	{
		DICTREC rec;
		char buf[1024] = "";
		_dictfile f("/home/olivka/bot/dict/rus2lat.dict2");
    
		if (!f.open())
		{
			// printf("cannot open DB\n");
			do_randomtopic(chan, DONNO_RDB, nick, buf);
			return;
		}
    
		if (f.findFirst(src, &rec))
		{
    		//printf("%s: %s\n", rec.source, rec.destination);
			char stmp[4096];
			_dictfile::parseDictLine(rec.destination, stmp);
			S("PRIVMSG %s :%s, \2%s\2: %s\n", chan, nick, rec.source, stmp);
		}
		else
		{
			DICTSEARCH ds;
			if (f.findLike(src, &ds) && ds.numRecords > 1)
			{
				*buf = 0;
				for (int i = 0; i < ds.numRecords; i++)
				{
					strcat(buf, ds.source[i]);
					if (i < ds.numRecords-1)
						strcat(buf, ", ");
				}
				//printf("%s: %s\n", src, buf);
				S("PRIVMSG %s :%s, \2%s\2: %s\n", chan, nick, src, buf);
			}
			else
    			S("PRIVMSG %s :%s, not found\n", chan, nick);
		}
    
		f.close();
	}
	else

	// LAT -> RUS
	{
		DICTREC2 rec;
		char buf[1024] = "";
		_dictfile2 f("/home/olivka/bot/dict/lat2rus.dict3");
    
		if (!f.open())
		{
			// printf("cannot open DB\n");
			do_randomtopic(chan, DONNO_RDB, nick, buf);
			return;
		}
    
		if (f.findFirst(src, &rec) || f.findFirstKey(src, &rec))
		{
    		//printf("%s: %s\n", rec.source, rec.destination);
			char stmp[4096];
			//_dictfile::parseDictLine(rec.destination, stmp);
			S("PRIVMSG %s :%s, \2%s\2: %s\n", chan, nick, rec.source, rec.destination);
		}
		else
		{
			DICTSEARCH2 ds;
			if ((f.findLike(src, &ds, true) || f.findLike(src, &ds)) && ds.numRecords > 1)
			{
				*buf = 0;
				for (int i = 0; i < ds.numRecords; i++)
				{
					strcat(buf, ds.source[i]);
					if (i < ds.numRecords-1)
						strcat(buf, ", ");
				}
				//printf("%s: %s\n", src, buf);
				S("PRIVMSG %s :%s, \2%s\2: %s\n", chan, nick, src, buf);
			}
			else
    			S("PRIVMSG %s :%s, not found\n", chan, nick);
		}
    
		f.close();
	}
}

void dictLatinList(char *nick, char *chan, char *src)
{
	if (isRuAlpha2(*src))
	// RUS -> LAT
	{
		DICTREC rec;
		char buf[1024] = "";
		_dictfile f("/home/olivka/bot/dict/rus2lat.dict2");
    
		if (!f.open())
		{
			// printf("cannot open DB\n");
			do_randomtopic(chan, DONNO_RDB, nick, buf);
			return;
		}
		
		DICTSEARCH ds;
		if (f.findLike(src, &ds) && ds.numRecords > 1)
		{
			*buf = 0;
			for (int i = 0; i < ds.numRecords; i++)
			{
				strcat(buf, ds.source[i]);
				if (i < ds.numRecords-1)
					strcat(buf, ", ");
			}
			//printf("%s: %s\n", src, buf);
			S("PRIVMSG %s :%s, \2%s\2: %s\n", chan, nick, src, buf);
		}
		else
			S("PRIVMSG %s :%s, not found\n", chan, nick);

		f.close();
	}
	else

	// LAT -> RUS
	{
		DICTREC2 rec;
		char buf[1024] = "";
		_dictfile2 f("/home/olivka/bot/dict/lat2rus.dict3");
    
		if (!f.open())
		{
			// printf("cannot open DB\n");
			do_randomtopic(chan, DONNO_RDB, nick, buf);
			return;
		}
		
		DICTSEARCH2 ds;
		if ((f.findLike(src, &ds, true) || f.findLike(src, &ds)) && ds.numRecords > 1)
		{
			*buf = 0;
			for (int i = 0; i < ds.numRecords; i++)
			{
				strcat(buf, ds.source[i]);
				if (i < ds.numRecords-1)
					strcat(buf, ", ");
			}
			//printf("%s: %s\n", src, buf);
			S("PRIVMSG %s :%s, \2%s\2: %s\n", chan, nick, src, buf);
		}
		else
			S("PRIVMSG %s :%s, not found\n", chan, nick);
    
		f.close();
	}
}

#include "netpack.c"

char *nocrlf(char *src)
{
    for (int i = 0;i < 10240;i++)
    {
        if (src[i] == '\r' || src[i] == '\n')
        {
        		src[i] = 0;
        		break;
        	}
        	else if (src[i] == 0) break;
    }
    return src;
}

void showDateByIndex(char *channel, char *nick, char *sindex)
{
	char line[1024];
	DATE_INFO diSet[OUTPUT_PURGE_COUNT];
	int numItems = 0, index;
	
	if (!inv_vars[INTV_SOWHAT])
	{
		do_randomtopic(channel, "dateprefix3", nick, sindex);
		return;
	}

	index = atoi(sindex);
	numItems = getDateRecords(diSet, OUTPUT_PURGE_COUNT, 0);

	if (index < 1 || index > 100 || index > numItems)
	{
		S ("PRIVMSG %s :%s, bad index\n", channel, nick);
		return;
	}
	
	if (!numItems)
	{
		do_randomtopic(channel, "dateprefix3", nick, sindex);
		return;
	}

	sprintf(line, "%s: \2%d.\2 %s\n", nick, index, diSet[index-1].text); 
	S ("PRIVMSG %s :%s\n", channel, line);
}

char *s_time(time_t ttime)
{
    struct tm *time_v;
    size_t rc = 0;

    strcpy(time_s,"N/A");

    if (!ttime)
		ttime = time(NULL);;

   	time_v = localtime(&ttime);	
	strftime(time_s, 63, "%d/%m/%Y %H:%M:%S", time_v);
	
	return time_s;
}

int isNoChan1(char *chan)
{
    if (noChanEnable && chan && strstr(NOCHAN1, chan)) return 1;
    return 0;
}

int isNoChan2(char *chan)
{
    if (noChanEnable && chan && strstr(NOCHAN2, chan)) return 1;
    return 0;
}

int sec_sleep(int sec)
{
    int max_sec = 30;
    char temp[1024];
    if (sec > max_sec) sec = max_sec;
    snprintf (temp, sizeof(temp), "/home/olivka/bot/sec_delay.sh %d\n", sec);
    char *res = (char *)run_program(temp);
    return sec;
}

void start_telegram_reader()
{
    int err = pthread_create(&ttid, NULL, &telegram_reader, NULL);
        if (err != 0)
            printf("\nCan't create TG thread :[%s]", strerror(err));
        else
            printf("\nTG thread created successfully\n");
}

void* telegram_reader(void *arg)
{
    unsigned long i = 0;
    pthread_t id = pthread_self();


#define BUFSZ 1024
FILE *pipe = popen("tail -f /home/olivka/bot/logs/tg.log", "r");
    while (1)
    {
	printf("123");
	char buf[4096];
	int t = fread(buf, 4096, 1, pipe);
	if (t > 0)
	{
	    buf[t] = 0;
	    printf(buf);
	    S ("PRIVMSG %s :%s\n", "ncuxonycbka", buf);
	}
    }
}

/*
int mkpath(char* file_path, mode_t mode) {
  assert(file_path && *file_path);
  char* p;
  for (p=strchr(file_path+1, '/'); p; p=strchr(p+1, '/')) {
    *p='\0';
    if (mkdir(file_path, mode)==-1) {
      if (errno!=EEXIST) { *p='/'; return -1; }
    }
    *p='/';
  }
  return 0;
}
*/
int mkpath(char *dir, mode_t mode)
{
char mkcmd[8000] = "";
sprintf(mkcmd, "mkdir -p %s", dir);
system(mkcmd);
return 0;
}

void chanserv_log_history(char *source, char *target, char *buf)
{
    char t[10240];
    char ts[1024];
    time_t now = time (0);
    strftime (ts, 1023, "%Y-%m-%d %H:%M:%S", localtime (&now));
    sprintf(t, "%s %s %s %s\n", ts, source, target, buf+1);


    // 1
    FILE *p = fopen("/home/olivka/bot/history/chanserv.log", "a+");
    if (p == 0)
    {
	printf("Error opening cs log\n");
	return;
    }    
    fprintf(p, t);
    fclose(p);


    // 2
    char di[1024];
    char ps[1024];
    char source1[1024];
    sprintf(di, "/home/olivka/bot/history/%s", target);
    mkdir(di, 0777);
    strcpy(source1, source);
    char *px = strchr(source1, '!');
    if (px != NULL) {
	*px = 0;
    }
    sprintf(ps, "/home/olivka/bot/history/%s/%s.log", target, source1);
    p = fopen(ps, "a+");
    if (p == 0)
    {
	printf("Error opening nick log\n");
	return;
    }
    sprintf(t, "%s %s\n", ts, buf+1);
    fprintf(p, t);
    fclose(p);

    // 3
    char tsi[1024];
    strftime (tsi, 1023, "%Y/%m/%d", localtime (&now));
    sprintf(di, "/home/olivka/bot/history/%s/%s", target, tsi);
    printf("Target H: %s\n", di);
    mkpath(di, 0777);
    sprintf(ps, "/home/olivka/bot/history/%s/%s/%s.log", target, tsi, source1);
    p = fopen(ps, "a+");
    if (p == 0)
    {
	printf("Error opening daily nick log\n");
	return;
    }
    fprintf(p, t);
    fclose(p);
}


