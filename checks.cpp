#include "checks.h"

bool soulCH::safe_find_in_line(char *word, char *line) {

	// changed incase of overflow, did not happen but just incase
	// Not used, have to investigate more to see if some strncmp on different
	// libc types do not account for the /0. There incase.

	int size_check=strlen(word);
	do {
		if((strlen(word) > strlen(line))) size_check=strlen(line);
		if(!strncmp(line,word,size_check)) return 1;
	} while(*(++line));
	
	return 0;
}

bool soulCH::find_in_line(char *word, char *line) {

	/* Find word in line */

	int times=0;
	do {
		if(!strncmp(line,word,strlen(word))) times++;
	} while(*(++line));
	
	return times;
}

bool soulCH::find_char_in_line(char letter,char *line) {
	
	/* find char in line */

	int times=0;
	do { 	
		if(*line == letter) times++;
	} while(*(++line));
	
	return times;
}

soulB::checks_returns soulCH::acl_checks(char *acl) {

	/* Run checks for ACL if they exist in confile file */

	soulB::checks_returns return_value = CONTINUE;

	if ((db_io->do_exist(users_file,0,acl,NULL)) == TRUE) if(return_value = user_match(acl)) return return_value; 		
	if ((db_io->do_exist(ext_file,0,acl,NULL)) == TRUE) if(return_value = extensions_match(acl)) return return_value;

	if ((db_io->do_exist(key_file,0,acl,NULL)) == TRUE) if(return_value = keyword_match(acl)) return return_value;
	if ((db_io->do_exist(prob_file,0,acl,NULL)) == TRUE) if(return_value = prob_match(acl)) return return_value;

	if ((db_io->do_exist(textdom_file,0,acl,NULL)) == TRUE) if(return_value = db_match(acl,DOMAINS)) return return_value; 
	if ((db_io->do_exist(texturl_file,0,acl,NULL)) == TRUE) if(return_value = db_match(acl,URLS)) return return_value; 

	return CONTINUE;
}

bool soulCH::open_file(const char *file_path,fstream *file_to_open) {

	/* Open file */

	file_to_open->open(file_path,fstream::in);
	if(!file_to_open->good()) {  db_io->log(ERR,serror20,file_path); return 1; }

	return 0;
}

bool soulCH::close_file(fstream *file_to_close) {

	/* close file */

	file_to_close->close();
}

bool soulCH::read_line(fstream *file_to_read, char *line_buffer) {
	
	/* Read line from file */

	while(!file_to_read->eof()) {
		file_to_read->getline(line_buffer,_1K,'\n');
		if(*line_buffer == '#' | *line_buffer == '\0') continue; 
		return 1;
 	} 

	return 0;
}

soulB::checks_returns soulCH::subnet_checks() {

	/* Run checks for IP */

	soulB::checks_returns return_value = CONTINUE;
 
	if((db_io->do_exist(lock_switch,0,NULL,input->ip_num)) == TRUE) if(return_value = lock_match()) return return_value; 
	if((db_io->do_exist(deny_file,0,NULL,input->ip_num)) == TRUE) if(return_value = method_match()) return return_value;
	
	return CONTINUE;
}

soulB::checks_returns soulCH::db_match(char *acl, databases database_type) {

	/* Match DB */

	char *wanted_keyword; int len=0;

	if(!database_type) { wanted_keyword = input->domain; } else { wanted_keyword = input->url; }
	if(db_io->find_match(acl,database_type,wanted_keyword)) return RETURN_DENIED; 
	
	return CONTINUE;
}

soulB::checks_returns soulCH::extensions_match(char *acl) {

	/* Match extensions */

	// Might need to do a after domain part to parse better.
	// eg, <www.domain.part></www/dish/movie.exe>
	//                           ^^^ want that part

	char *extptr; const char *file_path; 
	fstream *extensions_file = new fstream; char line_buffer[_1K];

	if((db_io->get_config(file_path,ext_file,0,acl,NULL)) != TRUE) return CHECKS_ERROR; 
	if(open_file(file_path,extensions_file)) return CHECKS_ERROR;

        if ((extptr=strrchr(input->url,'.')) == NULL) { return CHECKS_ERROR; } extptr++;

	while(read_line(extensions_file,line_buffer)) {
                if(!strcmp(extptr,line_buffer)) return RETURN_DENIED; 
        }

	close_file(extensions_file);
        return CONTINUE;
}

