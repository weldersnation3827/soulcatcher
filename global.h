
#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include "config.h" 

using namespace std; 

static char *default_config_path = SOUL_CONF;

static const int _1K = 1024; // 1K
static const int CONFIG_LINE_SIZE = _1K;

static const int MAX_IN_SIZE = 9999;
static const char *end = "_E_";

// Adding '-' into the front of a keyword will make it not check if the 
// keyword is in the config file and use the default.

static const char *config_keywords[] = {

	"GLOBAL_REDIRECT_PAGE", "GLOBAL_ERROR_REDIRECT", "GLOBAL_LOG_FILE",
	"-LOG_URLS_SWITCH", "-LOG_DENIED_SWITCH",	
	"-URL_SIZE", "-FQDN_SIZE", "-IDENT_SIZE", "-METHOD_SIZE", 
	"-DOMAIN_SIZE", end
};

static const char *acl_keywords[] = {

	"EXTENSIONS_FILE", "KEYWORDS_FILE", "TEXT_DOMAIN_FILE",
	"TEXT_URL_FILE", "SOUL_DB", "USER_LIST", "PROBABILITY_FILE", end
};

static const char *subnet_keywords[] = {

	"BAN_LIST", "REDIRECT_PAGE", "DENY_METHODS",
	"LOCK", "ERROR_REDIRECT", end
};

static const char *database_names[] = { "DOMAINS", "URLS" };

static const char *global_redirect_file = config_keywords[0];
static const int global_redirect_file_size = strlen(global_redirect_file);

static const char *error_redirect_file = config_keywords[1];
static const int error_redirect_file_size = strlen(error_redirect_file);

static const char *log_Cfile = config_keywords[2];
static const int log_Cfile_size = strlen(log_Cfile);

static const char *ext_file = acl_keywords[0];
static const int ext_file_size = strlen(ext_file);

static const char *key_file = acl_keywords[1];
static const int key_file_size = strlen(key_file);

static const char *textdom_file = acl_keywords[2];
static const int textdom_file_size = strlen(textdom_file);

static const char *texturl_file = acl_keywords[3];
static const int texturl_file_size = strlen(texturl_file);

static const char *db_file = acl_keywords[4];
static const int db_file_size = strlen(db_file);

static const char *users_file = acl_keywords[5];
static const int users_files_size = strlen(users_file);

static const char *prob_file = acl_keywords[6];
static const int prob_files_size = strlen(prob_file);

static const char *ban_file = subnet_keywords[0];
static const int ban_file_size = strlen(ban_file);

static const char *redirect_file = subnet_keywords[1];
static const int redirect_file_size = strlen(redirect_file);

static const char *deny_file = subnet_keywords[2];
static const int deny_file_size = strlen(deny_file);

static const char *lock_switch = subnet_keywords[3];
static const int deny_switch_size = strlen(lock_switch);

static const char *allow_switch = subnet_keywords[5];
static const int allow_switch_size = strlen(allow_switch);

#endif // GLOBAL_H
