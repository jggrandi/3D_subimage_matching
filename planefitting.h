
#ifndef PLANE_FITTING
#define PLANE_FITTING

#include <fstream>
#include <cstdlib>
#include <cstdio>

#include "utils.h"

using namespace std;

class PlaneFitting
{
public:
	PlaneFitting();
	~PlaneFitting();
	void loadPoints(BM **similarityResults, DATAINFO fittingInfo, LISTPARAM listInfo);
	void planeFittingCalculations();
	Plane getPlaneFitted();
	vector<Point3> getPointsToFit();
private:
	vector<Point3> pointsToFit;
	Plane planeFitted;
};


#endif // PLANE_FITTING