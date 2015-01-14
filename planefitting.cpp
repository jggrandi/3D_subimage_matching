#include "planefitting.h"

PlaneFitting::PlaneFitting(){}

PlaneFitting::~PlaneFitting(){}

void PlaneFitting::loadPoints(BM *similarityResults, DATAINFO fittingInfo)
{

	for (int i = 1; i < 49; i++)
	{
		float px = ((float)similarityResults[i].bmCoord.x / (float)fittingInfo.resWidth * 2.0f) - 1.0f;
		float py = ((float)similarityResults[i].bmCoord.y / (float)fittingInfo.resHeight* 2.0f) - 1.0f;
		float pz = ((float)similarityResults[i].bmCoord.z / (float)fittingInfo.resHeight * 2.0f) - 1.0f;
		
		Point3 coord(px,py,pz);
		pointsToFit.push_back(coord);
		cout<< coord.x() << " " << coord.y() << " "<< coord.z() << endl;
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