#include "fileio.h"

soulC::line_returns soulC::line_parse(ifstream &file_stream, char *&line, int line_size) {

	/* Check line in config file */

	if(!file_stream.getline(line,CONFIG_LINE_SIZE)) return IS_EOF;
	char temp_line_hold[line_size]; memset(temp_line_hold,0,line_size);

	remove_whitespace(line,line_size);
	if(*line == '\0' || *line == '#'  || *line == '{') return NOTHING;

	if(*line == '}') return END_SECTION;

	int pos = search_string(line,'='); if(!pos) pos=line_size;
	change_case(line,pos,TOUPPER);

	 if(pos == line_size) {

		// Acl or Subnet
		if(!strncmp(line,"SUBNET",6)) {
			return SUBNET; }
		if(!strncmp(line,"ACL",3)) {
			return ACL; }
	
		return ERROR;
	}

	strncpy(temp_line_hold,line,pos-1);

	// Global?
	if(!valid_keyword(config_keywords,temp_line_hold)) { 
			return GLOBAL; }
			
	// Subnet?
	if(!valid_keyword(subnet_keywords,temp_line_hold)) {
			return SUBNET_LINE; }
			
	//Acl?
	if(!valid_keyword(acl_keywords,temp_line_hold)) {
			return ACL_LINE; }

	return BAD_LINE;	
}

char *soulC::parse_subnet_line(char *subnet, int &vreturn) {

	/* Check subnet line in config */

	if(strlen(subnet)<7) { vreturn=1; return subnet; } 
	subnet+=6;

	if(subnet[strlen(subnet)-1]=='{') subnet[strlen(subnet)-1] = '\0';
	if(!isdigit(*subnet)) { 
		if(!strncmp(subnet,"DEFAULT",7)) memset(subnet,0,sizeof(subnet)); 
	}  

	return subnet;
}

char *soulC::parse_acl_line(char *acl, int &vreturn) {

	/* Check ACL line */

	if(strlen(acl)<4) { vreturn=1; return acl; }
	acl+=3; if(acl[strlen(acl)-1]=='{')  acl[strlen(acl)-1] = '\0'; 
	
	return acl;
}

bool soulC::config_parse(char *config_filepath, char *create_name) {
	
	/* Parse config file */

	char *inp_buffer = new char[CONFIG_LINE_SIZE]; ifstream config; 
	CSUBNET *subnet_holder; CACL *acl_holder; int line=0; soulC::line_returns line_status;

	config.open(config_filepath,ios::in);
	if (!config.good()) { stdout_log(ERR,serror5,config_filepath); return 1; }

	//GLOBAL
	SSTGlobal = new SST<char *>(500);

	// SUBNET
	SSTSubnet = new SST<CSUBNET *>(500);

	// ACL CONFIGS
	SSTAcl = new SST<CACL *>(500);

	while((line_status = line_parse(config,inp_buffer,CONFIG_LINE_SIZE)) != IS_EOF) { line++;

		if(line_status == NOTHING) continue;

		if(line_status == SUBNET) { 
			int vreturn=0; char *subnet = parse_subnet_line(inp_buffer,vreturn); if(vreturn) return 1;
			if((subnet_holder = SSTSubnet->add_to_hash_return_object(new CSUBNET(DEBUG_STATE,subnet),subnet)) == NULL) return 1;
				
				while((line_status = line_parse(config,inp_buffer,CONFIG_LINE_SIZE)) != END_SECTION) { line++;
					if(line_status == SUBNET_LINE) { if(subnet_holder->add_to_subnet(inp_buffer)) return 1; }
					else if(line_status != NOTHING) { stdout_log(ERR,serror1,inp_buffer,subnet_holder->subnet_name,line); return 1; }
				}
			
			if(DEBUG_STATE) subnet_holder->test();
		}

		if(line_status == ACL) {

			int vreturn=0; char *acl = parse_acl_line(inp_buffer,vreturn); if(vreturn) return 1;
			if((acl_holder = SSTAcl->add_to_hash_return_object(new CACL(DEBUG_STATE,acl,user),acl)) == NULL) return 1;

 				while((line_status = line_parse(config,inp_buffer,CONFIG_LINE_SIZE)) != END_SECTION) { line++;
					if(line_status == ACL_LINE) { if(acl_holder->add_to_acl(inp_buffer)) return 1; }
					else if (line_status != NOTHING) { stdout_log(ERR,serror2,inp_buffer,acl_holder->acl_name,line); return 1; }
				 }

			if(acl_holder->check_sanity()) return 1;
			if(create_db) { if(acl_holder->open_or_create_databases(1)) return 1; } 

			if(acl_holder->open_or_create_databases(0)) return 1;		
			if(DEBUG_STATE) acl_holder->test();
		}
			
		if(line_status == GLOBAL) { 

			char *high_end = new char[_1K];
			char *low_end = new char[_1K];

			if(split(inp_buffer,'=',high_end,low_end)) { stdout_log(ERR,serror4,inp_buffer); return 1; }
			if(SSTGlobal->add_to_hash(low_end,high_end)) { stdout_log(ERR,serror4,inp_buffer); return 1; }

			delete high_end; 
		 }
			
		if(line_status == BAD_LINE || line_status == ERROR) {
			stdout_log(ERR,serror3,inp_buffer,line); return 1; }	
	}
	
	delete inp_buffer; config.close();
	if(DEBUG_STATE) test();
	return 0;
}

