#include "handledata.h"

HandleData::HandleData(){}

HandleData::~HandleData(){}

int HandleData::loadData(DATAINFO h_data1, DATAINFO h_data2, OPT h_options)
{
	oopt = h_options;

	dataset1 = Handle3DDataset<imgT>(h_data1);
	dataset2 = Handle3DDataset<imgT>(h_data2);

	infoDataset1 = h_data1;
	infoDataset2 = h_data2;

	if(!dataset1.loadFile()) { printf("Failed to load %s\n",h_data1.fileName); return 0;}
	else printf("%s loaded!\n",h_data1.fileName );

	if(!dataset2.loadFile()) { printf("Failed to load %s\n",h_data2.fileName); return 0;}
	else printf("%s loaded!\n",h_data2.fileName );
	
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
		simList = simAssess.getBestMatchesListSize();
		fittingInfo = simAssess.getFittingInfo();
		inputPlane = simAssess.getPlaneEquation();
		functionalParameters = simAssess.getFunctionalParameters();

		planeFitting();
		distanceBetweenPlanes();
		angleBetweenPlanes();
		cout << endl;
		printInputPlane();
		printFittedPlane();
		printAngleBetweenPlanes();
		printDistanceBetweenPlanes();		
		//volumetricSimilarityValues();

		vector<Point3> pToFit = planeFit.getPointsToFit();
		logData.handleLog(oopt.logfilename, infoDataset1, infoDataset2, similarityResults, simList, inputPlane, fittedPlane, angleError, distanceError, timeCounter, functionalParameters, pToFit);		
	}
	else
	{
		simAssess.checkSimilarity(dataset1,dataset2,oopt);
		similarityResults = simAssess.getBestMatches();
		simList = simAssess.getBestMatchesListSize();
		fittingInfo = simAssess.getFittingInfo();
		inputPlane = simAssess.getPlaneEquation();
		planeFitting();
		distanceBetweenPlanes();
		angleBetweenPlanes();
		cout << endl;
		printInputPlane();
		printFittedPlane();
		printAngleBetweenPlanes();
		printDistanceBetweenPlanes();
	}
}

float HandleData::getRunningTime()
{
	return simCheckRuntime;
}

void HandleData::planeFitting()
{
	planeFit.loadPoints(similarityResults,fittingInfo,simList);
	planeFit.planeFittingCalculations();
	Plane p = planeFit.getPlaneFitted();
	vector3f tempvec(p.a(),p.b(),p.c());
	fittedPlane.vector = tempvec;
	fittedPlane.d = p.d();
}

void HandleData::printInputPlane()
{
	cout << "Input Plane: " << inputPlane.vector.x << " "<< inputPlane.vector.y << " " << inputPlane.vector.z <<" "<< inputPlane.d << endl;
}

void HandleData::printFittedPlane()
{
	cout << "Fitted Plane: " << fittedPlane.vector.x << " "<< fittedPlane.vector.y << " " << fittedPlane.vector.z <<" "<< fittedPlane.d << endl;
}

void HandleData::printAngleBetweenPlanes()
{
	cout << "Angle Between Planes: " << angleError << endl;
}

void HandleData::printDistanceBetweenPlanes()
{
	cout << "Distance Between Planes: " << distanceError << endl;
}


void HandleData::angleBetweenPlanes()
{
	float dot = dotProduct(inputPlane.vector,fittedPlane.vector);
	dot = dot / (fittedPlane.vector.length() *  inputPlane.vector.length());
	angleError = acos(dot)*180/3.14159265359;
}

