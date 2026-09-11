#ifndef STANDARD_DEFINES_H
#define STANDARD_DEFINES_H

//C C++ standard
#include<iostream>
#include<fstream>
#include<istream>
#include<stdio.h>
#include<string.h>
#include<vector>
#include<sstream>
#include<map>
#include<iterator>
#include<algorithm>
#include<time.h>


//	ITK Standard
#include <tcinit/tcinit.h>
#include <base_utils/Mem.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <fclasses/tc_string.h>
#include <ics/ics.h>
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
#include <user_exits/epm_toolkit_utils.h>
#include <sys/timeb.h>
#include <tc/folder.h>
#include <tc/emh.h>
#include <tc/preferences.h>
#include <tc/tc_util.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <tccore/part.h>
#include <tccore/workspaceobject.h>
#include <fclasses\tc_date.h>
#include <textsrv/textserver.h>
#include <res/res_itk.h>
#include <res/reservation.h>
//#include<Cls0classification/cls_itk.h>
//#include <Cls0classification/libcls0classification_exports.h>

using namespace std;
#include<base_utils\ScopedSmPtr.hxx>
#include <base_utils/IFail.hxx>
#include "M_Logger.hpp" 

//using namespace std;
using namespace Teamcenter;

#define ITEM_ID "item_id"
#define ITEM_REV_ID "item_revision_id"
#define OBJECT_TYPE "object_type"
#define OBJECT_NAME "object_name"
#define OBJECT_SUFFIX "m4suffix"
#define CLASS_ID "class_id"
#define SEARCH_INDEX_VIEW "SearchIndexView"
#define PREF_ICS_CLASSIFIABLE_TYPES "ICS_classifiable_types"
#define M4M_PART_TYPE "M4MPart"
#define M4M_STUDY_TYPE "M4MStudy"
#define M4M_CUSTOMER_TYPE "M4MCustomer"
#define M4M_COMMERCIAL_TYPE "M4MCommercial"
#define M4M_DRAWING_TYPE "M4MDrawing"

#define ITK(x)																													\
{																																\
    if ( (iStatus = (x)) != ITK_ok )																							\
    {																															\
            char *error_str = NULL;																								\
            EMH_ask_error_text ( iStatus, &error_str );																			\
            TC_write_syslog ( "ERROR: %d, ERROR MSG: %s. at Line: %d in File: %s\n", iStatus, error_str, __LINE__, __FILE__ );	\
            MEM_free ( error_str );																								\
    }																															\
}	

#define SAFE_MEM_FREE( a )  \
do                          \
{                           \
    if ( a != NULL )		\
    {                       \
        MEM_free( a );		\
		a = NULL;			\
    }                       \
}                           \
while ( 0 )																																														\



#endif
