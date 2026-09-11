#ifndef STANDARD_DEFINES_H
#define STANDARD_DEFINES_H

// C Standard
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// C++ Standard
#include <algorithm>
#include <ctime>
#include <codecvt>
#include <exception>
#include <fstream>			// Header providing file stream classes
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <ctime>



// Windows Standard
#include <tchar.h>
#include <windows.h>
#include <direct.h>
#include <ShlObj.h>

//	ITK Standard

#include <base_utils/Mem.h>

#include <bom/bom.h>

#include <cfm/cfm.h>

#include <fclasses/tc_string.h>

#include <ics/ics2.h>

#include <itk/te.h>
#include <tc/tc_startup.h>
#include <ae/datasettype.h>
#include <ae/dataset.h>
#include <qry/qry.h>

#include <property/nr.h>
#include <property/nr_errors.h>

#include <sa/am.h>
#include <sa/sa.h>

#include <server_exits/user_server_exits.h>

#include <ss/ss_const.h>

#include <sys/timeb.h>

#include <tc/emh.h>
#include <tc/preferences.h>
#include <tc/tc_startup.h>
#include <tc/tc_util.h>

#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <tccore/part.h>
#include <tccore/workspaceobject.h>

#include <textsrv/textserver.h>

#include <user_exits/epm_toolkit_utils.h>

#define SCOPE							TC_preference_site
#define bomAttr_levelZero				"bl_level_starting_0"
#define	bomAttr_itemRevOwningUser		"bl_rev_owning_user"


#define CONST_BLANK_VAL ""
#define TEMP_DIR_VAR "TEMP"
#define WIN_SEPERATOR "\\"
#define NON_WIN_SEPERATOR "//"
#define LOG_EXT ".log"
#define COMMA  "," 
#define HEADER_STRING "\n"+"**************************************************"
#define CONST_ITEM_ID_ATTR "item_id"
#define CONST_VALUE_ONE 1

using namespace std;

#define ITK(x)																												 \
{																															 \
    if ( (status = (x)) != ITK_ok )																							 \
    {																														 \
            char *error_str = NULL;																							 \
            EMH_ask_error_text ( status, &error_str );																		 \
            TC_write_syslog ( "ERROR: %d, ERROR MSG: %s. at Line: %d in File: %s\n", status, error_str, __LINE__, __FILE__ );\
            MEM_free ( error_str );																							 \
    }																														 \
}

#define SAFE_MEM_FREE( a )  \
if ( a != NULL )		\
{                       \
    MEM_free( a );		\
	a = NULL;			\
}

#define SAFE_MEM_FREE_ARRAY(p, count) {		\
   if ( p != NULL ) {						\
        for(int z = 0; z < count; z++) {	\
            if(p[z] != NULL) {				\
                MEM_free(p[z]);				\
                p[z] = NULL;				\
            }								\
        }									\
        MEM_free(p);						\
        p = NULL;							\
   }										\
}

#define FAIL_LOG						\
toolLogger_->writeln( ossLog.str() );	\
toolLogger_->logError(status);			\
failureLogger_->writeln( ossLog.str() );\
failureLogger_->logError(status)


//static char * stringToChar(string sName);


#endif#pragma once
