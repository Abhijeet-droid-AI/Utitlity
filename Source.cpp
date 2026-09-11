#include "Header.hpp"

void displayUsage(void);
map <int, std::map <std::string, std::string>> Read_input_file_1(char* fileName);
int checkObjectValidityForClassification(string ObjType, bool& isValidForClassification);
int getObject(const char* itemId, const char* pObjType, tag_t* tObj);
tag_t getItemOrRevToValidate(tag_t tObj, string ObjRevId);
int unclassifyObject(tag_t tObj, string classId);
int deleteObject(tag_t tObj, string ObjId);
std::vector<std::string_view> split(std::string_view buffer, const std::string_view delimiter);

stringstream ss;

int ITK_user_main(int argc, char* argv[])
{
	int iStatus = ITK_ok;
	int iUnUsed = 0;

	map <int, std::map <std::string, std::string>> ObjectsMap;

	const char* userid = ITK_ask_cli_argument("-u=");
	const char* password = ITK_ask_cli_argument("-p=");
	const char* group = ITK_ask_cli_argument("-g=");
	char* file = ITK_ask_cli_argument("-f=");
	char* Logfile = ITK_ask_cli_argument("-log=");

	if (userid == NULL || password == NULL || group == NULL || file == NULL || Logfile == NULL)
	{
		displayUsage();
		iStatus = !ITK_ok;
		return iStatus;
	}

	//read the csv file
	ObjectsMap = Read_input_file_1(file);

	//ITK auto login
	ITK(ITK_initialize_text_services(iUnUsed));
	iStatus = ITK_init_module(userid, password, group);
	logger = M_Logger(Logfile);
	logger.writebothlog("Start of Classification Delete Utility");

	if (iStatus == ITK_ok)
	{
		ITK_set_bypass(TRUE);

		logger.writebothlog("Login to Teamcenter is successful");

		for (auto& t : ObjectsMap)
		{
			logger.error_flag = 0;
			bool isValidForClassification = false;

			string
				ObjId,
				rev_id,
				classId,
				ObjType,
				strAction;

			tag_t
				tObj = NULLTAG;

			tag_t tLatestRev = NULLTAG;

			auto pt0 = t.second.find(ITEM_ID);
			ObjId.assign(pt0->second);

			auto pt1 = t.second.find(ITEM_REV_ID);
			rev_id.assign(pt1->second);

			auto pt2 = t.second.find(OBJECT_TYPE);
			ObjType.assign(pt2->second);

			auto pt3 = t.second.find(CLASS_ID);
			classId.assign(pt3->second);

			//ACTION column is optional -> UNCLASSIFY / DELETE_OBJECT (default)
			auto ptAction = t.second.find(ACTION_ID);
			if (ptAction != t.second.end())
			{
				strAction.assign(ptAction->second);
			}
			else
			{
				strAction.assign("DELETE_OBJECT");
			}

			logger.write("-----------------------------------------------------------------------------------------------------------------------");

			ss << "Processing " << "Item_id is -> " << pt0->second << "|" << "Item_revision_id is ->" << pt1->second << "|" << "object_type is -> " << pt2->second << "|" << "Icm class id is -> " << pt3->second << "|" << "Action is -> " << strAction << endl;
			logger.write(ss.str());
			ss.str("");
			std::cout << "Processing:: " << ObjId << "|" << rev_id << "|" << ObjType << "|" << classId << "|" << strAction << endl;

			//the classifiable types check is applied for the unclassify flow only,
			//deletion of an object does not depend on the classifiable types preference
			if (tc_strcasecmp(strAction.c_str(), "UNCLASSIFY") == 0)
			{
				checkObjectValidityForClassification(ObjType, isValidForClassification);

				if (!isValidForClassification)
				{
					ss << "ERROR: Object type -> " << ObjType << " is not valid for classification." << endl;
					logger.writefaillog(ss.str());
					ss.str("");
				}
			}
			else
			{
				isValidForClassification = true;
			}

			if (isValidForClassification)
			{
				getObject(ObjId.c_str(), ObjType.c_str(), &tObj);

				if (tObj != NULLTAG)
				{
					tLatestRev = getItemOrRevToValidate(tObj, rev_id);

					logger.write("INFO: Object is available in Teamcenter");

					logical isClassified = false;
					ITK(ICS_is_wsobject_classified(tLatestRev, &isClassified));

					if (isClassified == TRUE)
					{
						logger.write("INFO: Object is classified, removing the classification");

						int iUnclassifyStatus = unclassifyObject(tLatestRev, classId);

						if (iUnclassifyStatus == 0)
						{
							ss << "SUCCESS: Object unclassified successfully for object -> " << pt0->second << endl;
							logger.write(ss.str());
							ss.str("");
						}
						else
						{
							ss << "ERROR: Object unclassification is Failed for object -> " << pt0->second << endl;
							logger.writefaillog(ss.str());
							ss.str("");
						}
					}
					else
					{
						logger.write("INFO: Object is not classified");
					}

					if (tc_strcasecmp(strAction.c_str(), "DELETE_OBJECT") == 0)
					{
						logger.write("INFO: Starting Object deletion");

						int iDeleteStatus = deleteObject(tLatestRev, ObjId);

						if (iDeleteStatus == 0)
						{
							ss << "SUCCESS: Object deletion is completed successfully for object -> " << pt0->second << endl;
							logger.write(ss.str());
							ss.str("");
						}
						else
						{
							ss << "ERROR: Object deletion is Failed for object -> " << pt0->second << endl;
							logger.writefaillog(ss.str());
							ss.str("");
						}
					}
				}
				else
				{
					ss << "ERROR: Object is not available in Teamcenter, skip object deletion " << iStatus << endl;
					logger.writefaillog(ss.str());
					ss.str("");
				}
			}

			if (logger.error_flag == 1)
			{
				ss << "Processing:: " << ObjId << "|" << rev_id << "|" << ObjType << "|" << classId << endl;
				logger.writefaillog(ss.str());
				ss.str("");
				logger.writefaillog("-----------------------------------------------------------------------------------------------------------------------");
			}
		}

		iStatus = ITK_exit_module(true);
		if (iStatus == ITK_ok)
		{
			logger.writebothlog("-----------------------------------------------------------------------------------------------------------------------");
			logger.writebothlog("Teamcenter session is closed.");
		}
	}
	else
	{
		logger.writebothlog("Login to Teamcenter Failed..");
	}

	return iStatus;
}

