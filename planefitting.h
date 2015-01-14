
#ifndef PLANE_FITTING
#define PLANE_FITTING

#include <fstream>
#include <cstdlib>
#include <cstdio>

#include <handle3ddataset.h>
#include "utils.h"

using namespace std;

class PlaneFitting
{
public:
	PlaneFitting();
	~PlaneFitting();
	void loadPoints(BM *similarityResults, DATAINFO fittingInfo);
	void planeFittingCalculations();
	Plane getPlaneFitted();
private:
	vector<Point3> pointsToFit;
	Plane planeFitted;
};


#endif // PLANE_FITTING