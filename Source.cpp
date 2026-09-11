
#include "Header.hxx"

int ITK_user_main(int argc, char** argv)
{
	int iUnUsed = 0;
	ofstream fpOutFile;
	int iStatus = ITK_ok;
	vector <classifiedObjs_t> vecClassifiedObjs;
	//ResultCheck retCode;
	/*fstream fp;
	fp.open(Logfile);*/
	if (strncmp(argv[1], "-h", 2) == 0)
	{
		displayUsage();
		exit(1);
	}

	//map<string icoAttrvalues_t> mapClassifiedObjects;
	char* cpUserId = ITK_ask_cli_argument("-u=");
	char* cpPwd = ITK_ask_cli_argument("-p=");
	char* cpGrp = ITK_ask_cli_argument("-g=");
	char* cpInputFile = ITK_ask_cli_argument("-input=");
	char* cpOutputFile = ITK_ask_cli_argument("-output=");
	char* Logfile = ITK_ask_cli_argument("-log=");
	time_t start = std::time(nullptr);

	if (cpUserId == NULL || cpPwd == NULL || cpGrp == NULL || cpInputFile == NULL || Logfile == NULL)
	{
		displayUsage();
		iStatus = !ITK_ok;
		return iStatus;
	}

	ITK(ITK_initialize_text_services(iUnUsed));
	ITK(ITK_init_module(cpUserId, cpPwd, cpGrp));

	logger = M_Logger(Logfile);

	if (iStatus == ITK_ok)
		logger.write("Login Successful\n");

	vecClassifiedObjs = readInputFile(cpInputFile);

	map<tag_t, map<string, icoAttrValues_t>> mapClassifiedObjects;
	mapClassifiedObjects = getAllClassificationAttributeValues(vecClassifiedObjs);

	if(mapClassifiedObjects.size() >0)
		writeIntoFile(mapClassifiedObjects, cpOutputFile);

	return 0;
}

map<tag_t, map<string, icoAttrValues_t>> getAllClassificationAttributeValues(static vector <classifiedObjs_t> vecClassifiedObjs)
{
	
	int iStatus = ITK_ok;
	map<tag_t, map<string, icoAttrValues_t>> mapClassifiedObjects;
	tag_t classificationObject = NULLTAG;

	TC_write_syslog("ENTERED INTO %s\n", __FUNCTION__);
	std::cout << "ENTERED INTO " << __FUNCTION__ << "\n";


	//logger.write("\nProcessing total input objects " + vecClassifiedObjs.size());

	for (auto icoObject : vecClassifiedObjs)
	{
		logical isClassified = false;

		logger.write("_____________________________________________________________________");
		logger.write("Processing Item " + icoObject.item_id);
		try
		{
			ITK(ICS_is_wsobject_classified(icoObject.item_tag, &isClassified));
			icoObject.isItemClassified = isClassified;


			if (icoObject.rev_tag != NULLTAG)
			{
				isClassified = false;
				ITK(ICS_is_wsobject_classified(icoObject.rev_tag, &isClassified));
				icoObject.isItemRevClassified = isClassified;
			}

			if (icoObject.isItemClassified)
			{
				logger.write("Item is classified \t " + icoObject.item_id);
				getClassificationAttributeValues(icoObject.item_tag, mapClassifiedObjects, icoObject.item_id);
			}

			if (icoObject.isItemRevClassified)
			{
				logger.write("Item Revision is classified \t " + icoObject.item_id + "/" + icoObject.item_rev_id);
				getClassificationAttributeValues(icoObject.rev_tag, mapClassifiedObjects, icoObject.item_id, icoObject.item_rev_id);
			}

			if (icoObject.isItemClassified == false && icoObject.isItemRevClassified == false)
				logger.writebothlog("Niether Item Nor Revision is classified for " + icoObject.item_id);
		}
		catch (int iStatus)
		{
			char* error_str = NULL;																								
			EMH_ask_error_text(iStatus, &error_str);																			
			TC_write_syslog("ERROR: %d, ERROR MSG: %s. at Line: %d in File: %s\n", iStatus, error_str, __LINE__, __FILE__);	
			logger.writebothlog("ERROR: " + to_string(iStatus) + ", ERROR MSG:" + error_str + ". at Line: " + to_string(__LINE__) + " in File: " + __FILE__);
			MEM_free(error_str);
		}
		
	}

	TC_write_syslog("EXIT FROM %s\n", __FUNCTION__);
	std::cout << "EXIT FROM " << __FUNCTION__ << "\n";

	return mapClassifiedObjects;
}