void displayUsage(void)
{
	std::cout << "\n +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	std::cout << "\n Usage : " << endl;
	std::cout << "xxxxxx" << "  -u=<userid> -p=<passwd> -g=<group> -f=<input file> -log=<log file> " << "\t [-h | help] Displays this usage information" << endl << endl;
	std::cout << "\n +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
}

int checkObjectValidityForClassification(string ObjType, bool& isValidForClassification)
{
	int iStatus = ITK_ok;
	int valCount = 0;
	scoped_smptr <char*> prefValues;

	ITK(PREF_ask_char_values(PREF_ICS_CLASSIFIABLE_TYPES, &valCount, &prefValues));

	if (valCount > 0)
	{
		for (int xx = 0; xx < valCount; xx++)
		{
			if (tc_strncasecmp(ObjType.c_str(), prefValues[xx], ObjType.length()) == 0)
			{
				isValidForClassification = true;
			}
		}
	}

	return 0;
}

int unclassifyObject(tag_t tObj, string classId)
{
	int iStatus = ITK_ok;
	int iFailedCount = 0;

	tag_t relation = NULLTAG;
	int ifail = 0;
	int count = 0;
	tag_t* classificationObjects = NULL;

	ifail = GRM_find_relation_type("IMAN_classification", &relation);
	if (ifail == ITK_ok)
	{
		ifail = GRM_list_secondary_objects_only(tObj, relation, &count, &classificationObjects);

		if (ifail == ITK_ok)
		{
			if (count == 0)
			{
				logger.write("INFO: No classification objects found on the object");
			}

			for (int i = 0; i < count; i++)
			{
				tag_t tagOfObjectClass = NULLTAG;
				char* ObjectclassId = NULL;
				char* ObjectclassName = NULL;

				ITK(ICS_ask_class_of_classification_obj(classificationObjects[i], &tagOfObjectClass));

				if (tagOfObjectClass != NULLTAG)
				{
					ITK(ICS_ask_id_name(tagOfObjectClass, &ObjectclassId, &ObjectclassName));
				}

				//if a class id is supplied, unclassify only from that class, otherwise unclassify from all classes
				bool isMatchingClass = true;

				if (tc_strcmp(classId.c_str(), "") != 0 && tc_strcasecmp(classId.c_str(), "none") != 0)
				{
					isMatchingClass = (ObjectclassId != NULL && tc_strcasecmp(ObjectclassId, classId.c_str()) == 0);
				}

				if (isMatchingClass)
				{
					ss << "INFO: Removing classification object of class -> " << ObjectclassId << " (" << ObjectclassName << ")" << endl;
					logger.write(ss.str());
					ss.str("");

					ifail = GRM_delete_relation(tObj, relation, classificationObjects[i]);

					if (ifail != ITK_ok)
					{
						char* pcError = NULL;
						EMH_ask_error_text(ifail, &pcError);
						ss << "ERROR: Removing classification relation is Failed " << ifail << " " << pcError << endl;
						logger.writefaillog(ss.str());
						ss.str("");
						SAFE_MEM_FREE(pcError);
						iFailedCount++;
					}
				}
				else
				{
					ss << "INFO: Skipping classification object of class -> " << ObjectclassId << " as it does not match the supplied class -> " << classId << endl;
					logger.write(ss.str());
					ss.str("");
				}
			}

			if (count > 0)
			{
				ITK(AOM_save(tObj));

				//verify the classification status after removal
				logical isClassified1 = false;
				ITK(ICS_is_wsobject_classified(tObj, &isClassified1));

				if (isClassified1 == TRUE)
				{
					ss << "ERROR: Object is still classified after removing the classification relation(s)" << endl;
					logger.writefaillog(ss.str());
					ss.str("");
					iFailedCount++;
				}
			}
		}
		else
		{
			char* pcError = NULL;
			EMH_ask_error_text(ifail, &pcError);
			ss << "ERROR: Listing classification objects is Failed " << ifail << " " << pcError << endl;
			logger.writefaillog(ss.str());
			ss.str("");
			SAFE_MEM_FREE(pcError);
			iFailedCount++;
		}
	}
	else
	{
		char* pcError = NULL;
		EMH_ask_error_text(ifail, &pcError);
		ss << "ERROR: Relation type IMAN_classification not found " << ifail << " " << pcError << endl;
		logger.writefaillog(ss.str());
		ss.str("");
		SAFE_MEM_FREE(pcError);
		iFailedCount++;
	}

	SAFE_MEM_FREE(classificationObjects);

	return iFailedCount;
}

