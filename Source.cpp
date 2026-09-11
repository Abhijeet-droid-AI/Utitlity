#include "Header.hpp"

void displayUsage(void);
//map <int, std::map <std::string, std::string>> Read_input_file(char *fileName);
map <int, std::map <std::string, std::string>> Read_input_file_1(char* fileName);
int checkObjectValidityForClassification(string ObjType, bool& isValidForClassification);
//int getObject(const char *item_id, const char *suffix, const char *ObjType, tag_t *tObj);
int setClassAttributes(map <std::string, std::string> objData, tag_t tClass, tag_t tClassificationObj,string& FailedAttributes);
int getObject(const char* itemId, const char* pObjType, tag_t* tObj);
tag_t getItemOrRevToValidate(tag_t tObj, string ObjRevId);
int updateClassAttributes(const char* clsAttributeName, const char* clsAttributeValue, int& FailedAttrCount, string& FailedAttributes, tag_t tClassificationObj);
stringstream ss;

int ITK_user_main(int argc, char *argv[])
{
	int iStatus = ITK_ok;
	int iUnUsed = 0;
	
	map <int, std::map <std::string, std::string>> ObjectsMap;	

	const char* userid = ITK_ask_cli_argument("-u=");
	const char* password = ITK_ask_cli_argument("-p=");
	const char* group = ITK_ask_cli_argument("-g=");
	char* file = ITK_ask_cli_argument("-f=");
	char* Logfile = ITK_ask_cli_argument("-log=");

	if (userid == NULL || password == NULL || group == NULL || file == NULL  || Logfile == NULL)
	{
		displayUsage();
		iStatus = !ITK_ok;
		return iStatus;

	}
	//read the csv file
	
	//ObjectsMap = Read_input_file(file);
	/******Modification by suwarna start*****/

	ObjectsMap = Read_input_file_1(file);
	/******Modification by suwarna end*****/
	//ITK auto login
	ITK(ITK_initialize_text_services(iUnUsed));
	iStatus = ITK_init_module(userid, password, group);
 	logger = M_Logger(Logfile);
	logger.writebothlog("Start of Classification Utility");
	if (iStatus == ITK_ok)
	{
		ITK_set_bypass(TRUE);

		logger.writebothlog("Login to Teamcenter is successful");
		int LineNumber = 0;
		for (auto& t : ObjectsMap)
		{
			//LineNumber++;
			int nRevs;
			logger.error_flag = 0;
			bool isValidForClassification = false;

			string
				ObjId,
				rev_id,
				classId,
				ObjType;

			tag_t
				tClass = NULLTAG,
				tObj = NULLTAG,
				tClassificationObj = NULLTAG;

			scoped_smptr <tag_t> obj_revs;

			tag_t tLatestRev = NULLTAG;


			logical isClassified;
			logical isClassified1;

			auto pt0 = t.second.find(ITEM_ID);

			ObjId.assign(pt0->second);

			auto pt1 = t.second.find(ITEM_REV_ID);

			rev_id.assign(pt1->second);

			auto pt2 = t.second.find(OBJECT_TYPE);

			ObjType.assign(pt2->second);

			auto pt3 = t.second.find(CLASS_ID);

			classId.assign(pt3->second);



			//ITK(ITEM_find_item(ObjId.c_str(), &tObjs));

			logger.write("-----------------------------------------------------------------------------------------------------------------------");


			checkObjectValidityForClassification(ObjType, isValidForClassification);

			if (isValidForClassification)
			{
				logger.write("-----------------------------------------------------------------------------------------------------------------------");
				ss << "Processing " << "Item_id is -> " << pt0->second << "|" << "Item_revision_id is ->" << pt1->second << "|" << "object_type is -> " << pt2->second << "|" << "Icm class id is -> " << pt3->second << endl;
				logger.write(ss.str());
				ss.str("");
				std::cout << "Processing:: " << ObjId << "|" << rev_id << "|" << ObjType << "|" << classId << endl;
				getObject(ObjId.c_str(), ObjType.c_str(), &tObj);
				if (tObj != NULLTAG)
				{
					tLatestRev = getItemOrRevToValidate(tObj, rev_id);

					logger.write("INFO: Object is available in Teamcenter");

					ITK(ICS_is_wsobject_classified(tLatestRev, &isClassified));//obj_revs[iz]
					/******Modification by suwarna start*****/
					if (isClassified == TRUE)
					{
						logger.write("INFO: Object is already classified, checking the class Type/attribues for updation..");
						tag_t tClassifiedObject = NULLTAG;
						tag_t tClasss = NULLTAG;
						tag_t tclassificationObject = NULLTAG;
						tag_t tClassificationObj1 = NULLTAG;
						tag_t tclassifiedclass = NULLTAG;
						char* classid = NULL;
						tag_t tclassification_object_tags = NULLTAG;
						int num_classification_objects;

						vector<tag_t> vectorOfObjectTag;
						tag_t relation = NULLTAG;
						int ifail = 0;
						int count = 0;
						tag_t classificationObject = NULLTAG;
						tag_t* classificationObjects = NULL;
						tag_t tagOfObjectClass = NULLTAG;
						char* ObjectclassId = NULL;
						char* ObjectclassName = NULL;

						ifail = GRM_find_relation_type("IMAN_classification", &relation);
						if (ifail == ITK_ok)
						{

							ifail = GRM_list_secondary_objects_only(tLatestRev, relation, &count, &classificationObjects);
							vectorOfObjectTag.push_back(tLatestRev);

						}
						ITK(ICS_ask_classification_object(tLatestRev, &tclassificationObject));
						if (tclassificationObject != NULLTAG)
						{
							bool alreadyInTargetClass = false;

							for (int i = 0; i < count; i++)
							{
								if (tclassificationObject != NULLTAG)
									ITK(ICS_ask_class_of_classification_obj(classificationObjects[i], &tagOfObjectClass));
								if (tagOfObjectClass != NULLTAG)
									ITK(ICS_ask_id_name(tagOfObjectClass, &ObjectclassId, &ObjectclassName));
								//cout << "class name is:" << ObjectclassName << endl;
								if (strcmp(ObjectclassId, classId.c_str()) == 0)
								{
									alreadyInTargetClass = true;
									ITK(ICS_find_class(classId.c_str(), &tClasss));
									//ITK(ICS_ask_classification_object(tLatestRev, &tclassificationObject));
									//ITK(ICS_ask_class_of_classification_obj(tclassificationObject, &tclassifiedclass));
									//ITK(ICS_ask_classification_object_id(tclassificationObject, &classid));
									ITK(ICS_ico_ask_classified_object(tClasss, &tclassification_object_tags));

									if (tClasss != NULLTAG && tagOfObjectClass == tClasss)
									{
										ICS_ask_classification_object(tLatestRev, &tClassifiedObject);
										string FailedAttributes;

										int cnt = setClassAttributes(t.second, tClasss, tClassifiedObject, FailedAttributes);

										if (cnt == 0)
										{
											ss << "SUCCESS: Object classification with all attributes is completed successfully for object -> " << pt0->second << endl;
											logger.write(ss.str());
											ss.str("");
										}
										else
										{
											ss << "ERROR: Object classification is Failed for object -> " << pt0->second << " " << " for -> " << FailedAttributes << " attributes." << endl;
											logger.writefaillog(ss.str());
											ss.str("");
										}

									}
								}
							}
							if (!alreadyInTargetClass)
							{
								ITK(ICS_find_class(classId.c_str(), &tClass));

								if (tClass != NULLTAG)
								{
									ITK(ICS_create_classification_object(tLatestRev, ObjId.c_str(), tClass, &tClassificationObj));

									if (tClassificationObj != NULLTAG)
									{
										ITK(ICS_classify_wsobject(tLatestRev, tClassificationObj));

									}
									else
									{
										ss << "ERROR: Class-> " << classId.c_str() << " error in classification object creation " << iStatus << endl;
										logger.writefaillog(ss.str());
										ss.str("");

									}
								}
								else
								{
									ss << "ERROR: Class-> " << classId.c_str() << " is not available in Teamcenter,skipping Object classification " << iStatus << endl;
									logger.writefaillog(ss.str());
									ss.str("");

								}

								ITK(ICS_is_wsobject_classified(tLatestRev, &isClassified1));

								if (isClassified1 == TRUE)
								{
									ss << "Object classification is completed successfully for object -> " << pt0->second << endl;
									logger.write(ss.str());
									ss.str("");
									string FailedAttributes;

									int cnt = setClassAttributes(t.second, tClass, tClassificationObj, FailedAttributes);

									if (cnt == 0)
									{
										ss << "SUCCESS: Object classification with all attributes is completed successfully for object -> " << pt0->second << endl;
										logger.write(ss.str());
										ss.str("");
									}
									else
									{
										ss << "ERROR: Object classification is Failed for object -> " << pt0->second << " " << " for -> " << FailedAttributes << " attributes." << endl;
										logger.writefaillog(ss.str());
										ss.str("");
									}

									//logger.write("-----------------------------------------------------------------------------------------------------------------------");

								}
								else
								{
									ss << "ERROR : Object classification is Failed for object -> " << pt0->second << " " << iStatus << endl;
									logger.writefaillog(ss.str());
									ss.str("");
								}
							}

						}
								
						
					}

					/*ITK(ICS_find_class(classId.c_str(), &tClasss));
					//ITK(ICS_ask_classification_object(tLatestRev, &tclassificationObject));
					//ITK(ICS_ask_class_of_classification_obj(tclassificationObject, &tclassifiedclass));
					//ITK(ICS_ask_classification_object_id(tclassificationObject, &classid));
					//ITK(ICS_ico_ask_classified_object(tClasss, &tclassification_object_tags));

					/*if (tClasss != NULLTAG && tclassifiedclass != tClasss)
					{

						ITK(ICS_create_classification_object(tLatestRev, ObjId.c_str(), tClasss, &tClassificationObj1));

						if (tClassificationObj1 != NULLTAG)
						{
							ITK(ICS_classify_wsobject(tLatestRev, tClassificationObj1));

						}
						else
						{
							ss << "ERROR: Class-> " << classId.c_str() << " error in classification object creation " << iStatus << endl;
							logger.writefaillog(ss.str());
							ss.str("");

						}

						ITK(ICS_is_wsobject_classified(tLatestRev, &isClassified1));

						if (isClassified1 == TRUE)
						{
							ss<<"Object classification is completed successfully for object -> " << pt0->second << endl;
							logger.write(ss.str());
							ss.str("");
							string FailedAttributes;

							int cnt = setClassAttributes(t.second, tClass, tClassificationObj1, FailedAttributes);

							if (cnt == 0)
							{
								ss <<"SUCCESS: Object classification with all attributes is completed successfully for object -> " << pt0->second << endl;
								logger.write(ss.str());
								ss.str("");
							}
							else
							{
								ss <<"ERROR: Object classification is Failed for object -> " << pt0->second << " " << " for -> " << FailedAttributes << " attributes." << endl;
								logger.writefaillog(ss.str());
								ss.str("");
							}

							//logger.write("-----------------------------------------------------------------------------------------------------------------------");

						}
						else
						{
							ss <<"ERROR : Object classification is Failed for object -> " << pt0->second << " " << iStatus << endl;
							logger.writefaillog(ss.str());
							ss.str("");
						}

						//logger.write("-----------------------------------------------------------------------------------------------------------------------");
					}*/
					/*else if (tClasss != NULLTAG && tclassifiedclass == tClasss)
					{
						ICS_ask_classification_object(tLatestRev, &tClassifiedObject);
						string FailedAttributes;

						int cnt = setClassAttributes(t.second, tClasss, tClassifiedObject, FailedAttributes);

						if (cnt == 0)
						{
							ss <<"SUCCESS: Object classification with all attributes is completed successfully for object -> " << pt0->second << endl;
							logger.write(ss.str());
							ss.str("");
						}
						else
						{
							ss <<"ERROR: Object classification is Failed for object -> " << pt0->second << " " << " for -> " << FailedAttributes << " attributes." << endl;
							logger.writefaillog(ss.str());
							ss.str("");
						}

						//logger.write("-----------------------------------------------------------------------------------------------------------------------");


					}
					else
					{
						ss <<"ERROR: Class-> " << classId.c_str() << " is not available in Teamcenter,skipping Object classification " << iStatus << endl;
						logger.writefaillog(ss.str());
						ss.str("");

					}
					/******Modification by suwarna end
				}*/
					else
					{
						logger.write("INFO: Object is not classified, Starting Object classification");

						ITK(ICS_find_class(classId.c_str(), &tClass));

						if (tClass != NULLTAG)
						{
							ITK(ICS_create_classification_object(tLatestRev, ObjId.c_str(), tClass, &tClassificationObj));

							if (tClassificationObj != NULLTAG)
							{
								ITK(ICS_classify_wsobject(tLatestRev, tClassificationObj));

							}
							else
							{
								ss << "ERROR: Class-> " << classId.c_str() << " error in classification object creation " << iStatus << endl;
								logger.writefaillog(ss.str());
								ss.str("");

							}
						}
						else
						{
							ss << "ERROR: Class-> " << classId.c_str() << " is not available in Teamcenter,skipping Object classification " << iStatus << endl;
							logger.writefaillog(ss.str());
							ss.str("");

						}

						ITK(ICS_is_wsobject_classified(tLatestRev, &isClassified1));

						if (isClassified1 == TRUE)
						{
							ss << "Object classification is completed successfully for object -> " << pt0->second << endl;
							logger.write(ss.str());
							ss.str("");
							string FailedAttributes;

							int cnt = setClassAttributes(t.second, tClass, tClassificationObj, FailedAttributes);

							if (cnt == 0)
							{
								ss << "SUCCESS: Object classification with all attributes is completed successfully for object -> " << pt0->second << endl;
								logger.write(ss.str());
								ss.str("");
							}
							else
							{
								ss << "ERROR: Object classification is Failed for object -> " << pt0->second << " " << " for -> " << FailedAttributes << " attributes." << endl;
								logger.writefaillog(ss.str());
								ss.str("");
							}

							//logger.write("-----------------------------------------------------------------------------------------------------------------------");

						}
						else
						{
							ss << "ERROR : Object classification is Failed for object -> " << pt0->second << " " << iStatus << endl;
							logger.writefaillog(ss.str());
							ss.str("");
						}
					}


				}
				else
				{
					ss << "ERROR: Object is not available in Teamcenter, skip object classification " << iStatus << endl;
					logger.writefaillog(ss.str());
					ss.str("");
				}
			}
			else
			{
				ss << "ERROR: Object type -> " << ObjType << " is not valid for classification." << endl;
				logger.writefaillog(ss.str());
				ss.str("");
			}
			if (logger.error_flag == 1)
			{
				//logger.writefaillog("-----------------------------------------------------------------------------------------------------------------------");
				ss << "Processing:: " << ObjId << "|" << rev_id << "|" << ObjType << "|" << classId << endl;
				//ss << "Processed :: " << pObjId.c_str() << "@" << pObjRevId.c_str() << "@" << pObjectType.c_str() << endl;
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

	std::cout << "xxxxxx" << "  -u=<userid> -p=<passwd> -g=<group> -f=<input file> -log=<log file> "<<"\t [-h | help] Displays this usage information"<< endl<<endl;

	std::cout << "\n +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
}

int checkObjectValidityForClassification(string ObjType, bool &isValidForClassification)
{
	int iStatus = ITK_ok;
	int valCount = 0;
	scoped_smptr <char*> prefValues;

	ITK(PREF_ask_char_values(PREF_ICS_CLASSIFIABLE_TYPES, &valCount , &prefValues));
	
	if ( valCount > 0)
	{
		for (int xx = 0; xx < valCount; xx++)
		{
			if (tc_strncasecmp(ObjType.c_str(), prefValues[xx] , ObjType.length()) == 0)
			{
				isValidForClassification = true;
			}
		}
	}

	return 0;
}
int setClassAttributes(map <std::string, std::string> objData, tag_t tclass, tag_t tClassificationObj, string& FailedAttributes)
{

	int nAttr = 0;
	int ifail = 0;
	int iStatus = ITK_ok;
	int FailedAttrCount = 0;

	tag_t tView = NULLTAG;

	scoped_smptr <int>
		iFlags,
		unctNumbers,
		unctFormats;

	scoped_smptr <char*>
		cAttrNames,
		cAttrUnits,
		cAttrValues;

	string clsAttributeName;
	string clsAttributeValue;
	FailedAttributes.clear();

	//ITK(ICS_describe_classification_object(tClassificationObj, &tView, &nAttr, &unctNumbers, &unctFormats, &cAttrNames, &cAttrValues, &cAttrUnits));

	char* classID = NULL;
	char* viewID = NULL;

	ITK(ICS_ask_cid_sid_of_classification_obj(tClassificationObj, &classID, &viewID));

	const char 	theClassId = NULL;
	int  	theCount = 0;
	int* theIds = 0;
	char** theNames = NULL;
	char** theShortNames = NULL;
	char** theAnnotations = NULL;
	int* theArraySize = 0;
	int* theFormat = 0;
	char** theUnit = NULL;
	char** theMinValues = NULL;
	char** theMaxValues = NULL;
	char** theDefaultValues = NULL;
	char** theDescriptions = NULL;
	int* theOptions = 0;

	ITK(ICS_class_describe_attributes(classID, &theCount, &theIds, &theNames, &theShortNames, &theAnnotations, &theArraySize, &theFormat, &theUnit, &theMinValues, &theMaxValues, &theDefaultValues, &theDescriptions, &theOptions
	));

	for (int ii = 0; ii < theCount; ii++)
	{
		clsAttributeValue.clear();

		string format;
		format.assign(std::to_string(theFormat[ii]));

		//cout << "the attribute at ii is:" << theNames[ii] << endl;
		auto att = objData.find(theNames[ii]);

		
		if (att == objData.end())
		{
			//cout << "element Could not found" << endl;
		}
		else
		{
			//cout << "Setting class attribute -> " << att->first << " with value -> " << att->second << endl;
			clsAttributeName.assign(att->first);

			//cout << "array size:" << theArraySize[ii] << endl;
			if (theArraySize[ii] == 0)
			{
				if (theFormat[ii] < 0)
				{

					int
						nSharedsites,
						options,
						nLovEntries;

					scoped_smptr <char>
						OwningSites,
						Keylovname;

					scoped_smptr <char*>
						lovKeys,
						sharedsites,
						LovValues;

					scoped_smptr<bool> Deprecated_status;



					ITK(ICS_keylov_get_keylov(format.c_str(), &Keylovname, &options, &nLovEntries, &lovKeys, &LovValues, &Deprecated_status, &OwningSites, &nSharedsites, &sharedsites));
					//std::cout << Keylovname.get() << std::endl;
					if (tc_strcmp(att->second.c_str(), "") != 0)
					{
						clsAttributeValue.clear();

						clsAttributeValue.assign(att->second);

						int nChars = att->second.length();
						bool keyFound = false;

						for (int zz = 0; zz < nLovEntries; zz++)
						{

							//if (tc_strncasecmp(LovValues[zz], att->second.c_str(), nChars) == 0)
							if (tc_strcasecmp(LovValues[zz], att->second.c_str()) == 0 || tc_strcasecmp(lovKeys[zz], att->second.c_str()) == 0)
							{
								//logfile << "\n LovValues:" << LovValues[zz];

								clsAttributeValue.assign(lovKeys[zz]);
								if (clsAttributeValue.find("#>") == 0)
								{
									clsAttributeValue.erase(0, 2);
								}
								/*else
								{
									cout << "key not found in lov" << lovKeys[zz] << endl;
								}*/

								ss << "\n LovValues:" << LovValues[zz] << " Length:" << att->second.length() << " Key:" << lovKeys[zz];
								logger.write(ss.str());
								ss.str("");

								keyFound = true;
								break;
							}
						}
						if (!keyFound)
						{
							ss << "ERROR: Key not found for  lov:" << Keylovname.get() << " and value: " << att->second << endl;
							logger.writefaillog(ss.str());
							ss.str("");
							ss.clear();

							//std::cout << "Key not found for value: " << att->second << std::endl;
						}
						if (tc_strcmp(clsAttributeValue.c_str(), "") == 0)
						{
							ss << "ERROR: Value -> " << att->second.c_str() << " for attribute -> " << clsAttributeName << " Does not matches the TC LOV values" << endl;
							logger.writefaillog(ss.str());
							ss.str("");
							FailedAttrCount++;
							FailedAttributes.append(",");
							FailedAttributes.append(clsAttributeName);
							clsAttributeValue.assign("");
						}
					}
				}
				else
				{
					clsAttributeValue.assign(att->second);
				}
			}

			else if (theArraySize[ii] > 0)
			{
				//std::cout << "Type is not keylov" << ifail << endl;

				// conside the attrubute value as is
				clsAttributeValue.assign(att->second);
			}
			else
			{
				std::cout << "array size not matching" << endl;
			}	
			const char* cAttrName;

			cAttrName = const_cast<const char*>(clsAttributeName.c_str());

			const char* cAttrValue;

			//logfile << "\n before Conversion:"<<clsAttributeValue;
			cAttrValue = const_cast<const char*>(clsAttributeValue.c_str());

			ITK(ICS_set_values_for_classification_obj(tClassificationObj, 1, &cAttrName, &cAttrValue));

			if (iStatus == ITK_ok)
			{
				ss << "Class Attribute update for attriibute -> " << cAttrName << " with Value -> " << cAttrValue << " is Completed Successfully" << endl;
				logger.write(ss.str());
				ss.str("");
			}

			else
			{
				ss << "ERROR: Class Attribute update for attriibute -> " << cAttrName << " with Value -> " << cAttrValue << " is Failed " << iStatus << endl;
				logger.writefaillog(ss.str());
				ss.str("");
				FailedAttrCount++;
				FailedAttributes.append(",");
				FailedAttributes.append(cAttrName);
			}
		}
		
		
	}

	return FailedAttrCount;

}
int updateClassAttributes(const char* clsAttributeName, const char* clsAttributeValue, int& FailedAttrCount, string & FailedAttributes, tag_t tClassificationObj)
{
	int iStatus = ITK_ok;
	const char* cAttrName;

	cAttrName = const_cast<const char*>(clsAttributeName);

	const char* cAttrValue;

	cAttrValue = const_cast<const char*>(clsAttributeValue);

	//std::cout << "cAttrValue::" << cAttrValue <<endl;

	iStatus = ICS_set_values_for_classification_obj(tClassificationObj, 1, &cAttrName, &cAttrValue);

	if (iStatus == ITK_ok)
	{
		ss << "Success|" << "Attribute update successfully| " << cAttrName << "|" << cAttrValue;
		logger.write(ss.str());
		ss.str("");
	}
	else
	{
		char* pcError = NULL;
		EMH_ask_error_text(iStatus, &pcError);
		ss << "Fail|" << "ERROR:Class Attribute update for attriibute is Failed " << cAttrName << "," << cAttrValue << pcError;
		logger.writefaillog(ss.str());
		ss.str("");
		FailedAttrCount++;
		SAFE_MEM_FREE(pcError);
		FailedAttributes.append(",");
		FailedAttributes.append(cAttrName);
	}

	return 0;

}

//int getObject(const char *item_id, const char *suffix, const char *ObjType, tag_t *tObj)
//{
//
//	//std::std::cout << "Start of find_obj_rev" << endl;
//	int nObjs = 0;
//	int iStatus = ITK_ok;
//	scoped_smptr <tag_t> tObjs;
//	char* cObjType;
//	
//	const char
//
//		*names[3] = { ITEM_ID , OBJECT_SUFFIX , OBJECT_TYPE },
//		*values[3] = { item_id , suffix , ObjType };
//
//
//	//ITK(ITEM_find_item_revs_by_key_attributes(2, names, values, item_rev_id, &n, &revs));
//
//	ITK(ITEM_find_items_by_key_attributes(3, names, values, &nObjs, &tObjs));
//
//	if (nObjs > 0)
//	{
//		*tObj = tObjs[0];
//	}
//
//	//std::std::cout << "End of find_obj_rev" << endl;
//
//	return 0;
//
//}

int getObject(const char *itemId, const char *pObjType, tag_t *tObj)
{

	//std::std::cout << "Start of find_obj_rev" << endl;
	int nObjs = 0;
	int iStatus = ITK_ok;
	tag_t * tObjs = NULL;
	char * cObjType = NULL;

	const char

		*names[2] = { ITEM_ID , OBJECT_TYPE },
		*values[2] = { itemId , pObjType };
		/**names[3] = { ITEM_ID , OBJECT_SUFFIX,OBJECT_TYPE },
		*values[3] = { itemId , suffix,pObjType };*/

	/*if (tc_strcmp(pObjType, M4M_PART_TYPE) == 0 || tc_strcmp(pObjType, M4M_STUDY_TYPE) == 0 || tc_strcmp(pObjType, M4M_CUSTOMER_TYPE) == 0 || tc_strcmp(pObjType, M4M_COMMERCIAL_TYPE) == 0 || tc_strcmp(pObjType, M4M_DRAWING_TYPE) == 0)
	{
		ITK(ITEM_find_items_by_key_attributes(2, names, values, &nObjs, &tObjs));
	}
	else
	{*/
		ITK(ITEM_find_items_by_key_attributes(2, names, values, &nObjs, &tObjs));
	//}

	//logfile << "nObjs -> " << nObjs << " found in Teamcenter" << endl;

	if (nObjs > 0)
	{
		for (int ss = 0; ss < nObjs; ss++)
		{
			//logfile << "Object -> " << itemId << " found in Teamcenter" << endl;

			ITK(AOM_ask_value_string(tObjs[ss], OBJECT_TYPE, &cObjType));

			//logfile << "Teamcenter Primary object type is ->  "<< cObjType << endl;

			if (tc_strcmp(pObjType, cObjType) == 0)
			{
				*tObj = tObjs[ss];

			}
		}
	}
	/*else
	{
		ss << "ERROR:Object -> " << itemId << " not found in Teamcenter" << endl;
		logger.writefaillog(ss.str());
		ss.str();
	}*/

	return 0;

}

std::vector<std::string_view> split(std::string_view buffer, const std::string_view delimiter) {
	std::vector<std::string_view> result;
	std::string_view::size_type pos;

	while ((pos = buffer.find(delimiter)) != std::string_view::npos) {
		auto match = buffer.substr(0, pos);
		/*if (!match.empty()) {
			result.push_back(match);
		}*/
		
			result.push_back(match);
		
		buffer.remove_prefix(pos + delimiter.size());
	}

	/*if (!buffer.empty()) {
		result.push_back(buffer);
	}*/
	
		result.push_back(buffer);
	

	return result;
}
/* map <int, std::map <std::string, std::string>> Read_input_file_1(char* fileName)
{
	ifstream MyFile;
	map <int, std::map <std::string, std::string>> ObjectsMap;

	MyFile.open(fileName, ios::in);
	if (!MyFile.is_open())
	{
		std::std::cout << "Failed to open the file" << endl;
		//sWrite << "Failed to open the file" << endl;
		//loggers.writefaillog(sWrite.str());
		//sWrite.str("");
	}
	else
	{
		std::std::cout << "File opened successfully" << endl;
		//sWrite << "File opened successfully" << endl;
		//loggers.write(sWrite.str());
		//sWrite.str("");

		int count = 0;

		string line;

		map< int, string> HeadersMap;

		vector <std::map<std::string, std::string>> Objects;

		while (getline(MyFile, line, '\n'))
		{
			count++;
			int AttrCount = 0;
			string linevec;

			vector <string> vec;
			std::map <std::string, std::string> PropMap;

			istringstream ss(line);
			//std::cout << "ss::" << ss.str() << endl;
			while (getline(ss, linevec, '~##'))
			{
				AttrCount++;

				if (count == 1)
				{
					HeadersMap.insert(std::pair< int, string>(AttrCount, linevec));
				}
				else
				{
					if (!linevec.empty())
					{
						auto it3 = HeadersMap.find(AttrCount);

						PropMap.insert(std::pair<string, string>(it3->second, linevec));
					}

				}
			}
			if (count > 1)
			{
				map<string, string>::iterator it;

				for (it = PropMap.begin(); it != PropMap.end(); it++)
				{
					if (it->first.find("@") != std::string::npos) {
						string s = it->first;
						//std::cout << "s::"<< it->first << endl;
						std::string delimiter = "@";
						size_t pos = 0;
						std::string token;
						while ((pos = s.find(delimiter)) != std::string::npos) {
							token = s.substr(0, pos);
							//std::std::cout << token << "::" << it->second << std::endl;
							PropMap.insert(std::pair<string, string>(token, it->second));
							s.erase(0, pos + delimiter.length());

						}
						if (!s.empty())
						{
							//std::cout << s <<"::" << it->second << endl;
							PropMap.insert(std::pair<string, string>(s, it->second));
						}

					}
				}

			}


			if (count > 1)
			{
				/*map<string, string>::iterator it;
				std::cout <<"---------------------------------------------------" << endl;
				for (it = PropMap.begin(); it != PropMap.end(); it++)
				{
					std::cout << it->first << "::" << it->second << endl;
				}
				std::cout << "---------------------------------------------------" << endl;
				ObjectsMap.insert(std::pair<int, std::map <std::string, std::string>>(count, PropMap));
			}

		}

		for (auto& t : ObjectsMap)
		{
			for (auto& s : t.second)
			{
			}
			//std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
		}
		MyFile.close();
	}
	return ObjectsMap;
}
*/

map <int, std::map <std::string, std::string>> Read_input_file_1(char* fileName)
{
	
	ifstream MyFile;
	map <int, std::map <std::string, std::string>> ObjectsMap;

	MyFile.open(fileName, ios::in);
	if (!MyFile.is_open())
	{
		std::cout << "Failed to open the Input file" << endl;		
		//logfile << "Failed to open the Input file" << endl;
	
	}
	else
	{
		std::cout << "Input File opened successfully" << endl;
		//logfile << "Input File opened successfully" << endl;		
		

		int count = 0;

		string line;

		map< int, string> HeadersMap;

		vector <std::map<std::string, std::string>> Objects;

		while (getline(MyFile, line, '\n'))
		{
			count++;
			int AttrCount = 0;
			string linevec;

			vector <string> vec;
			std::map <std::string, std::string> PropMap;
			vector <std::string> items;
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
//
//map <int, std::map <std::string, std::string>> Read_input_file(char *fileName)
//{
//	ifstream MyFile;
//	map <int, std::map <std::string, std::string>> ObjectsMap;
//
//	MyFile.open(fileName, ios::in);
//	if (!MyFile.is_open())
//	{
//		std::std::cout << "Failed to open the file" << endl;
//	}
//	else
//	{
//		std::std::cout << "File opened successfully" << endl;
//
//		int count = 0;
//
//		string line;
//
//		map< int, string> HeadersMap;
//
//		vector <std::map<std::string, std::string>> Objects;
//
//		while (getline(MyFile, line, '\n'))
//		{
//			//std::std::cout << "Line is --" << line << endl;
//
//			count++;
//			int AttrCount = 0;
//			string linevec;
//
//			vector <string> vec;
//			std::map <std::string, std::string> PropMap;
//
//			istringstream ss(line);
//
//			while (getline(ss, linevec, '$'))
//			{
//				AttrCount++;
//
//				//std::std::cout << "AttrCount --" << AttrCount << endl;
//
//				if (count == 1)
//				{
//					//std::std::cout << "Capture the Headers " << endl;
//
//					HeadersMap.insert(std::pair< int, string>(AttrCount, linevec));
//				}
//				else
//				{
//					auto it3 = HeadersMap.find(AttrCount);
//
//					//std::std::cout << "Attribute Value is " << linevec << endl;
//
//					//std::std::cout << "Attribute Header is " << it3->second << endl;
//
//					PropMap.insert(std::pair<string, string>(it3->second, linevec));
//
//				}
//			}
//
//			if (count > 1)
//			{
//				ObjectsMap.insert(std::pair<int, std::map <std::string, std::string>>(count, PropMap));
//			}
//
//		}
//
//		for (auto& t : ObjectsMap)
//		{
//			//std::cout << "Final prop map" << t.first << endl;
//			//std::cout << "Final prop map" << t.second << endl;
//
//			//std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
//
//			for (auto& s : t.second)
//			{
//				//std::cout << "Final prop map --" << s.first << endl;
//				//std::cout << "Final prop map --" << s.second << endl;
//
//			}
//			//std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
//		}
//
//		///std::std::cout << "Number of lines" << endl << count << endl;
//	}
//	return ObjectsMap;
//}

tag_t getItemOrRevToValidate(tag_t tObj, string ObjRevId)
{
	scoped_smptr<char> rev_id;
	scoped_smptr<char> item_id;
	int iStatus = ITK_ok;
	tag_t obj_rev = NULLTAG;

	ITK(ITEM_ask_id2(tObj, &item_id));
	//std::cout << "ObjRevId.c_str()::" << ObjRevId.c_str() << endl;
	if (ObjRevId.empty())
	{
		//ITK(ITEM_ask_latest_rev(tObj, &obj_rev));
		std::cout << "Processing object ::" << item_id.getString() << endl;
		return tObj;
	}
	else if (tc_strcasecmp(ObjRevId.c_str(), "none") == 0)
	{
		std::cout << "Processing object ::" << item_id.getString()<< endl;
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
	iStatus=ITEM_ask_rev_id2(obj_rev, &rev_id);
	if (iStatus != ITK_ok)
	{
		ss << "ERROR: Revision is not available in Teamcenter,skipping Object classification " << endl;
		logger.writefaillog(ss.str());
		ss.str("");
		//std::cout << "Revision not available in Teamcenter"<< endl;		
	}
	else
	{
		std::cout << "Processing object ::" << item_id.getString() << "\\" << rev_id.get() << endl;
	}
	
	
	/*sWrite << "INFO: Object latest revision is " << rev_id.getString() << endl;
	loggers.write(sWrite.str());
	sWrite.str("");*/

	return obj_rev;
}