void soulC::test() {

	/* output test crap */

	cout << "Main Hashes" << endl;	
	SSTGlobal->show_tables();

	cout << "Subnet Main Hashes: " << endl;
	SSTSubnet->show_tables();

	cout << "Acl Main Hashes: " << endl;
	SSTAcl->show_tables();
}

char *soulC::get_title_from_range(int *ip_address) {

	/* Get subnet ptr from IP range */ 
		
	linear_bookmark subnet_holder = {0,0}; CSUBNET *subnet_ptr;

	while(!SSTSubnet->linear_search(subnet_holder,subnet_ptr)) {
		if(areweinsubnet(subnet_ptr->return_start_quad(),subnet_ptr->return_end_quad(),ip_address)) return subnet_ptr->subnet_name;
	}

	return NULL;
}

soulB::config_returns soulC::get_config(const char *&S_return,const char *wanted, int want, char *acl=NULL, int *ip_addy=NULL) {

	/* Get config info */

	// 0 - AUTO
	// 1 - GLOBAL
	// 2 - SUBNETS ( IF CANNOT FIND IN SUBNET(IP_ADDY) WILL LOOK IN DEFAULT SUBNET ) 
	// 3 - ACL
	// 4 - SUBNETS
	// 5 - DEFAULT SUBNET
	
	CACL *acl_ptr; CSUBNET *subnet_ptr; char *subnet_title=NULL; int error=0;
	if (!want) { want=1; if(ip_addy!=NULL) want=2; if(acl!=NULL) want=3; }
		
	switch(want) {

		case 1:
			if((S_return = SSTGlobal->get_object((char *)wanted)) == NULL) return TRUE;
			break;
		case 2:
			if((subnet_title = get_title_from_range(ip_addy)) != NULL) {
				if((subnet_ptr = SSTSubnet->get_object(subnet_title)) != NULL) {  
					if(subnet_ptr->get_config(S_return,wanted)) return TRUE;
				}
			} 
			return get_config(S_return, wanted, 5);
 			break;
		case 3:
			if((acl_ptr = SSTAcl->get_object((char *)acl)) == NULL) return CONFIG_ERROR;
			if(acl_ptr->get_config(S_return,wanted)) return TRUE;
			break;
		case 4:
			if((subnet_title = get_title_from_range(ip_addy)) == NULL) return CONFIG_ERROR;
			if((subnet_ptr = SSTSubnet->get_object(subnet_title)) == NULL) return CONFIG_ERROR;
			if(subnet_ptr->get_config(S_return,wanted)) return TRUE;
			break;
		case 5:
			if((subnet_ptr = SSTSubnet->get_object("")) == NULL) return CONFIG_ERROR;
			if(subnet_ptr->get_config(S_return,wanted)) return TRUE;
			break;	
	}

	return FALSE;
}

soulB::config_returns soulC::do_exist(const char *wanted, int want, char *acl=NULL, int *ip_addy=NULL) {

	/* check if keyword exists in config file */
	
	// 0 - AUTO
	// 1 - GLOBAL
	// 2 - SUBNETS ( IF CANNOT FIND IN SUBNET(IP_ADDY) WILL LOOK IN DEFAULT SUBNET ) 
	// 3 - ACL
	// 4 - SUBNETS
	// 5 - DEFAULT SUBNET

	CSUBNET *subnet_ptr; CACL *acl_ptr; char *subnet_title=NULL;
	if (!want) { want=1; if(ip_addy!=NULL) want=2; if(acl!=NULL) want=3; }

	switch(want) {
	
		case 1:
			return (config_returns)SSTGlobal->do_exist((char *)wanted);
		case 2:
			if((subnet_title = get_title_from_range(ip_addy)) != NULL) {
				if((subnet_ptr = SSTSubnet->get_object(subnet_title)) != NULL) {  
					return subnet_ptr->do_exist(wanted);
				}
			} 
			return do_exist(wanted, 5);
		case 3:
			if((acl_ptr = SSTAcl->get_object((char *)acl)) == NULL) return CONFIG_ERROR;
			return acl_ptr->do_exist(wanted);
		case 4:
			if((subnet_title = get_title_from_range(ip_addy)) == NULL) return CONFIG_ERROR;
			if((subnet_ptr = SSTSubnet->get_object(subnet_title)) == NULL) return CONFIG_ERROR;
			return subnet_ptr->do_exist(wanted);
		case 5:
			if((subnet_ptr = SSTSubnet->get_object("")) == NULL) return CONFIG_ERROR;
			return subnet_ptr->do_exist(wanted);
	}

	return FALSE;
}
	