int deleteObject(tag_t tObj, string ObjId)
{
	int iStatus = ITK_ok;
	int iFailedCount = 0;

	try
	{
		//lock the object before deletion
		ITK(AOM_refresh(tObj, TRUE));

		iStatus = AOM_delete(tObj);

		if (iStatus == ITK_ok)
		{
			iStatus = AOM_save(tObj);
		}

		if (iStatus == ITK_ok)
		{
			ss << "INFO: Object -> " << ObjId << " is deleted from Teamcenter" << endl;
			logger.write(ss.str());
			ss.str("");
		}
		else
		{
			char* pcError = NULL;
			EMH_ask_error_text(iStatus, &pcError);
			ss << "ERROR: Object deletion is Failed " << iStatus << " " << pcError << endl;
			logger.writefaillog(ss.str());
			ss.str("");
			SAFE_MEM_FREE(pcError);
			iFailedCount++;
		}
	}
	catch (int iCaughtStatus)
	{
		char* pcError = NULL;
		EMH_ask_error_text(iCaughtStatus, &pcError);
		ss << "ERROR: Object deletion is Failed " << iCaughtStatus << " " << pcError << endl;
		logger.writefaillog(ss.str());
		ss.str("");
		SAFE_MEM_FREE(pcError);
		iFailedCount++;
	}

	return iFailedCount;
}

