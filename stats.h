
#ifndef __STATS_H__
#define __STATS_H__

#include "serrors.h"
#include "config.h"
#include "global.h"

#if ( DATABASE_TYPE == 1 )
	#include <db.h>
#endif

#include <iostream>

class soulS {

public:

soulS();
~soulS();

bool print_stats();

private:


};

#endif /* __STATS_H_ */
