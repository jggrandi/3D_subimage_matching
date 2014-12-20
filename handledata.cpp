#include "handledata.h"

HandleData::HandleData(){}

HandleData::~HandleData(){}

int HandleData::loadData(DATAINFO h_img1, DATAINFO h_img2, OPT h_options)
{
	oopt = h_options;

	dataset1 = Handle3DDataset<imgT>(h_img1);
	dataset2 = Handle3DDataset<imgT>(h_img2);

	if(!dataset1.loadFile()) { printf("Failed to load %s\n",h_img1.fileName); return 0;}
	else printf("%s loaded!\n",h_img1.fileName );

	if(!dataset2.loadFile()) { printf("Failed to load %s\n",h_img2.fileName); return 0;}
	else printf("%s loaded!\n",h_img2.fileName );
	
	return 1;
}

void HandleData::similarityCheck()
{
	if(oopt.logdata || oopt.verbose)
	{
		double timeCounter= (double)getTickCount();
		similarityResults = q.checkSimilarity(dataset1,dataset2, oopt);
		timeCounter = ((double)getTickCount() - timeCounter)/getTickFrequency();
		logData.handleLog(oopt.logfilename, similarityResults,timeCounter);
		printf("Runtime: %f\n",timeCounter );
	}
	else
		similarityResults = q.checkSimilarity(dataset1,dataset2,oopt);
}



