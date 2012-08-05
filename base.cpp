#include "base.h"

bool soulB::touch(char *filename, bool createheader, char *user) {

	/* Open file and create header if requested */

	char *header="# This file was created automatically by soulcatcher";
	ofstream touch_file(filename,ios::app);
	
	if(!touch_file.good()) return 1;
	if(createheader) touch_file << header << endl; 
	touch_file.close();

	return 0;
}

bool soulB::split(char *whole, char spliter, char *&high_end, char *&low_end) {

	/* Split a string in two. */

	int whole_size = (strlen(whole) + 1); int pos=0;

	char *whole_copy = new char[ whole_size ];
	char *whole_copy_root = whole_copy;
	strncpy(whole_copy,whole,whole_size);

	pos = search_string(whole_copy,spliter); 
	
	if(!pos) { delete whole_copy; return 1; }
	
	strncpy(high_end,whole_copy,pos); high_end[pos-1] = '\0'; 
	if(pos==whole_size) { delete whole_copy; return 1; } 
	
	whole_copy+=pos; pos = strlen(whole_copy);
	strncpy(low_end,whole_copy,pos); low_end[pos] = '\0'; 

	delete whole_copy_root;
	return 0;
}

void soulB::set_global_defaults() {

	/* Get process id */

	SC_PID=getpid();
}

bool soulB::remove_whitespace(char *buffer,int len) {

	/* remove whitespace from a string */
	//REDO CACHE USAGE MASSIVE

        int counter=0; 
	
        char *temp = new char[len]; memset(temp,0,len);

        for (int i=0;i < len;i++) {

                if (buffer[i]!='\t' && buffer[i]!=' ') {
                        temp[counter++]=buffer[i];
                }
        }

        temp[counter] = '\0'; memcpy(buffer,temp,len);

        delete(temp);
        return 0;
}

bool soulB::change_case(char *buffer,int len, enum casechange want) {

	/* change case of a string */

        for (int i=0;i < len;i++) {
        if (!want) { *buffer = toupper(*buffer); } else { *buffer=tolower(*buffer); }
        *buffer++; }

        memcpy(buffer,buffer,len);
        return 0;
}

int soulB::search_string(char *ss, char wanted) {

	/* search string for a character */

        int counter=1; int length=strlen(ss);

	if(!*ss) return 0;
        for(;;) {
                if (*ss == wanted) { return counter; } else if(counter == length) { break; }
                *ss++; counter++; }
        return 0;
}

bool soulB::enter_subnet(char *subnet, int subnet_quad[]) {

	/* change subnet string to quad array */

	char *subtok_ptr; int counter=0;
	
	char *temp_subnet = new char[strlen(subnet)];
	strncpy(temp_subnet,subnet,strlen(subnet));

        subtok_ptr = strtok(temp_subnet,".");
	if(subtok_ptr == NULL) { delete(temp_subnet); return 1; }
	
        while(subtok_ptr != NULL) {
                subnet_quad[counter] = atoi(subtok_ptr);
                subtok_ptr = strtok(NULL,".");
                counter++;
        }
	
	delete(temp_subnet);
        return 0;
}

bool soulB::areweinsubnet(int startsubnet[], int endsubnet[], int calcsubnet[]) {

	/* check if an address in within a range */

        for (int i=0; i < 4; i++) {
   	     if ((calcsubnet[i] >= startsubnet[i]) && (calcsubnet[i] <= endsubnet[i])) { } else { return 0; }
        }
        return 1;
}

bool soulB::valid_keyword(const char *keywords[],char *keyword) {

	// CLEAN!

	/* check if the string is a keyword in soul */

	int i=0; char *temp_keyword_ptr;
	
   	while(strcmp(keywords[i],end)) {
			temp_keyword_ptr = (char*)keywords[i]; if(*keywords[i]=='-') { temp_keyword_ptr = (char*)++keywords[i]; *keywords[i]--; }
			if (!strncmp(temp_keyword_ptr,keyword,strlen(keyword))) { return 0; }  i++;
	}
	
	return 1;
}

bool soulB::valid_answer(const char *question, int &answer) {

	/* check if the question is a valid answer */

	char letter = toupper(*question);

	switch(letter) {

		case 'T':
		case 'Y': answer=1; break;
		case 'F':
		case 'N': answer=0; break;

		default: return 1;
	}

	return 0;
}

bool soulB::stdout_log(types log_type, const char *string_parse, ...) {

	/* output to stdout */

	char log_types[][10] = { "INFO", "URL", "BOUNCE", "ERR", "DEBUG", "EMRG" };
	char head_buffer[10]; va_list argp; char *temp_char; int temp_int; double temp_double; int len=0; 
	
	if (log_type<=6 && log_type>=0) {
		strcpy(head_buffer,log_types[log_type]);
	}

	time_t current_time = time(0);
	char *the_time = ctime(&current_time); the_time[strlen(the_time)-1] = '\0';
	
	va_start(argp,string_parse);  cout << endl << head_buffer << ": " << the_time << " : PID=" << SC_PID << "." << endl;
	cout << "Error Details:\n" << endl; 
	
	while(*string_parse) { 

		if (*string_parse != '%') { cout << *string_parse; } else {

			switch (*++string_parse) {

				case 's':
					temp_char = va_arg(argp, char *);
					cout << temp_char; break;
				case 'i':
					temp_int = va_arg(argp, int);
					cout << temp_int;  break;
				case 'd':
					temp_double = va_arg(argp, double);
					cout << temp_double;  break;
			}
		}
	string_parse++; len++;	
	}

	cout << endl;
	va_end(argp);
	return 0;
}

bool soulB::is_ip(char *ip) {

	/* Checks if is an ip by looking to see if the first 3 '.' are < 3 places apart and numbers */

	int token_count=0, len=0;

	while((len = search_string(ip,'.'))) {

		if(len <= 4) { token_count++; if(!isdigit(*ip)) return 0; } 
		ip+=len; if(token_count==3) return 1;
	}

	return 0;
}

