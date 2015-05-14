#ifndef LOG_DATA
#define LOG_DATA

#include <cstdio>
#include <sstream>
#include "utils.h"
#include <regex>

using namespace std;

class LogData
{
public:
	LogData();
	~LogData();
	void handleLog(char *l_fileName, DATAINFO l_infoDataset1, DATAINFO l_infoDataset2, BM** l_similarityResults, LISTPARAM l_simList, PLANEEQ l_inputPlane, PLANEEQ l_fittedPlane, float l_angleError, float l_distanceError, double l_runtime, FP l_functionalParameters, vector<Point3> l_pTofit);
	bool createFile();
	bool saveDataIntoFile();
	bool closeFile();
private:
	char *fileName;
	DATAINFO infoDataset1;
	DATAINFO infoDataset2;	
	FILE *logFile;
	BM   **simResults;
	LISTPARAM simList;
	PLANEEQ inputPlane;
	PLANEEQ fittedPlane;
	FP functionalParameters;
	float angleError;
	float distanceError;
	double runtime;
	vector<Point3> pointsToFit;
};


#endif //LOG_DATA