#include "soul.h"

bool soulI::welcome_message() {

	/* Welcome Log Message */

	char *line[30]; memset(line,'-',(sizeof(line)+1));
	
	time_t current_time = time(0);
	char *the_time = ctime(&current_time); the_time[strlen(the_time)-1] = '\0';

	if(db_io->log(WELC,"Soulcatcher started at PID %i, timestamp %s",SC_PID,the_time)) return 1; 
	
	return 0;
}

void soulI::set_defaults() {

	/* Set Default Variables */

	log_urls=1; // Log urls??
	log_denied=1; // Log Denied Urls??
		
	URL_SIZE = 4000; // Url size
	IP_SIZE = 16; // Ip string Size
        FQDN_SIZE = 100; // Full Qualified Domain Name Size
        IDENT_SIZE = 100; // Ident size
        METHOD_SIZE = 20; // Method eg, GET, CONNECT size
	DOMAIN_SIZE = URL_SIZE; // Domain size
	
	INPUT_LINE_SIZE = (URL_SIZE+IP_SIZE+FQDN_SIZE+IDENT_SIZE+METHOD_SIZE); // INPUT LINE SIZE
}

bool soulI::reparse() {

	/* On -HUP close and open config file and redo toggles */

	db_io->log(INFO,serror21);
	
	delete(db_io);
	db_io = new soulC(path,0,NULL,user,DEBUG_STATE);
	
	if(setting_toggles()) return 1;
	if(DEBUG_STATE) db_io->test();
	
	return 0;
}			

bool soulI::use_infostruct(info_struct *tinput, info_enum status) {

	/* Creating, cleaning and uncreating info_struct */

	int count=0;
	char *temp[] = { tinput->url,tinput->ip,tinput->fqdn,tinput->ident,tinput->method,tinput->domain, NULL };
	int temp_size[] = { URL_SIZE,IP_SIZE,FQDN_SIZE,IDENT_SIZE,METHOD_SIZE,DOMAIN_SIZE };

	switch(status) {
	
		case CREATE:
			tinput->url = new char[URL_SIZE]; tinput->ip = new char[IP_SIZE]; 
	     		tinput->fqdn = new char[FQDN_SIZE]; tinput->ident = new char[IDENT_SIZE];
			tinput->method = new char[METHOD_SIZE]; tinput->domain = new char[DOMAIN_SIZE];
			break;
		case UNCREATE:
			while(temp[count]!=NULL) { delete(temp[count]); count++; }
			break;
		case CLEAN:
			tinput->status=0; 
			while(temp[count]!=NULL) { if(temp[count]==NULL) { db_io->log(ERR,serror22); return 1; }
				memset(temp[count],0,temp_size[count]); count++; }
			break;
		default:
			return 1;
	}

	return 0;
}

soulI::soulI(char *main_path,int create, char *create_name, char *priv_name, bool DEBUG) :
	 path(main_path), db_create(create), user(priv_name), DEBUG_STATE(DEBUG) {

	/* Constructor */
	
	set_defaults(); 

	try {

		if ((input = new info_struct)==NULL) { cout << serror22 << endl; return; }
		db_io = new soulC(main_path,db_create,create_name,priv_name,DEBUG_STATE);

		if(setting_toggles()) return;	

	} catch(int i) {

		switch(i) {

			case 1: cout << serror8 << endl; break;
			case 2: cout << serror9 << endl; break;
		}

		throw(i); return;
	}
}

bool soulI::setting_toggles() {
	
	/* Set config file toggles */

	if(set_toggle("LOG_URLS_SWITCH",log_urls)) return 1;
	if(set_toggle("LOG_DENIED_SWITCH",log_denied)) return 1; 

	if(set_int("URL_SIZE",URL_SIZE)) return 1; if(set_int("FQDN_SIZE",FQDN_SIZE)) return 1;
	if(set_int("IDENT_SIZE",IDENT_SIZE)) return 1; if(set_int("METHOD_SIZE",METHOD_SIZE)) return 1;
	if(set_int("DOMAIN_SIZE",DOMAIN_SIZE)) return 1;
	
	return 0;
}

bool soulI::set_toggle(char *toggle_switch,bool &toggle) {

	/* Set certain toggle */

	const char *buffer; int answer=0;

	if(db_io->get_config(buffer,toggle_switch,0,NULL,NULL)) return 0;
	if (valid_answer(buffer,answer)) { db_io->log(ERR,serror23,toggle_switch); return 1; }
	toggle = answer;

	return 0;
}

