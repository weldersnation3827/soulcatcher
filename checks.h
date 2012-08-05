#ifndef CHECKS_CPP
#define CHECKS_CPP

#include "base.h"
#include "fileio.h"
#include "serrors.h"
#include "config.h"
#include "global.h"

#include <iostream>
#include <math.h>
#include <fstream>

class soulCH : soulB {

private:

soulC *db_io;
info_struct *input;

bool DEBUG_STATE;

int URL_SIZE;
int IP_SIZE;
int FQDN_SIZE;
int IDENT_SIZE;
int METHOD_SIZE;
int DOMAIN_SIZE;

bool find_in_line(char *,char *);
bool find_char_in_line(char, char *);
bool safe_find_in_line(char *,char *);
checks_returns db_match(char *, databases);
soulB::checks_returns method_match();
soulB::checks_returns keyword_match(char *);
soulB::checks_returns lock_match();
soulB::checks_returns extensions_match(char *);
soulB::checks_returns user_match(char *);
soulB::checks_returns prob_match(char*);
bool open_file(const char *, fstream *);
bool close_file(fstream *);
bool read_line(fstream *, char *);

public:

soulCH(soulC *config, info_struct *inp_str, bool DEBUG) : db_io(config), input(inp_str), DEBUG_STATE(DEBUG) { }
~soulCH() {}

soulB::checks_returns acl_checks(char *);
soulB::checks_returns subnet_checks();

};

#endif
