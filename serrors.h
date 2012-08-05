
#ifndef __ERROR_MESSAGES__
#define __ERROR_MESSAGES__

/* Error messages */

static const char *serror1 = "Invalid or malformed keyword at line %s in subnet %s at line: %i. ";
static const char *serror2 = "Invalid or malformed keyword at line %s in acl %s at line: %i. ";
static const char *serror3 = "Invalid or malformed keyword at line %s in global at line: %i. ";
static const char *serror4 = "Malformed global keyword: %s";
static const char *serror5 = "Unable to find config file at location: %s";
static const char *serror6 = "Config file has either TEXT_DOMAIN_DB or TEXT_URL_DB with no SOUL_DB or SOULDB on its own in acl %s, please correct this.";
static const char *serror7 = "Cannot create or open log file at location %s";
static const char *serror8 = "Error in Class fileio, please check log for more info.";
static const char *serror9 = "Error while checking sanity of config file, please check log for more info.";
static const char *serror10 = "Unable to find %s. Please check if file is present and permissions are correct.";
static const char *serror11 = "User %s does not have read permissions for file %s.";
static const char *serror12 = "User %s does not exist";
static const char *serror13 = "User %s is not the owner of %s, i tried to change the permissions but it failed.";
static const char *serror14 = "ERROR: Unable to get user information";
static const char *serror15 = "ERROR: Cannot change UID to user ";
static const char *serror16 = "ERROR: Exiting.";
static const char *serror17 = "Error repharsing config file, QUITING.";
static const char *serror18 = "Unable to allocated memory for SoulCH::, EXITING";
static const char *serror19 = "ban_list in the default subnet needs to exist.";
static const char *serror20 = "Unable to open file %s.";
static const char *serror21 = "Caught HUP signal - reparsing config file.";
static const char *serror22 = "Unable to allocation memory to infostruct - EXITING";
static const char *serror23 = "Invalid Answer to switch %s";
static const char *serror24 = "Invalid Answer to option %s or option is set higher than %i";
static const char *serror25 = "Syntax of URL wrong, sending user to URL: %s";
static const char *serror26 = "String too large for %s buffer, it is set at %i. Increase %s option in config file.";
static const char *serror27 = "DOMAIN_SIZE size is to low.. recommended you set it to the same as URL_SIZE";
static const char *serror29 = "Unable to write record into database.";
static const char *serror30 = " - Unable to clean database must not be created yet. Continuing.";
static const char *serror31 = "Unable to read database (%s) in acl %s, please check if file or path is present, if not please run ./soul -a";
static const char *serror32 = "Malformed Subnet address: %s";
static const char *serror33 = "Subnet %s has an ACL listed called %s. This ACL does not exist.";
static const char *serror34 = "Databases finished. Exiting.";


#endif // ERROR_MESSAGES