bool soulI::set_int(char *toggle_int,int &int_in_question) {

	/* set certain integer from config file */

	const char *buffer; int int_temp=0;
	
	if(db_io->get_config(buffer,toggle_int,0,NULL,NULL)) return 0; int_temp = atoi(buffer);
	if(!int_temp || int_temp > MAX_IN_SIZE) { db_io->log(ERR,serror24,toggle_int,MAX_IN_SIZE); return 1; }
	
	int_in_question = int_temp;	
	return 0;
}
		
bool soulI::working_loop() {

	/* Da loop */

	int string_return=0; soulCH *checks;
	
	if(use_infostruct(input,CREATE)) return 1;
	if(welcome_message()) return 1;	

	while(1) {

		if((string_return = string_input())==1) { parse_error(); continue; } else if(string_return==2) { break; }
		
		if((checks = new soulCH(db_io,input,DEBUG_STATE))==NULL) { stdout_log(ERR,serror18); return 1; }
		if((string_return = process_banlist(checks))==1) { parse_error(); continue; }
		delete(checks);
		
		if(string_output()) { parse_error(); continue; }
	}

	use_infostruct(input,UNCREATE); return 0;
}

soulI::~soulI() { delete(db_io); delete(input); }

bool soulI::parse_error() {

	/* Output if error is found */

	const char *buffer;

	if(db_io->get_config(buffer,error_redirect_file,0,NULL,NULL)) return 1; 
	db_io->log(ERR,serror25,buffer);
	cout << buffer << " -/- - GET" << endl; 
	
	return 0;
}

int soulI::string_input() {

	/* Get input */

	if(use_infostruct(input,CLEAN)) return 1;	  
	char buffer[INPUT_LINE_SIZE];

	if(DEBUG_STATE) db_io->stdout_log(DEBUG,"STATUS TOGGLE EQUALS %i",input->status);

        cin.getline(buffer,INPUT_LINE_SIZE,'\n');
        if (!*buffer) { return 2; } // If null, kill
		
        if (parseinput(buffer,0)) return 1; 
	
        return 0;
}

bool soulI::parseinput(char *buffer, int type) { 
	
	/* parse input */

        char *tokenptr; int counter=0;
	char *ident_chars[] = { "URL_SIZE","FQDN_SIZE","IDENT_SIZE","METHOD_SIZE" };
	int temp_size[] = { URL_SIZE,FQDN_SIZE,IDENT_SIZE,METHOD_SIZE };
              
        tokenptr = strtok(buffer," ");
        while (tokenptr!=NULL) {
	
	if(strlen(tokenptr) >= temp_size[counter]) { 

                db_io->log(ERR,serror26,ident_chars[counter],temp_size[counter],ident_chars[counter]);
                return 1;
        }
	
        switch (counter) {
        
                case 0: strncpy(input->url,tokenptr,URL_SIZE); break;
                case 1: strncpy(input->fqdn,tokenptr,FQDN_SIZE); break;
                case 2: strncpy(input->ident,tokenptr,IDENT_SIZE); break;
                case 3: strncpy(input->method,tokenptr,METHOD_SIZE); break;
        
        }
                counter++; tokenptr = strtok(NULL," ");
        }
	
	if (type != 1) { if (domainparse(input->url)) { return 1; } } /* Parse domain */
	if (fqdn_parse(input->fqdn)) { return 1; } /* Parse fqdn and ip */

	return 0;
}

bool soulI::domainparse(char *url) {
            
	/* parse domain part of input */

	char *url_sec_buffer = url; int len=0;

	while(url_sec_buffer) { 
		if(*url_sec_buffer == '/') { 
			url_sec_buffer++; 
			if(*url_sec_buffer == '/') { url_sec_buffer++;
				if(!is_ip(url_sec_buffer)) { 
					len = search_string(url_sec_buffer,'.'); if (!len) return 1; url_sec_buffer+=len; 
				}
				len = search_string(url_sec_buffer,':');
				if (!len) { len = search_string(url_sec_buffer,'/'); if (!len) return 1; }
				break;
			}
		}
		url_sec_buffer++;
	}	
		

	if ((len) >= DOMAIN_SIZE) { db_io->log(ERR,serror27); return 1; }
	strncpy(input->domain,url_sec_buffer,len-1);

        return 0;
}

bool soulI::fqdn_parse(char *fqdn_buffer) {
         
	/* Parse ip and fqdn part of input */

        int fqdn_buffer_len=strlen(fqdn_buffer); int len=0;
	
        if(!(len=search_string(fqdn_buffer,'/'))) return 1;
        memcpy(input->ip,fqdn_buffer,len-1); // Entering into input struct - input->ip

        if (enter_subnet(input->ip,input->ip_num)) return 1; // Entering into input struct - input->ip_num
	fqdn_buffer+=len; if (len==fqdn_buffer_len) return 1; 

	memcpy(input->fqdn,fqdn_buffer,FQDN_SIZE); // Entering into input struct - input->fqdn  

        return 0;
}

