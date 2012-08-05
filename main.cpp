
#include <unistd.h>
#include <signal.h>
#include <db.h>
#include <iostream>
#include <stdarg.h>
#include <pwd.h>

#include "soul.h"
#include "stats.h"
#include "serrors.h"

soulI *inp; soulS *status;

void signal_hup(int sig) { if(inp->reparse()) { cout << serror17 << endl; exit(0); } }

void display_version() { cout << "SoulCatcher Version: " << PACKAGE_VERSION << endl; }

void display_help() {

display_version();
cout << "\n-c <config path> ... Config file is at location <config path>" << endl
<< "-t <acl name> ... Create all url and domain databases for a certain acl" << endl
<< "-r <acl name> ... Create a certain domain database" << endl
<< "-l <acl name> ... Create a certain url database" << endl << endl
<< "-p <user> ... Run program as user" << endl << endl
<< "-v ... Version details" << endl 
<< "-h ... This screen" << endl
<< "-a ... Create all url and domain databases" << endl
<< "-o ... Create all domain databases" << endl
<< "-u ... Create all url databases" << endl
<< "-s ... Stats" << endl 
<< "-d ... Debug" << endl 
<< endl << "Please E-mail:  " << PACKAGE_BUGREPORT << " to report any bugs." << endl << endl;

}

int main(int argc, char *argv[]) {

char soul_path[_1K]={0}; char create_name[50]={0}; char priv_name[50]={0};
int create=0; int opt=0; bool debug=0;
struct passwd *user_info;

enum options { 	CREATE_ALL=1,
		CREATE_ALL_URL,
		CREATE_ALL_DOMAIN,
		CREATE_ALL_CERTAIN,
		CREATE_URL_CERTAIN,
		CREATE_DOMAIN_CERTAIN,
		
};

// signals

(void) signal(SIGHUP,signal_hup);

while ((opt = getopt(argc,argv,"c:r:l:t:p:aduvsho")) != -1) {

	switch(opt) {
		case 'c': strncpy(soul_path,(char *)optarg,sizeof(soul_path)); break;
		case 'r': create=CREATE_DOMAIN_CERTAIN; strncpy(create_name,(char *)optarg,sizeof(create_name)); break;
		case 'l': create=CREATE_URL_CERTAIN; strncpy(create_name,(char *)optarg,sizeof(create_name)); break;
		case 't': create=CREATE_ALL_CERTAIN; strncpy(create_name,(char *)optarg,sizeof(create_name)); break;

		case 'p': strncpy(priv_name,(char *)optarg,sizeof(priv_name)); break;
		
		case 's': status = new soulS(); status->print_stats(); delete(status); exit(0);
		case 'a': create=CREATE_ALL; break;
		case 'o': create=CREATE_ALL_DOMAIN; break;
		case 'u': create=CREATE_ALL_URL; break;
		case 'v': display_version(); exit(0);
		case 'd': debug=1; break;

		case 'h':
		case '?':
			 display_help(); exit(0);
	}
}

// user info - port point 

if(*priv_name) {
	if(!(user_info = getpwnam(priv_name))) { cout << serror14 << " on user " << priv_name << "." << endl; return 1; }	
} else { 
	if(!(user_info = getpwuid(getuid()))) { cout << serror14 << "." << endl; return 1; }
}

strncpy(priv_name,user_info->pw_name,sizeof(priv_name));	

// Set SC to selected effective uid - port point

if(seteuid(user_info->pw_uid)) { cout << serror15 << priv_name << "." << endl; return 1; }

// soul default path

if (!*soul_path) memcpy(soul_path,default_config_path,sizeof(soul_path));

// init

try {
	inp = new soulI(soul_path,create,create_name,priv_name,debug);
} catch (int i) {
	
	switch(i) {
		case 3: cout << serror34 << endl; return 0;
		default: cout << serror16 << endl; return 1;
	}
}

// loop 

if(inp->working_loop()) cout << serror16 << endl;

// closing

delete(inp);
return 0;

}