void getClassificationAttributeValues(tag_t objectTag, map<tag_t, map<string, icoAttrValues_t>>& mapClassifiedObjects, string ItemId, string RevId)
{
	/*fstream fp;
	fp.open(Logfile);*/
	int iStatus = ITK_ok;
	tag_t classificationObject = NULLTAG;
	TC_write_syslog("ENTERED INTO %s\n", __FUNCTION__);
	//std::cout << "ENTERED INTO " << __FUNCTION__ << "\n";

	map<string, icoAttrValues_t> icoAttrValues;
	//std::cout << "_____________________________________________________________________\n";
	//std::cout << "Collecting classification attributes for " << ItemId << endl;

	logger.write("Collecting classification attributes for " + ItemId);

	ITK(ICS_ask_classification_object(objectTag, &classificationObject));
	if (classificationObject != NULLTAG)
	{
		int attrCount = 0, * unctNumbers = NULL, * unctFormats = NULL;
		int* attrIds = NULL;
		int* attrValueCount = NULL;
		char*** attrValues = NULL;
		char** attrNames = NULL;
		tag_t
			view_tag = NULLTAG;
		char*** units = NULL;
		char** display_values = NULL;
		char** prop_name = NULL;
		int num = 0;
		char** Values = NULL;

		ITK(AOM_ask_value_strings(objectTag, "fnd0IcsClassNames", &num, &Values));

		string  tempString;
		if (RevId.empty())
			tempString.assign(ItemId);
		else
			tempString.assign(ItemId).append("/").append(RevId);

		if (num > 1)
			logger.writebothlog("ERROR :: Have multiple classified objects on item " + tempString);

		ITK(ICS_ico_ask_attributes_optimized(classificationObject, &attrCount, &attrIds, &attrNames, &attrValueCount, &attrValues, &units));

		logger.write("TOTAL ATTRIBUTE COUNT IS " + to_string(attrCount));
		for (int inx = 0; inx < attrCount; inx++)
		{
			icoAttrValues_t icoValues;
			icoValues.theAttributeValCounts = attrCount;
			setAttributeNames.insert(attrNames[inx]);
			if (attrValueCount[inx] > 0)
			{
				icoValues.theAttributeValues = attrValues[inx][0];
				setAttributevalues.insert(attrValues[inx][0]);
				icoAttrValues.insert(pair<string, icoAttrValues_t>(string(attrNames[inx]), icoValues));
			}
			else
			{
				icoValues.theAttributeValues = NULL;
				icoAttrValues.insert(pair<string, icoAttrValues_t>(string(attrNames[inx]), icoValues));
			}
			
			string tempAttrValue;
			if (icoValues.theAttributeValues == NULL)
				tempAttrValue = "";
			else
				tempAttrValue.assign(icoValues.theAttributeValues);
			logger.write("CLASSIFCATION : " + (string)attrNames[inx] + " \t " + tempAttrValue);
		}

		if (mapClassifiedObjects.find(objectTag) == mapClassifiedObjects.cend())
			mapClassifiedObjects.insert(pair<tag_t, map<string, icoAttrValues_t>>(objectTag, icoAttrValues));


		SAFE_SM_FREE(attrValues);
		SAFE_SM_FREE(attrNames);
		SAFE_SM_FREE(attrIds);
		SAFE_SM_FREE(attrValueCount);
		SAFE_SM_FREE(units);
		SAFE_MEM_FREE(Values);

	}
	else
	{

	}
	TC_write_syslog("EXIT FROM %s\n", __FUNCTION__);
	//cout << "EXIT FROM " << __FUNCTION__ << "\n";

}
void displayUsage(void)
{

	cout << "\n +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

	cout << "\n Usage : " << endl;

	cout << "\n ClassificationExtract utility is used for Extracting classification attributes the objects that are mentioned in input file " << endl;

	cout << "\n -----------------------------------------------------------------------------------------------------------" << endl;

	cout << "xxxxxx" << "  -u=<userid> -p=<passwd> -g=<group> -input=<input file path> -output=<out file path> -log=<log Directory> " << endl << endl;

	cout << "[-h] Displays this usage information" << endl << endl;

	cout << "Input file Header" << endl << endl;

	cout << "item_id[|rev_id]" << endl;

	cout << "\n +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
}

