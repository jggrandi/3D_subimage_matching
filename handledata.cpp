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
		q.checkSimilarity(dataset1,dataset2, oopt);
		timeCounter = ((double)getTickCount() - timeCounter)/getTickFrequency();
		simCheckRuntime = timeCounter;
		
		similarityResults = q.getBestMatches();


		//logData.handleLog(oopt.logfilename, similarityResults,timeCounter);
		
	}
	else
	{
		q.checkSimilarity(dataset1,dataset2,oopt);
		similarityResults = q.getBestMatches();
	}
}

float HandleData::getRunningTime()
{
	return simCheckRuntime;
}

void HandleData::planeFitting()
{

}

void HandleData::volumetricSimilarityValues()
{

        // imgT **simVolume = (imgT**)calloc(infoImg.resDepth,infoImg.resDepth * sizeof(imgT*));
        // for (int i=0; i < infoImg.resDepth; i++)
        //     simVolume[i] = (imgT*)calloc(infoImg.resWidth*infoImg.resHeight, sizeof(imgT));

        // for (int i = 0; i < infoImg.resWidth * infoImg.resHeight; i++)
        // {
        //     if(bM[i].bmPlane == 1)
        //     //printf("%d,%d,%d\n", bM[i].bmCoord.z,bM[i].bmCoord.x,bM[i].bmCoord.y);
        //         simVolume[bM[i].bmCoord.z][ijn(bM[i].bmCoord.x,bM[i].bmCoord.y,infoImg.resWidth)] = 65000;
        // }

}



