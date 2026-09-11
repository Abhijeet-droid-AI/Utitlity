#include "M_Logger.hpp"

using namespace std;

/**
* @brief   BW : Constructor.
* @return  void.
*
* @par Note
*
*/
//M_Logger* M_Logger::logger = NULL;

DLLAPI M_Logger::M_Logger(void)
{

}

/******************************************************************************************************************************************
Constructor    : M_Logger(string inLogDirectory )
Description : Creates a log files at inputed location and open it for writing

******************************************************************************************************************************************/
DLLAPI M_Logger::M_Logger(string inLogDirectory)
{
	logDirectory_.assign(inLogDirectory);

	string s = "SuccessEPM_";
	string fs = "FailEPM_";

	__time64_t rawtime;
	struct tm timeinfo;
	char buffer[200];
	/*
		time(&);
		localtime_s(timeinfo, &rawtime);*/
	_time64(&rawtime);
	// Convert to local time.
	_localtime64_s(&timeinfo, &rawtime);

	strftime(buffer, 200, "%Y_%m_%d__%H_%M_%S", &timeinfo);

	//cout << buffer << endl;
	s += buffer;
	fs += buffer;

	logFileName_.assign(s);
	FaillogFileName_.assign(fs);

	(logFile_) = new ofstream();
	(FaillogFile_) = new ofstream();

	string file = logDirectory_ + s + ".log";
	string ffile = logDirectory_ + fs + ".log";

	(*logFile_).open(file, ios::app);
	(*FaillogFile_).open(ffile, ios::app);
}

M_Logger logger;

/******************************************************************************************************************************************
Constructor    : write(string inInputLine)
Description : Writes to correspoding log file

******************************************************************************************************************************************/
DLLAPI void M_Logger::write(string inInputLine)
{
	*logFile_ << inInputLine << endl;
	//TC_write_syslog("\n%s\n", inInputLine.c_str());
}

/******************************************************************************************************************************************
Constructor    : writefaillog(string inInputLine)
Description : Writes to fail log file

******************************************************************************************************************************************/
DLLAPI void M_Logger::writefaillog(string inInputLine)
{
	error_flag = 1;
	*FaillogFile_ << inInputLine << endl;
	//TC_write_syslog("\n%s\n", inInputLine.c_str());
	
	//cout << inInputLine << endl;
}

/******************************************************************************************************************************************
Constructor    : writebothlog(string inInputLine)
Description : Writes to both fail and success log files

******************************************************************************************************************************************/
DLLAPI void M_Logger::writebothlog(string inInputLine)
{
	*logFile_ << inInputLine << endl;
	*FaillogFile_ << inInputLine << endl;
}

DLLAPI void M_Logger::close()
{
	logFile_->close();
	FaillogFile_->close();
}

DLLAPI void M_Logger::writeQuery(string query)
{
	*logFile_ << "88888888888888888888888888888888888888888888888888888888888888888888888\n";
	*logFile_ << query << endl;
	*logFile_ << "88888888888888888888888888888888888888888888888888888888888888888888888\n";
}