vector <classifiedObjs_t> readInputFile(char* cpInputFile)
{
	ifstream fpInputFile;
	int iStatus = ITK_ok;
	vector <classifiedObjs_t> vecIcoObjects;
	char delimit = '|';
	fpInputFile.open(cpInputFile);

	if (fpInputFile.is_open())
	{
		string line;

		logger.write("Input file opened successfully " + (string)cpInputFile);

		getline(fpInputFile, line);
		vector<string> vectLines;
		while (getline(fpInputFile, line))
		{
			tag_t tRevTag = NULLTAG;
			tag_t tItemTag = NULLTAG;
			
			vectLines.clear();

			stringstream ss(line);
			string token;
			while (getline(ss, token, delimit))
				vectLines.push_back(token);

			if (!vectLines[0].empty())
			{
				find_item((char*)vectLines[0].c_str(), &tItemTag);
			}
			if (vectLines.size() == 2)
			{
				find_rev((char*)vectLines[0].c_str(), (char*)vectLines[1].c_str(), &tRevTag);
			}
			
			classifiedObjs_t icoObjects;

			if (tItemTag != NULLTAG)
			{
				logger.write("\nItem found " + vectLines[0]);
				icoObjects.item_id = vectLines[0];
				icoObjects.item_tag = tItemTag;
			}
			else
			{
				logger.writebothlog("\nItem not found " + vectLines[0]);
				cout << "Item not found " << vectLines[0] << endl;
				continue;
			}

			if (tRevTag != NULLTAG)
			{
				icoObjects.item_rev_id = vectLines[1];
				icoObjects.rev_tag = tRevTag;
			}

			if(tItemTag != NULLTAG)
				vecIcoObjects.push_back(icoObjects);

		}
	}
	else
	{
		logger.writebothlog("Failed to open Input file " + (string)cpInputFile);
		std::cout << "Coulnt able to open input file " << cpInputFile << endl;
	}

	return vecIcoObjects;
}


