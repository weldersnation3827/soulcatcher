#ifndef BASE_H
#define BASE_H

#include "serrors.h"
#include "config.h"
#include "global.h"

#include <iostream>
#include <stdarg.h>
#include <string>
#include <fstream>

class soulB {

public:

int URL_SIZE;
int IP_SIZE;
int FQDN_SIZE;
int IDENT_SIZE;
int METHOD_SIZE;
int DOMAIN_SIZE;

pid_t SC_PID;

int INPUT_LINE_SIZE;

struct key_data {

        char key[100];  
        char data[_1K];

        key_data *next;
};

struct pack {

        char data[_1K];
        pack *next;
	pack *previous;
};

struct info_struct {

	char *url;
	char *ip; 
	int ip_num[4];
        char *fqdn;
        char *ident;
        char *method;

        char *domain;
        bool status; // Status - Bool Allowed/Denied
	bool toggle; // toggle - Bool return toggle
};

enum casechange { TOUPPER=0,
                  TOLOWER };


enum types { INFO=0,
             URL,
             BOUNCE,
             ERR,
             DEBUG,
             EMRG,
	     WELC
};

enum DB_type {  BERK=0,
		TDB,
		MYSQL,
		POSTSQL };

enum options { 	CREATE_ALL=1,
		CREATE_ALL_URL,
		CREATE_ALL_DOMAIN,
		CREATE_ALL_CERTAIN,
		CREATE_URL_CERTAIN,
		CREATE_DOMAIN_CERTAIN,
};

enum checks_returns { 	CONTINUE=0,
			CHECKS_ERROR, 
			RETURN_ALLOWED,
			RETURN_DENIED
};

enum config_returns {	FALSE=0,
			TRUE,
			CONFIG_ERROR
};

enum databases { DOMAINS=0,
                 URLS};

bool split(char *, char, char *&, char *&);
bool valid_keyword(const char *[],char *);
bool valid_answer(const char *, int &);
int search_string(char *, char);
bool areweinsubnet(int [], int [], int []);
bool remove_whitespace(char *,int);
bool change_case(char *, int, enum casechange);
bool enter_subnet(char *, int[]);
bool stdout_log(types, const char *,...);
void set_global_defaults();
bool touch(char *,bool,char *);
bool is_ip(char *);

soulB() { set_global_defaults(); }
~soulB() { }

protected:

bool DEBUG_STATE;

};

#endif /* BASE_H */