soulB::checks_returns soulCH::keyword_match(char *acl) {

	/* Match Keywords */

	char *checkptr; pack *package; const char *file_path;
	fstream *keyword_file = new fstream; char line_buffer[_1K];
	
	if((db_io->get_config(file_path,key_file,0,acl,NULL)) != TRUE) return CHECKS_ERROR; 
	if(open_file(file_path,keyword_file)) return CHECKS_ERROR;
	
	while(read_line(keyword_file,line_buffer)) {
		
		char *line_ptr = *&line_buffer; 
		if (*line_ptr == '+') { line_ptr++; checkptr = input->url; } else { checkptr = input->domain; }

		int len = strlen(line_ptr);
		do {
			if (!strncmp(checkptr,line_ptr,len)) return RETURN_DENIED;
		} while (*(++checkptr));
	}

	close_file(keyword_file);
	return CONTINUE;
}

soulB::checks_returns soulCH::lock_match() {

	/* Check if lock wanted */

	const char *lock_status; int answer=0; 
	
	if((db_io->get_config(lock_status,lock_switch,0,NULL,input->ip_num)) != TRUE) return CHECKS_ERROR;
	if(valid_answer(lock_status,answer)) return CHECKS_ERROR;  if(answer) return RETURN_DENIED;
	
	return CONTINUE;
}

soulB::checks_returns soulCH::method_match() {

	/* Check if method matches */

	const char *method_list; 

	if((db_io->get_config(method_list,deny_file,0,NULL,input->ip_num)) != TRUE) return CHECKS_ERROR;
	if(find_in_line(input->method,(char *)method_list)) return RETURN_DENIED;
	
	return CONTINUE;
}

soulB::checks_returns soulCH::user_match(char *acl) {

	/* Check if user matches */

	const char *user_list;

	if((db_io->get_config(user_list,users_file,0,acl,NULL)) != TRUE) return CHECKS_ERROR;
	if(find_in_line(input->ident,(char *)user_list)) return RETURN_DENIED;

	return CONTINUE;
}

soulB::checks_returns soulCH::prob_match(char *acl) {
	
	/* Check probablity of bad URL */

	// REDO ERRORS

	const char *file_path; pack *package; float score=12.0; 
	float total_value=0; float score_add=0; int field_len=0; 
	fstream *probab_file = new fstream; char line_buffer[_1K];
	
	if((db_io->get_config(file_path,prob_file,0,acl,NULL)) != TRUE) return CHECKS_ERROR; 
	if(open_file(file_path,probab_file)) return CHECKS_ERROR;
	
	while(read_line(probab_file,line_buffer)) {
	
		char *temp_string = line_buffer; char *temp_token = line_buffer; int second_token=0;
		if(!(field_len=search_string(temp_string,'='))) { db_io->log(ERR,"The field %s was invalid.",package->data); return CONTINUE; }
		
		int third_token = (field_len-1); temp_token+=field_len;
		
		if(!isdigit(*temp_token)) { db_io->log(ERR,"The field %s had no contents or invalid contents",package->data); return CONTINUE; }
		score_add = atof(temp_token); 
		
		if(!strncmp(temp_string,"SCORE",5)) {
			if (!score_add || score_add <0) { db_io->log(ERR,"The score check in file %s was invalid, continuing but keeping default score of %d",file_path,(double)score);
				} else { score = score_add; }
		}
		
		second_token=search_string(temp_string,'_'); 
		if(!strncmp(temp_string,"WORD",4)) { 
	 	
	 		char word_store[_1K]={0}; 
			if(!second_token) { db_io->log(ERR,"The field %s was invalid.",package->data); return CONTINUE; }
			if((third_token-second_token)>=_1K) return CHECKS_ERROR;
			
			temp_string+=second_token; strncpy(word_store,temp_string,third_token-second_token);
			int how_many = find_in_line(word_store,input->url);
			total_value+=(score_add*how_many);
		}
		
		if(!strncmp(temp_string,"LETTER",6)) {
			
			char letter_store='\0';
			if(!second_token) { db_io->log(ERR,"The field %s was invalid.",package->data); return CONTINUE; }
				
			if(((third_token-second_token))!=1) { db_io->log(ERR,"Need a letter between LETTER_ and = in line %s. Skipping.",package->data); break; }
			letter_store = temp_string[second_token]; 
		
			int how_many = find_char_in_line(letter_store,input->url);
			total_value+=(score_add*how_many);
		}
	}
	
	if(DEBUG_STATE) db_io->log(DEBUG,"Prob check returned value of %d",total_value);
	
	close_file(probab_file);

	if(total_value >= score) return RETURN_DENIED;
	return CONTINUE;
}
