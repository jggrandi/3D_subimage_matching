#include "planefitting.h"

PlaneFitting::PlaneFitting(){}

PlaneFitting::~PlaneFitting(){}

void PlaneFitting::loadPoints(BM **similarityResults, DATAINFO fittingInfo, LISTPARAM listInfo)
{
	for (int i = 0; i < listInfo.listSize; i++)
	{

		for (int j = 0; j < listInfo.listRankSize; j++)
		{
			if(similarityResults[i][j].bmSimValue != -1111)
			{
				float px = ((float)similarityResults[i][j].bmCoord.x / (float)fittingInfo.resWidth * 2.0f) - 1.0f;
				float py = ((float)similarityResults[i][j].bmCoord.y / (float)fittingInfo.resHeight* 2.0f) - 1.0f;
				float pz = ((float)similarityResults[i][j].bmCoord.z / (float)fittingInfo.resHeight * 2.0f) - 1.0f;
				
				Point3 coord(px,py,pz);
				pointsToFit.push_back(coord);
				cout << similarityResults[i][j].bmCoord.x <<" " << similarityResults[i][j].bmCoord.y << " " << similarityResults[i][j].bmCoord.z <<endl;
				cout<< coord.x() << " " << coord.y() << " "<< coord.z() << endl;			
			}
			else
				cout << "Discarted" << endl;
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