void HandleData::distanceBetweenPlanes()
{

	float p0 = -(fittedPlane.vector.z*1.0f  + fittedPlane.vector.y*1.0f + fittedPlane.d)/(fittedPlane.vector.x);
	vector3f pl0v0(1,1,p0);
	float p1 = -(fittedPlane.vector.z*1.0f  + fittedPlane.vector.y*-1.0f + fittedPlane.d)/(fittedPlane.vector.x);
	vector3f pl0v1(1,1,p1);
	float p2 = -(fittedPlane.vector.z*-1.0f + fittedPlane.vector.y*-1.0f + fittedPlane.d)/(fittedPlane.vector.x);
	vector3f pl0v2(1,1,p2);
	float p3 = -(fittedPlane.vector.z*-1.0f + fittedPlane.vector.y*1.0f + fittedPlane.d)/(fittedPlane.vector.x);
	vector3f pl0v3(1,1,p3);


	p0 = -(inputPlane.vector.z*1.0f  + inputPlane.vector.y*1.0f + inputPlane.d )/(inputPlane.vector.x);
	vector3f pl1v0(1,1,p0);
	p1 = -(inputPlane.vector.z*1.0f  + inputPlane.vector.y*-1.0f + inputPlane.d)/(inputPlane.vector.x);
	vector3f pl1v1(1,1,p1);
	p2 = -(inputPlane.vector.z*-1.0f + inputPlane.vector.y*-1.0f + inputPlane.d)/(inputPlane.vector.x);
	vector3f pl1v2(1,1,p2);
	p3 = -(inputPlane.vector.z*-1.0f + inputPlane.vector.y*1.0f + inputPlane.d)/(inputPlane.vector.x);
	vector3f pl1v3(1,1,p3);


	float v0_d =  sqrt(pow((pl0v0.x - pl1v0.x),2) + pow((pl0v0.y - pl1v0.y),2) + pow((pl0v0.z - pl1v0.z),2));
	float v1_d =  sqrt(pow((pl0v1.x - pl1v1.x),2) + pow((pl0v1.y - pl1v1.y),2) + pow((pl0v1.z - pl1v1.z),2));
	float v2_d =  sqrt(pow((pl0v2.x - pl1v2.x),2) + pow((pl0v2.y - pl1v2.y),2) + pow((pl0v2.z - pl1v2.z),2));		
	float v3_d =  sqrt(pow((pl0v3.x - pl1v3.x),2) + pow((pl0v3.y - pl1v3.y),2) + pow((pl0v3.z - pl1v3.z),2));		

	v0_d = abs(v0_d);
	v1_d = abs(v1_d);
	v2_d = abs(v2_d);
	v3_d = abs(v3_d);

	distanceError = ( (pow(v0_d,2)*100) + (pow(v1_d,2)*100) + (pow(v2_d,2)*100) + (pow(v3_d,2)*100) )/ 4;
}

void HandleData::volumetricSimilarityValues()
{
	imgT **simVolume = new imgT*[fittingInfo.resDepth];
	for (int i=0; i < fittingInfo.resDepth; i++)
	    simVolume[i] = new imgT[fittingInfo.resWidth*fittingInfo.resHeight]();

	printf("%d,%d,%d\n", fittingInfo.resDepth,fittingInfo.resWidth,fittingInfo.resHeight );

	for (int i = 0; i < simList.listSize; i++)
	{
		for (int j = 0; j < simList.listRankSize; j++)
		{
	    	printf("%d,%d,%d\n", similarityResults[i][j].bmCoord.z,similarityResults[i][j].bmCoord.x,similarityResults[i][j].bmCoord.y);
	    	simVolume[similarityResults[i][j].bmCoord.z][ijn(similarityResults[i][j].bmCoord.x,similarityResults[i][j].bmCoord.y,fittingInfo.resWidth)] = 65000;
		}
	}


    DATAINFO savePixels;
    savePixels.fileName = (char *) malloc(100);
    strcpy(savePixels.fileName, "similarityPoints.raw");
    savePixels.resWidth = fittingInfo.resWidth;
    savePixels.resHeight = fittingInfo.resHeight;
    savePixels.resDepth = fittingInfo.resDepth;
    ///*if(*/data_vol1.saveModifiedImage<imgT>(subImg1, savePixels);//) printf("Image saved (%s)!\n", savePixels.fileName);
    //printf("A\n");
    Handle3DDataset <imgT>tosave(savePixels);
  
    if(tosave.saveModifiedDataset(simVolume, savePixels)) printf("Volume saved (%s)!\n", savePixels.fileName);
    else printf("erro!\n");
    //printf("A\n");
    free(savePixels.fileName);
    savePixels.fileName=NULL; 


}