bool soulI::parse_banlist() {

	/* Parse the banlist and set its order */

	// CHANGE TO CONSTANT - REWRITE TO HASH

	const char *banlist_buffer; char *banlist_token; int counter=0; int error=0;
	banlist_root = new sorted_banlist; banlist_ptr = banlist_root; banlist_ptr->next=NULL;

	if((db_io->get_config(banlist_buffer,ban_file,0,NULL,input->ip_num)) != TRUE) return 1; 

	for (int i=0;i<2;i++) {

		int banlist_buffer_len = strlen(banlist_buffer)+1;

		char *banlist_sec_buffer = new char[banlist_buffer_len]; strncpy(banlist_sec_buffer,banlist_buffer,banlist_buffer_len);
		if((banlist_token=strtok(banlist_sec_buffer,",")) == NULL) return 1;

		change_case(banlist_sec_buffer,banlist_buffer_len,TOUPPER);

		switch(i) {
			case 0: 
				while(banlist_token != NULL) { 
					if(*banlist_token == '!') {
						banlist_ptr->id = counter; strncpy(banlist_ptr->name,banlist_token,sizeof(banlist_ptr->name));
						banlist_ptr->next = new sorted_banlist; banlist_ptr = banlist_ptr->next; banlist_ptr->next = NULL;
						counter++; 
					}
				banlist_token=strtok(NULL,",");
				}
				break;

			case 1: 
				while(banlist_token != NULL) { 
					if(*banlist_token != '!') { 
						banlist_ptr->id = counter; strncpy(banlist_ptr->name,banlist_token,sizeof(banlist_ptr->name)); 
						banlist_ptr->next = new sorted_banlist; banlist_ptr = banlist_ptr->next; banlist_ptr->next = NULL;
						counter++;
					}
				banlist_token=strtok(NULL,",");
				}
				break;
		}
		delete(banlist_sec_buffer);
	}

	return 0;
}

bool soulI::process_banlist(soulCH *checks) {

	/* process banlist and call checks */

	soulB::checks_returns return_value=CONTINUE; 

	if(parse_banlist()) return 1;
	banlist_ptr = banlist_root; 

	while(banlist_ptr->next != NULL) {

		char *banlist_name = banlist_ptr->name;
		if(*banlist_name == '!') { banlist_name++; input->toggle=1; }

		return_value=checks->acl_checks(banlist_name);
		switch(return_value) {
		
			case CHECKS_ERROR: return 1; 
			case RETURN_ALLOWED: return 0;
			case RETURN_DENIED: input->status=1; return 0;
		};  input->toggle=0;

		return_value=checks->subnet_checks(); 
		switch(return_value) {
		
			case CHECKS_ERROR: return 1; 
			case RETURN_ALLOWED: return 0;
			case RETURN_DENIED: input->status=1; return 0;
		}
		
		banlist_ptr = banlist_ptr->next;
	}

	return 0;
}

bool soulI::string_output() {

	/* std output */

        const char *redirect; 

	if(input->toggle) input->status=0; /* The ! ban_list toggle */

	if(!input->status) {
		
                cout << input->url << ' ' <<
                        input->ip << '/' << input->fqdn << ' ' <<
                        input->ident << ' ' <<
                        input->method << ' ' << endl;
		if(log_urls) { if (db_io->log(URL,"%s %s %s %s %s",input->url,input->ip,input->fqdn,input->ident,input->method)) return 1; }
		
        } else {

                if((db_io->get_config(redirect,redirect_file,0,NULL,input->ip_num)) != TRUE) {
			db_io->get_config(redirect,redirect_file,0,NULL,NULL); }
		
		// PARSE AT START NOT REDO EVERY OUTPUT

		while(*redirect) {
			
			if(*redirect=='%') { *redirect++;
				
				if(!*redirect) { cout << '%'; break; }
				switch(*redirect) {
				
					case 'i': cout << input->ip; break;
					case 'm': cout << input->method; break;
					case 'f': cout << input->fqdn; break;
					case 'd': cout << input->ident; break;
					default: cout << '%' << *redirect;
				
				} *redirect++;
			}
			
			cout << *redirect;
			redirect++;
		}
			
                cout << ' ' <<
                        input->ip << '/' << input->fqdn << ' ' <<
                        input->ident << ' ' <<
                        input->method << ' ' << endl;
		
		if(log_denied) { if (db_io->log(BOUNCE,"%s %s %s %s %s",input->url,input->ip,input->fqdn,input->ident,input->method)) return 1; }
        }
        return 0;
}
