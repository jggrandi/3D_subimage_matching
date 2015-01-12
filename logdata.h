#ifndef LOG_DATA
#define LOG_DATA

#include <cstdio>
#include "utils.h"

class LogData
{
public:
	LogData();
	~LogData();
	// void handleLog(char *l_fileName, vector<sliceRank> l_similarityResults, double l_runtime);
	// bool createFile(char *l_fileName);
	// bool saveDataIntoFile(char *l_fileName, vector<sliceRank> l_similarityResults, double l_runtime);
	// bool closeFile(char *l_fileName);
private:
	char *fileName;
	FILE *logFile;
};


#endif //LOG_DATA