void writeIntoFile(map<tag_t, map<string, icoAttrValues_t>> mapClassifiedObjects, char* cpOutputFile)
{
	int iStatus = ITK_ok;
	ofstream fpInputFile;
	fpInputFile.open(cpOutputFile);

	TC_write_syslog("ENTERED INTO %s\n", __FUNCTION__);
	std::cout << "ENTERED INTO " << __FUNCTION__ << "\n";

	logger.write("\n\nWriting into output file " + (string)cpOutputFile);
	if (fpInputFile.is_open())
	{
		//write header
		fpInputFile << "item_id|item_revision_id|object_type|CLASS_ID|CLASS_NAME";
		for (string attrName : setAttributeNames)
		{
			fpInputFile << "|" << attrName;
		}

		fpInputFile << "\n";
		for (auto classifiedObject : mapClassifiedObjects)
		{
			try
			{
				tag_t objectTag = classifiedObject.first;

				if (isObjectSubType(objectTag, "Item"))
				{

					char* object_id = NULL;
					char* item_id;
					ITK(ITEM_ask_id2(objectTag, &item_id));
					ITK(WSOM_ask_object_type2(objectTag, &object_id));
					fpInputFile << item_id << "|" << "|" << object_id;
					logger.write("\Writing Item " + (string)item_id);
				}

				else if (isObjectSubType(objectTag, "ItemRevision"))
				{

					tag_t item_tag = NULLTAG;
					char* item_id = NULL;
					char* rev_id = NULL;
					char* Object_Type = NULL;

					ITK(ITEM_ask_item_of_rev(objectTag, &item_tag));
					ITK(WSOM_ask_object_type2(item_tag, &Object_Type));
					ITK(ITEM_ask_id2(item_tag, &item_id));
					ITK(ITEM_ask_rev_id2(objectTag, &rev_id));
					//cout << Object_Type << endl;
					fpInputFile << item_id << "|" << rev_id << "|" << Object_Type;
					logger.write("\Writing Item " + (string)item_id + "/" + rev_id);
				}

				tag_t classificationObject = NULLTAG;
				char* classId = NULL;
				char* className = NULL;
				tag_t classObject = NULLTAG;

				ITK(ICS_ask_classification_object(objectTag, &classificationObject));
				if (classificationObject != NULLTAG)
					ITK(ICS_ask_class_of_classification_obj(classificationObject, &classObject));
				if (classObject != NULLTAG)
					ITK(ICS_ask_id_name(classObject, &classId, &className));

				fpInputFile << "|" << classId << "|" << className;

				map<string, icoAttrValues_t> mapClassifiedObject;

				mapClassifiedObject = classifiedObject.second;
				for (string attrName : setAttributeNames)
				{
					if (mapClassifiedObject.find(attrName) != mapClassifiedObject.cend())
					{
						icoAttrValues_t icoValues = mapClassifiedObject.find(attrName)->second;
						if(icoValues.theAttributeValues == NULL)
							fpInputFile << "|" << "";
						else
							fpInputFile << "|" << icoValues.theAttributeValues;
					}
					else
					{
						fpInputFile << "|";
					}
				}
				fpInputFile << "\n";
			}
			catch (...)
			{
				logger.write("ERROR::Unable to process Attribute ");
				continue;
			}

			
		}
	}
	else
	{
		logger.writebothlog("Failed to open Input file " + (string)cpOutputFile);
		cout << "Could not be able to open output file " << cpOutputFile << endl;

	}

	TC_write_syslog("Exited from %s\n", __FUNCTION__);
	cout << "EXITED from " << __FUNCTION__ << "\n";

}

logical isObjectSubType(tag_t object, string parentType)
{
	int iStatus = ITK_ok;

	logical isSubtype = false;
	tag_t parentTypeTag = NULLTAG;
	tag_t objectTypeTag = NULLTAG;

	ITK(TCTYPE_ask_object_type(object, &objectTypeTag));
	ITK(TCTYPE_find_type(parentType.c_str(), NULL, &parentTypeTag));
	ITK(TCTYPE_is_type_of(objectTypeTag, parentTypeTag, &isSubtype));

	return isSubtype;
}

static void find_item(char* item_id, tag_t* item)
{
	int n = 0;
	int iStatus = ITK_ok;

	tag_t
		* items;
	const char
		* names[1] = { "item_id" },
		* values[1] = { item_id };

	ITK(ITEM_find_items_by_key_attributes(1, names, values, &n,	&items));
	if (n > 0) *item = items[0];
	if (items) MEM_free(items);
}

static int find_rev(char* item_id, char* rev_id, tag_t* rev)
{
	int
		n = 0,
		iStatus = ITK_ok;
	tag_t
		* items;
	const char
		* names[1] = { "item_id" },
		* values[1] = { item_id };

	ITK(ITEM_find_item_revs_by_key_attributes(1, names, values, rev_id,
		&n, &items));
	if (n > 0) *rev = items[0];
	if (items) MEM_free(items);

	return 0;
}