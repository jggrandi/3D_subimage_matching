#include "planefitting.h"

PlaneFitting::PlaneFitting(){}

PlaneFitting::~PlaneFitting(){}

void PlaneFitting::loadPoints(BM **similarityResults, DATAINFO fittingInfo, LISTPARAM listInfo)
{

	int planeDirection[9]={0,0,0,0,0,0,0,0};

	for (int i = 0; i < listInfo.listSize; i++)
		for (int j = 0; j < listInfo.listRankSize; j++)
			if(similarityResults[i][j].bmSimValue != -1111)
				planeDirection[similarityResults[i][j].bmPlane]++;


	for (int xd=0; xd<9;xd++)
		printf("[%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",planeDirection[0],planeDirection[1],planeDirection[2],planeDirection[3],planeDirection[4],planeDirection[5],planeDirection[6],planeDirection[7],planeDirection[8] );

	int greaterDirection = 0;

	for(int ix = 0; ix < 8; ix++)
		if(planeDirection[ix+1] > planeDirection[greaterDirection])
			greaterDirection = ix+1;	

	printf("%d\n", greaterDirection );

	for (int i = 0; i < listInfo.listSize; i++)
	{
		for (int j = 0; j < listInfo.listRankSize; j++)
		{
			if((similarityResults[i][j].bmPlane == greaterDirection) && (similarityResults[i][j].bmSimValue != -1111))
			{
				float px = ((float)similarityResults[i][j].bmCoord.x / (float)fittingInfo.resWidth * 2.0f) - 1.0f;
				float py = ((float)similarityResults[i][j].bmCoord.y / (float)fittingInfo.resHeight* 2.0f) - 1.0f;
				float pz = ((float)similarityResults[i][j].bmCoord.z / (float)fittingInfo.resHeight * 2.0f) - 1.0f;
				
				Point3 coord(px,py,pz);
				pointsToFit.push_back(coord);				
			}
		}	
	}


}

void PlaneFitting::planeFittingCalculations()
{
	if(pointsToFit.size() > 0)
		linear_least_squares_fitting_3(pointsToFit.begin(),pointsToFit.end(),planeFitted,CGAL::Dimension_tag<0>());
}

Plane PlaneFitting::getPlaneFitted()
{
	return planeFitted;
}

vector<Point3> PlaneFitting::getPointsToFit()
{
	return pointsToFit;
}