#ifndef __ACL_SUB__
#define __ACL_SUB__

#include "SST.h"
#include "souldb.h"
#include "serrors.h"
#include "config.h"
#include "global.h"

#if ( DATABASE_TYPE == 1 )
	#include <db.h>
#endif

#include <iostream>
#include <string>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>

class CSUBNET : private soulB {

private:

bool DEBUG_STATE;

SST <char *> *SSTSubnet;

int subnet_start_quad[4];
int subnet_end_quad[4];

public:

CSUBNET(bool, char *);
~CSUBNET();

char subnet_name[_1K];

void test();
int *return_start_quad();
int *return_end_quad();
bool add_to_subnet(char *);
config_returns get_config(const char *&,const char *);
config_returns do_exist(const char *);

};

class CACL : soulDB {

private:

SST <char *> *SSTAcl;

#if ( DATABASE_TYPE == 1 )
DB *domptr;
DB *urlptr;
#endif

char *user;

bool DEBUG_STATE;

#if ( DATABASE_TYPE == 1 )
bool close_acl_databases();
#endif 

bool check_existence_and_permissions(char *);
bool link_check();

public:

CACL(bool, char *, char *);
~CACL();

char acl_name[20];

#if ( DATABASE_TYPE == 1 )
bool open_or_create_databases(bool);
bool db_match(databases, char *);
#endif

void test();
bool check_sanity();
bool add_to_acl(char *);

config_returns do_exist(const char *);
config_returns get_config(const char *&,const char *);

};

#endif
