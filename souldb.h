#ifndef __SOUL_DB__
#define __SOUL_DB__

#include "base.h"
#include "serrors.h"
#include "config.h"
#include "global.h"

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>

#if ( DATABASE_TYPE == 1 )
	#include <db.h>
#endif

class soulDB : public soulB {
	
	private:

#if ( DATABASE_TYPE == 1 )
		
	DB *domptr;
	DB *urlptr; 
	
#endif

	bool DEBUG_STATE;

	public:
	
	soulDB() {};
	~soulDB() {};
	
#if ( DATABASE_TYPE == 1 )

	bool create_DB(databases, const char *, const char *,char *, u_int32_t);
	bool open_DB(char *,databases, const char *, DB *&, u_int32_t);
	bool close_DB(DB *&);
	bool clean_DB(databases, const char *);
	bool find_match(DB *&, char *, int);
	
#endif

};

#endif /* SOUL_DB */
