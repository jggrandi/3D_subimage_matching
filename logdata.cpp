#include "logdata.h"

LogData::LogData(){}

LogData::~LogData(){}

void LogData::handleLog(char *l_fileName, DATAINFO l_infoDataset1, DATAINFO l_infoDataset2, BM** l_similarityResults, LISTPARAM l_simList, PLANEEQ l_inputPlane, PLANEEQ l_fittedPlane, float l_angleError, float l_distanceError, double l_runtime, FP l_functionalParameters)
{
	fileName = l_fileName;
	simResults = l_similarityResults;
	simList = l_simList;
	inputPlane = l_inputPlane;
	fittedPlane = l_fittedPlane;
	angleError = l_angleError;
	distanceError = l_distanceError;
	runtime = l_runtime;
	infoDataset1 = l_infoDataset1;
	infoDataset2 = l_infoDataset2;
	functionalParameters = l_functionalParameters;

	if(!createFile()) printf("File not created.\n");
	if(!saveDataIntoFile()) printf("File not salved\n"); else printf("Log file saved as %s.\n",fileName);
	if(!closeFile()) printf("File not closed.\n");
}

bool LogData::createFile()
{

	printf("%d,%d\n",functionalParameters.KERNEL, functionalParameters.SAMPLING );
	stringstream output;
	output << infoDataset1.fileName << "X" <<infoDataset2.fileName << "_"<<functionalParameters.KERNEL << "_"<<functionalParameters.SAMPLING << ".csv";
	string filename = output.str();


    std::regex prefix("../../../../Hepatec/Datasets/",std::regex_constants::ECMAScript | std::regex_constants::icase);
    std::regex sulfix(".raw",std::regex_constants::ECMAScript | std::regex_constants::icase);
    std::regex otherfix("/",std::regex_constants::ECMAScript | std::regex_constants::icase);
	
	std::string new_s = std::regex_replace(filename, prefix, "1");
	new_s = std::regex_replace(new_s, sulfix, "_");
	new_s = std::regex_replace(new_s, otherfix, "_");

	stringstream completeFileName;
	completeFileName << "log/" <<new_s;
	string name = completeFileName.str();
	const char* ss = name.c_str();
	printf("%s\n",ss );


	if(!(logFile = fopen(ss, "w")))
		return false;
	return true;
}

bool LogData::saveDataIntoFile()
{

	fprintf(logFile, "RankPos;Z;X;Y;Plane;SimValue\n");
	for(unsigned int i = 0; i < simList.listSize; i++)
	{
		fprintf(logFile, "%d\n",i);			
		for(unsigned int j = 0; j < simList.listRankSize; j++)
		{
			fprintf(logFile, "%d;%d;%d;%d;%d;%f\n", j, simResults[i][j].bmCoord.z, simResults[i][j].bmCoord.x,simResults[i][j].bmCoord.y, simResults[i][j].bmPlane, simResults[i][j].bmSimValue);	
		}
	}
	fprintf(logFile, "Input  Plane Eq; %f;%f;%f;%f\n",inputPlane.vector.x, inputPlane.vector.y, inputPlane.vector.z, inputPlane.d);
	fprintf(logFile, "Fitted Plane Eq; %f;%f;%f;%f\n",fittedPlane.vector.x, fittedPlane.vector.y, fittedPlane.vector.z, fittedPlane.d);
	fprintf(logFile, "Angle Error; %f\n",angleError);
	fprintf(logFile, "Distance Error; %f\n",distanceError);
	fprintf(logFile, "Runtime; %f\n",runtime );
	return true;
}

bool LogData::closeFile()
{
	fclose(logFile);
	return true;
}
