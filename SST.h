
// DSH - 02/01/06 
// Templates for simple hash

#ifndef SST_H
#define SST_H

// Hash Size is equal to unsigned char - 255
#define hash_size 255 

#include <iostream>
#include <exception>

/*

Design:

	Will copy key for memloc reasons.
	Deletes data on token close.

*/

struct linear_bookmark {

	unsigned char placeset;
	unsigned int offset;
};

template <class J>
class tokens {
	
	// Tokens for table

	public:

	char *token_key;
	J token_content;

	tokens *next;
	
	tokens() {};
	tokens(char *token_key_in, J token_content_in, tokens *next_in=NULL) : token_key(token_key_in), token_content(token_content_in), next(next_in) {};
	~tokens() { delete token_key; delete token_content; }

};

template <class T>
class SST {

public:

SST(int max_entriesIn) : max_entries(max_entriesIn) {

	// Normal constructor
	
	debug=false; clean_init(); 
};

SST(int max_entriesIn, bool debugIn) : debug(debugIn), max_entries(max_entriesIn) {

	// Debug constructor

	clean_init();
};	

~SST() { 

	// Delete table on object close

	delete_whole_tables();

};

bool delete_tables() {

	// Public wrapper for delete_whole_table()

	return delete_whole_tables();
}

T add_to_hash_return_object(T add_object, char *key) {

	// Add to hash and return added object
	// Usefull when calling with something like (add_to_hash(new object(),"line"));

	if(add_to_hash(add_object,key)) return NULL;
	return add_object;
}

bool add_to_hash(T add_object, char *key) {

	// Add to hash

	using namespace std;

	unsigned char index=0; 
	if(objects_to_hash == NULL) return 1;

	-killall (index,key);

	if((add_to_hash_line(index,key,add_object)) == QUITERROR) return 1;

	return 0;
}

bool show_tables() {

	// Show Table properties

	using namespace std; 

	cout << endl;
	for (int i=0;i<hash_size;i++) {

		if (init[i]) { 
		
			int count=0;
			tokens<T> *token_root = objects_to_hash[i]; tokens<T> *token_ptr = token_root;

			cout << " -- Index " << i << " has ";
	 
			while(token_ptr != NULL) { count++; token_ptr=token_ptr->next; }
			token_ptr=token_root;	
		
			cout << count; if(count > 1) { cout << " properties. "; } else { cout << " property. "; } cout << endl;
			
			while(token_ptr != NULL ) {  
				cout << "   -- Property ID String: " << token_ptr->token_key << endl;
				token_ptr=token_ptr->next;
			}
		} 
	}
	cout << "\nTable has " << entries << " indexes used." << endl << endl;

	return 0;
}

T get_object(char *key) {

	// Get object from key

	unsigned char index=0; T holder;
	
	get_key(index,key);
	holder = get_token_object(index,key);

	// returning null or internally crashes
	if(holder == NULL) { return NULL; } else { return holder; }
}

bool do_exist(char *key) {

	// Check if object exists

	unsigned char index=0;
	get_key(index,key);

	if((get_token_object(index,key)) != NULL) return 1; 

	return 0;
}

int delete_object(char *key) {

	// delete object from key

	using namespace std;

	unsigned char index=0;
	get_key(index,key);

	tokens<T> *token_root = objects_to_hash[index]; tokens<T> *token_ptr = token_root;
	tokens<T> *token_previous=NULL;

	try {
		while(token_ptr != NULL) {

			if(!strncmp(token_ptr->token_key,key,strlen(token_ptr->token_key))) { 
			
				if(token_previous != NULL && token_ptr->next != NULL) {
					token_previous->next = token_ptr->next; delete(token_ptr);

				} else if( token_previous == NULL && token_ptr->next != NULL) {
					objects_to_hash[index] = token_ptr->next; delete(token_ptr); 

				} else if( token_previous == NULL && token_ptr->next == NULL) {
					delete(token_ptr); init[index] = false; 
				}

				entries--; return 0;
			}

			token_previous = token_ptr;
			token_ptr=token_ptr->next;
		}

	} catch( exception &error ) {

		cout << "Std Error - delete_object(): " << error.what() << endl; return 1;
	}

	return 0;
}

int linear_search(linear_bookmark &bookmark, T &ret_object) {

	// To search through linear, bookmark holds pos and offset, ret_object is the return

	for (int i=(int)bookmark.placeset;i<hash_size;i++) {
		if(init[i]) { 

			tokens<T> *token_root = objects_to_hash[i]; tokens<T> *token_ptr = token_root;
			if(!bookmark.offset) for(int j=0;j<bookmark.offset;j++) token_ptr = token_ptr->next;

			ret_object = token_ptr->token_content; 
			if(token_ptr->next != NULL) { bookmark.offset+=1; } else { bookmark.placeset=i+1; } 

			return 0; 
		}
	}
	return 1;
}

private:

bool debug;
bool init[hash_size];
int max_entries;
int entries;

tokens<T> *objects_to_hash[hash_size];
enum return_values { GOOD=0, QUITERROR, CONTERROR };

return_values clean_init() {

	// Clean init[] and set entries to 0

	entries=0; 
	for(int i=0;i<hash_size;i++) init[i]=false;

	return GOOD;
}	

return_values check_duplication(unsigned char index, char *key) {

	// Check for duplicates

	if(!init[index]) return CONTERROR;

	tokens<T> *token_root = objects_to_hash[index]; tokens<T> *token_ptr = token_root;

	while(token_ptr != NULL) {

		if(!strncmp(token_ptr->token_key,key,strlen(token_ptr->token_key))) return QUITERROR;
		token_ptr = token_ptr->next;
	}

	return GOOD;
}

T get_token_object(unsigned char index, char *key) {

	// Get object from key

	if(!init[index]) return NULL;

	tokens<T> *token_root = objects_to_hash[index]; tokens<T> *token_ptr = token_root;

	while(token_ptr != NULL) {

		if(!strncmp(token_ptr->token_key,key,strlen(token_ptr->token_key))) return token_ptr->token_content;
		token_ptr=token_ptr->next;
	}

	return NULL;
}

return_values get_key(unsigned char &keyOut, char *key) {

	// Simple string addition algo.

	unsigned char index=0;

	if(key==NULL) return QUITERROR;
	while(*key) index += *key++;

	keyOut = index;
	return GOOD;
}

return_values add_to_hash_line(unsigned char index, char *key_in, T object_to_add) {

	// Add to hash lines

	using namespace std;
	int key_len = (strlen(key_in) + 1);	

	if((entries+1)>max_entries) { if (debug) cout << "Max entries reached." << endl; return CONTERROR; }

	// Copy key for safety, prone to overwritten key mem loc's
	// Copied key and data gets delete'ed on token delete

	char *key = new char[ key_len ];
	strncpy(key,key_in,key_len);

	try {
		if(!init[index]) {

			objects_to_hash[index] = new tokens<T>(key,object_to_add); init[index]=true;
			entries++;	
	
		} else {

			if(check_duplication(index,key)) { if(debug) cout << "Duplication of key, " << key << ", found. Returning error." << endl; 
				return CONTERROR; }

			tokens<T> *token_root = objects_to_hash[index]; tokens<T> *token_ptr = token_root;

			while(token_ptr->next) token_ptr=token_ptr->next; 
			token_ptr->next = new tokens<T>(key,object_to_add,NULL); 

		}; 

	} catch(exception &error) {
		
		cout << "Std Error - add_to_hash_line(): " << error.what() << endl; return QUITERROR;
	}	

	if(debug) show_tables();

	return GOOD;
}

return_values delete_hash_line(unsigned char index) {

	// Delete whole hash line at index

	using namespace std;

	if(!init[index]) return CONTERROR;

	try {
		tokens<T> *token_root = objects_to_hash[index]; tokens<T> *token_ptr = token_root;
		tokens<T> *token_previous;

		while(token_ptr != NULL) {

			token_previous=token_ptr;
			token_ptr = token_ptr->next;
 			delete token_previous; entries--;
		}; 

		init[index] = false;

	} catch(exception &error) {

		cout << "Std Error - delete_hash_line(): " << error.what() << endl; return QUITERROR;
	}

	return GOOD;
}	

return_values delete_whole_tables() {

	// Delete whole table

	for (int i=0;i<hash_size;i++) if(init[i]) if(delete_hash_line((unsigned char)i) == QUITERROR) return QUITERROR;	

	clean_init();
	return GOOD;
}

};

#endif //SST_H
