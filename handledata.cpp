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
		simAssess.checkSimilarity(dataset1,dataset2, oopt);
		timeCounter = ((double)getTickCount() - timeCounter)/getTickFrequency();
		simCheckRuntime = timeCounter;
		
		similarityResults = simAssess.getBestMatches();
		fittingInfo = simAssess.getFittingInfo();

		//logData.handleLog(oopt.logfilename, similarityResults,timeCounter);		
	}
	else
	{
		simAssess.checkSimilarity(dataset1,dataset2,oopt);
		similarityResults = simAssess.getBestMatches();
		fittingInfo = simAssess.getFittingInfo();
	}
}

float HandleData::getRunningTime()
{
	return simCheckRuntime;
}

void HandleData::planeFitting()
{
	planeFit.loadPoints(similarityResults,fittingInfo);
	planeFit.planeFittingCalculations();
	Plane p = planeFit.getPlaneFitted();
	cout << p.a() <<" "<< p.b() <<" "<< p.c() <<" "<< p.d() <<" "<< endl;
}

void HandleData::volumetricSimilarityValues()
{



}



