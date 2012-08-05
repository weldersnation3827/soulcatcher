#ifndef SOUL_CONFIG
#define SOUL_CONFIG

#include "base.h"
#include "sub_acl.h"
#include "SST.h"
#include "serrors.h"
#include "config.h"
#include "global.h"

#include <unistd.h>
#include <fstream>
#include <ctime>
#include <stdarg.h>

class soulC : public soulB {

private:

enum line_returns { SUBNET=0, ACL, GLOBAL, NOTHING, BAD_LINE, ERROR, END_SECTION,
					SUBNET_LINE, ACL_LINE, IS_EOF };

const char *config_path;
char *user; 
int create_db;
char *db_name;
int log_urls, log_denied;

bool DEBUG_STATE;

SST <char *> *SSTGlobal;
SST <CSUBNET *> *SSTSubnet;
SST <CACL *> *SSTAcl;

char *parse_subnet_line(char *, int &);
char *parse_acl_line(char *, int &);
soulC::line_returns line_parse(ifstream &, char *&, int);
bool log_parse(char *, char *, int, info_struct *);
bool config_parse(char *,char *);
bool close_config();
bool check_subnets_against_acls();
char *get_title_from_range(int *);


public:

soulC(const char *path, int create, char *create_name, char *priv_name, bool DEBUG) : config_path(path), create_db(create), 
	user(priv_name) ,DEBUG_STATE(DEBUG)
 			{ if(config_parse((char *)config_path, create_name)) { throw(1); return; }
			  if(create_db) { throw(3); return; } 
			  if(check_subnets_against_acls()) { throw(2); return; } 
			}

~soulC() { close_config(); }

bool ip_exist_in_subnets(int *);
bool log(types,const char *,...);
config_returns get_config(const char *&, const char *, int, char *, int*);
config_returns do_exist(const char *, int, char *, int *);
bool find_match(char *, databases, char *);
void test();
        
};
        
#endif // SOUL_CONFIG