bool soulC::ip_exist_in_subnets(int *ip_addy) {

	/* check if IP exists in subnet */

	if(get_title_from_range(ip_addy) == NULL) return 1;
	return 0;
}

bool soulC::close_config() {

	/* Close all config ptr's */

	// GLOBAL
   	delete SSTGlobal;
	
	// SUBNET
	delete SSTSubnet;

	// ACL
	delete SSTAcl;
	
	return 0;
}

bool soulC::check_subnets_against_acls() {

	/* check if banlist contains correct ACL names and ban_list in default is present */

	linear_bookmark subnet_holder = {0,0}; CSUBNET *subnet_ptr; const char *ban_list;
	char *banlist_token;

	if(!do_exist(ban_file,5,NULL,NULL)) { stdout_log(ERR,serror19); return 1; } 

	while(!SSTSubnet->linear_search(subnet_holder,subnet_ptr)) {
		
		if(subnet_ptr->get_config(ban_list,ban_file) == FALSE) continue;

		int banlist_len = (strlen(ban_list)+1);
		char *banlist_noconst = new char[banlist_len]; strncpy(banlist_noconst,ban_list,banlist_len);
		
		if((banlist_token=strtok(banlist_noconst,",")) == NULL) return 1;

		while(banlist_token != NULL) {
	
			change_case(banlist_token,strlen(banlist_token),TOUPPER);
			if(*banlist_token == '!') banlist_token++;

			if((do_exist("ANYTHING_I_NEED",0,banlist_token,NULL)) == CONFIG_ERROR) { 

				stdout_log(ERR,serror33,subnet_ptr->subnet_name,banlist_token);			
				delete(banlist_noconst); return 1;
			}

			banlist_token=strtok(NULL,",");
		}
		delete(banlist_noconst);
	}

	return 0;
}

bool soulC::find_match(char *acl, databases database_type, char *wanted_keyword) {
	
	/* Find DB match for ACL */

	CACL *acl_ptr;
	
	acl_ptr = SSTAcl->get_object(acl);
	if(acl_ptr->db_match(database_type,wanted_keyword)) return 1; 
	
	return CONTINUE;
}

bool soulC::log(types log_type, const char *string_parse,...) {

	/* Output to file log */

	const char *log_file; char log_types[][10] = { "INFO", "URL", "BOUNCE", "ERR", "DEBUG", "EMRG", "WELC" };
	char head_buffer[10]; va_list argp; char *temp_char; int temp_int; double temp_double;

	if (get_config(log_file,log_Cfile,0,NULL,NULL)) return 1;
	ofstream log_file_s(log_file,ios::app);

	if(!log_file_s.good()) { stdout_log(ERR,serror7,log_file); return 1; }
	
	if (log_type<=6 && log_type>=0) {
		strcpy(head_buffer,log_types[log_type]);
	}

	time_t current_time = time(0);
	char *the_time = ctime(&current_time); the_time[strlen(the_time)-1] = '\0';

	va_start(argp,string_parse); if(log_type!=WELC) log_file_s << head_buffer << ": " << the_time << " : PID "  << SC_PID << " : ";

	while(*string_parse) {

		if (*string_parse != '%') { log_file_s << *string_parse; } else {

			switch (*++string_parse) {

				case 's': 
					temp_char = va_arg(argp, char *);
					log_file_s << temp_char;  break;

				case 'i':
					temp_int = va_arg(argp, int);
					log_file_s << temp_int;  break;
				case 'd':	
					temp_double = va_arg(argp, double);
					log_file_s << temp_double;  break;
			}
		}
		string_parse++;
	}
	log_file_s << endl;

    log_file_s.close(); va_end(argp);
    return 0;
}
