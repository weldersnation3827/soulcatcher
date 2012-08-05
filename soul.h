#ifndef INPUT_CPP
#define INPUT_CPP

#include "fileio.h"
#include "base.h"
#include "checks.h"
#include "serrors.h"
#include "config.h"
#include "global.h"

#include <iostream>
#include <string>

class soulI : public soulB {

private:

enum info_enum { CREATE=0, UNCREATE, CLEAN };
struct sorted_banlist { int id; char name[1024]; sorted_banlist *next; };

bool parseinput(char *,int);
bool use_infostruct(info_struct *, info_enum);
bool domainparse(char *);
bool fqdn_parse(char *);  
bool parse_error();
bool setting_toggles();
bool set_toggle(char *,bool &);
bool set_int(char *,int &);
void set_defaults();
bool welcome_message();
bool parse_banlist();
bool process_banlist(soulCH *);
bool string_output();
int string_input();

bool DEBUG_STATE;

char *path;
char *user;
info_struct *input;

soulC *db_io;

sorted_banlist *banlist_root; 
sorted_banlist *banlist_ptr; 

int db_create;
bool log_urls;
bool log_denied;

public:

bool reparse();
bool working_loop();

soulI(char *,int, char *,char *,bool);
~soulI();

};

#endif
