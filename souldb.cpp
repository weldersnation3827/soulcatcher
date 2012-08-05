#include "souldb.h"

/* Database Types

	1 - Berkeley Database 
	2 - Another

*/

#if ( DATABASE_TYPE == 1 )

bool soulDB::create_DB(databases wanted, const char *db_path, const char *text_path, char *acl, u_int32_t db_flags) {

	/* create Berk DB */

	fstream DB_text; int records=0; 

	cout << "\nCreating " << database_names[wanted] << ": "  << acl << endl;
	cout << "\n - Cleaning database of " << database_names[wanted] << "." << endl;
	
	if(clean_DB(wanted,db_path)) return 1;
	
	DB_text.open(text_path,ios::in);
	if (!DB_text.good()) { stdout_log(ERR,serror10,text_path); return 1; }

	cout << " - Creating records now ..." << endl;

	int err; db_recno_t recno=1; char line_buffer[_1K]; DBT recordnum, url;
	
	DB *db_ptr = domptr; if (wanted) db_ptr = urlptr;
	if(open_DB(acl,wanted,db_path,db_ptr,db_flags)) return 1;
	
	while (!DB_text.eof()) {

		DB_text.getline(line_buffer,sizeof(line_buffer));
		if (*line_buffer == '\0' || *line_buffer == '#') continue;

		memset(&recordnum,0,sizeof(recordnum)); memset(&url,0,sizeof(url));
		recordnum.data = (void *)line_buffer; recordnum.size = strlen(line_buffer);
		url.data = (void *)""; url.size = 1; records++;

		if ((err = db_ptr->put(db_ptr, NULL, &recordnum, &url,0)) != 0) {
			stdout_log(ERR,serror29); return 1; }
	}
	
	close_DB(db_ptr);

	cout << "\n"  << acl << " created.. " << records << " records." << endl;

	DB_text.close(); 
	return 0;
}

bool soulDB::clean_DB(databases wanted, const char *destination) {

	/* Clean berk DB */

	int err; DB *temp;

	if((err = db_create(&temp, NULL, 0))!=0) return 1;
	if(err = temp->remove(temp,destination,database_names[wanted],0)) { 
		switch (err) {
			case 2: cout << serror30 << endl; return 0;
                       	default: temp->err(temp,err, "%s", "clean_db error");
		}
	}

	return 0;
}

bool soulDB::open_DB(char *acl, databases want, const char *dbpath, DB *&db_ptr, u_int32_t flags) {

	/* open berk DB */

        int err;
	const char *db_name = database_names[want];

	if ((err = db_create(&db_ptr, NULL, 0))!=0) return 1;
        if ((err = db_ptr->open(db_ptr,NULL, dbpath, db_name, DB_BTREE, flags, 0600))!=0) {
                switch (err) {
			case 2: stdout_log(ERR,serror31,db_name,acl); return 1;
                       	default: db_ptr->err(db_ptr,err, "%s", "db_open error");
		}
	}

	return 0;
}

bool soulDB::close_DB(DB *&db_ptr) {
	
	/* Close berk DB */

	int err;
        if ((err=db_ptr->close(db_ptr,0)) != 0) {
		switch (err) {
                	default: db_ptr->err(db_ptr, err, "%s", "db_close error"); return 1;
		}
	}

	return 0;
}

bool soulDB::find_match(DB *&db_ptr, char *wanted_keyword, int keyword_size) {

	/* Find match in Berk DB */

	int err; db_recno_t recno=1; DBT recordnum, url; 
	
	memset(&recordnum,0,sizeof(recordnum)); memset(&url,0,sizeof(url));
	recordnum.data = wanted_keyword; recordnum.size = keyword_size;

	switch (db_ptr->get(db_ptr,NULL,&recordnum,&url,0)) {
		case 0: return 1;
 		// case DB_NOTFOUND: break; // if (DEBUG_STATE) db_io->log(DEBUG,"Key/pair not found in %i database",ud); break;
		// default: break; // db_io->log(ERR,"Cursor in domain_match produced an error"); break;
	}
	
	return 0;
} 

#endif