int getObject(const char* itemId, const char* pObjType, tag_t* tObj)
{
	int nObjs = 0;
	int iStatus = ITK_ok;
	tag_t* tObjs = NULL;
	char* cObjType = NULL;

	const char
		*names[2] = { ITEM_ID , OBJECT_TYPE },
		*values[2] = { itemId , pObjType };

	ITK(ITEM_find_items_by_key_attributes(2, names, values, &nObjs, &tObjs));

	if (nObjs > 0)
	{
		for (int ss = 0; ss < nObjs; ss++)
		{
			ITK(AOM_ask_value_string(tObjs[ss], OBJECT_TYPE, &cObjType));

			if (tc_strcmp(pObjType, cObjType) == 0)
			{
				*tObj = tObjs[ss];
			}
		}
	}

	return 0;
}

std::vector<std::string_view> split(std::string_view buffer, const std::string_view delimiter)
{
	std::vector<std::string_view> result;
	std::string_view::size_type pos;

	while ((pos = buffer.find(delimiter)) != std::string_view::npos)
	{
		auto match = buffer.substr(0, pos);
		result.push_back(match);
		buffer.remove_prefix(pos + delimiter.size());
	}

	result.push_back(buffer);

	return result;
}

map <int, std::map <std::string, std::string>> Read_input_file_1(char* fileName)
{
	ifstream MyFile;
	map <int, std::map <std::string, std::string>> ObjectsMap;

	MyFile.open(fileName, ios::in);
	if (!MyFile.is_open())
	{
		std::cout << "Failed to open the Input file" << endl;
	}
	else
	{
		std::cout << "Input File opened successfully" << endl;

		int count = 0;
		string line;
		map< int, string> HeadersMap;

		while (getline(MyFile, line, '\n'))
		{
			count++;
			int AttrCount = 0;
			string linevec;

			std::map <std::string, std::string> PropMap;
			auto split_values = split(line, "~##");

			for (size_t i = 0; i < split_values.size(); ++i)
			{
				AttrCount++;

				if (count == 1)
				{
					HeadersMap.insert(std::pair< int, string>(AttrCount, split_values[i]));
				}
				else
				{
					auto it3 = HeadersMap.find(AttrCount);
					PropMap.insert(std::pair<string, string>(it3->second, split_values[i]));
				}
			}

			if (count > 1)
			{
				ObjectsMap.insert(std::pair<int, std::map <std::string, std::string>>(count, PropMap));
			}
		}
	}

	return ObjectsMap;
}

tag_t getItemOrRevToValidate(tag_t tObj, string ObjRevId)
{
	scoped_smptr<char> rev_id;
	scoped_smptr<char> item_id;
	int iStatus = ITK_ok;
	tag_t obj_rev = NULLTAG;

	ITK(ITEM_ask_id2(tObj, &item_id));

	if (ObjRevId.empty())
	{
		std::cout << "Processing object ::" << item_id.getString() << endl;
		return tObj;
	}
	else if (tc_strcasecmp(ObjRevId.c_str(), "none") == 0)
	{
		std::cout << "Processing object ::" << item_id.getString() << endl;
		return tObj;
	}
	else if (tc_strcasecmp(ObjRevId.c_str(), "last") == 0)
	{
		ITK(ITEM_ask_latest_rev(tObj, &obj_rev));
	}
	else
	{
		ITK(ITEM_find_revision(tObj, ObjRevId.c_str(), &obj_rev));
	}

	iStatus = ITEM_ask_rev_id2(obj_rev, &rev_id);
	if (iStatus != ITK_ok)
	{
		ss << "ERROR: Revision is not available in Teamcenter,skipping Object deletion " << endl;
		logger.writefaillog(ss.str());
		ss.str("");
	}
	else
	{
		std::cout << "Processing object ::" << item_id.getString() << "\\" << rev_id.get() << endl;
	}

	return obj_rev;
}
