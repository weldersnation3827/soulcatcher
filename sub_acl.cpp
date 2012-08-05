#include "sub_acl.h"

/* ------------------ SUBNETS ----------------- */

CSUBNET::CSUBNET(bool DEBUG, char *subnet) : DEBUG_STATE(DEBUG) {

	/* enter subnet details */

	SSTSubnet = new SST<char *>(500);

	if(!*subnet) { strcpy(subnet_name,"DEFAULT"); return; }
	strncpy(subnet_name,subnet,sizeof(subnet_name));

	char *subnet_start = new char[20]; char *subnet_end = new char[20];
	memset(subnet_start,0,20); memset(subnet_end,0,20);

	if(split(subnet,'-',subnet_start,subnet_end)) { 
		if(*subnet) { strncpy(subnet_start,subnet,20); strncpy(subnet_end,subnet,20); } else { 
			if(stdout_log(ERR,serror32,subnet)); return;
		 }
	}
	
	if(enter_subnet(subnet_start,subnet_start_quad)) { stdout_log(ERR,serror32,subnet); return; }
	if(enter_subnet(subnet_end,subnet_end_quad)) { stdout_log(ERR,serror32,subnet); return; }

	delete subnet_start; delete subnet_end;
};

CSUBNET::~CSUBNET() { delete SSTSubnet; }

int *CSUBNET::return_start_quad() { return subnet_start_quad; }

int *CSUBNET::return_end_quad() { return subnet_end_quad; }

bool CSUBNET::add_to_subnet(char *add_line) {

	/* add line to subnet */

	int add_line_size = strlen(add_line) + 1;
	char *high_end = new char[ add_line_size ];
	char *low_end = new char[ add_line_size ];

	if(split(add_line,'=',high_end,low_end)) return 1;
	if(SSTSubnet->add_to_hash(low_end,high_end)) return 1;

	delete high_end;
	return 0;
}

void CSUBNET::test() { 

	/* show test info */

	cout << " -- Subnet " << subnet_name << endl;
	SSTSubnet->show_tables();
}

soulB::config_returns CSUBNET::do_exist(const char *wanted) {

	/* check if wanted line exists in subnet */

	if(SSTSubnet->do_exist((char *)wanted)) return TRUE;
	return FALSE;
}

soulB::config_returns CSUBNET::get_config(const char *&S_return,const char *wanted) {

	/* get a line from subnet */

	if((S_return = SSTSubnet->get_object((char *)wanted)) != NULL) return TRUE;
	return FALSE;
}

/* --------------------- ACLS ---------------------- */

CACL::CACL(bool DEBUG, char *acl, char *priv_name) : DEBUG_STATE(DEBUG), user(priv_name), domptr(NULL), urlptr(NULL) { 
	
	/* enter info into acl */

	SSTAcl = new SST<char *>(500);	
	strncpy(acl_name,acl,sizeof(acl_name));
}

CACL::~CACL() { close_acl_databases(); delete SSTAcl; }

bool CACL::add_to_acl(char *add_line) {

	/* enter line into acl */

	int add_line_size = strlen(add_line) + 1;
	char *high_end = new char[ add_line_size ];
	char *low_end = new char[ add_line_size ];

	if(split(add_line,'=',high_end,low_end)) return 1;
	if(SSTAcl->add_to_hash(low_end,high_end)) return 1;

	if((strstr(high_end,"_FILE")) != NULL) if(check_existence_and_permissions(low_end)) return 1;

	delete high_end; 
	return 0;
}

bool CACL::check_sanity() {

	/* check sanity of object info*/

	if(link_check()) return 1;
	return 0;
}

bool CACL::link_check() {
	
	/* check if certain lines exists within subnet if another certain line does */

	// TEMP CHECK 
	bool db_able = do_exist(db_file);
	bool domopen = do_exist(textdom_file); bool urlopen = do_exist(texturl_file);

	if((((domopen + urlopen) > 0) && !db_able) || (((domopen + urlopen) == 0) && db_able)) { stdout_log(ERR,serror6,acl_name); return 1; }
	return 0;
}

#if ( DATABASE_TYPE == 1 )

bool CACL::open_or_create_databases(bool openorcreate) {
	
	/* create acl databases */
	// 0 - open
	// 1 - create

	const char *db_path, *text_path; u_int32_t db_flags = 0;

	if(!get_config(db_path,db_file)) return 0; 	
	if(openorcreate) { db_flags = DB_CREATE; } else { db_flags = DB_RDONLY; }
	 
	if(get_config(text_path,textdom_file)) {
		switch(openorcreate) {
			case 0: if(open_DB(acl_name,DOMAINS,db_path,domptr,db_flags)) return 1; break;
			case 1: if(create_DB(DOMAINS,db_path,text_path,acl_name,db_flags)) return 1; break;
		}
	}

	if(get_config(text_path,texturl_file)) {
		switch(openorcreate) {
			case 0: if(open_DB(acl_name,URLS,db_path,urlptr,db_flags)) return 1; break;
			case 1: if(create_DB(URLS,db_path,text_path,acl_name,db_flags)) return 1; break;
		}
	}
 
	return 0;
}

#endif

bool CACL::check_existence_and_permissions(char *file_path) {

	/* check permission of file - port point */

	struct stat file_stats; struct passwd *user_info;
	mode_t modes; 

	if(stat(file_path,&file_stats)) { touch(file_path,true,user); return 0; } 
	modes = file_stats.st_mode;

	// check user has read permissions and check if it is a file.

	if(!S_ISREG(modes)) { stdout_log(ERR,serror10,file_path); return 1; }
	if((S_IRUSR) > (modes & S_IRWXU)) { stdout_log(ERR,serror11,user,file_path); return 1; }

#ifndef NAME_CHECK

	// Check if its the users file 

	if(!(user_info = getpwnam(user))) { stdout_log(ERR,serror12,user); return 1; }  
	if(user_info->pw_uid != file_stats.st_uid) { 
		if(chown(file_path,user_info->pw_uid,user_info->pw_gid)) { 
			stdout_log(ERR,serror13,user,file_path); return 1; 
		} 
	}

#endif //NAME_CHECK

	return 0;
}

void CACL::test() { 
	
	/* print test info */

	cout << " -- Acl " << acl_name << " is setting files to the user " << user << endl;
	cout << " -- Domain database is "; if(!domptr) cout << "not "; cout << "open." << endl;
	cout << " -- URL database is "; if(!urlptr) cout << "not "; cout << "open." << endl;
	
	SSTAcl->show_tables();
}

#if ( DATABASE_TYPE == 1 )

bool CACL::db_match(databases database_type, char *wanted_keyword) {

	DB *db_ptr;

	if(!database_type) { db_ptr = domptr; } else { db_ptr = urlptr; }
	if(find_match(db_ptr,wanted_keyword,strlen(wanted_keyword))) return 1; 	
	
	return 0;
}

#endif

soulB::config_returns CACL::do_exist(const char *wanted) {

	/* check if line exists in acl */

	if(SSTAcl->do_exist((char *)wanted)) return TRUE;
	return FALSE;
}	

soulB::config_returns CACL::get_config(const char *&S_return,const char *wanted) {

	/* get line in acl */

	if((S_return = SSTAcl->get_object((char *)wanted)) != NULL) return TRUE;
	return FALSE;
}

#if ( DATABASE_TYPE == 1 )

bool CACL::close_acl_databases() {

	/* close acl databases */

	if(domptr) close_DB(domptr);
	if(urlptr) close_DB(urlptr);

	return 0;
}

#endif
