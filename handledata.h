
#ifndef HANDLE_DATA
#define HANDLE_DATA

#include <fstream>
#include <cstdlib>
#include <cstdio>

#include "similarityassessment.h"
#include "utils.h"
#include "logdata.h"
#include "planefitting.h"


class HandleData
{
public:
	HandleData();
	~HandleData();
	int  loadData(DATAINFO h_img1, DATAINFO h_img2, OPT h_options);
	void similarityCheck();
	void planeFitting();
	void volumetricSimilarityValues();
	float getRunningTime();
	void printInputPlane();
	void printFittedPlane();
	void angleBetweenPlanes();
	void distanceBetweenPlanes();
	void printAngleBetweenPlanes();
	void printDistanceBetweenPlanes();

private:
	Handle3DDataset <imgT>dataset1;
	Handle3DDataset <imgT>dataset2;
	DATAINFO infoDataset1;
	DATAINFO infoDataset2;
	SimilarityAssessment simAssess;
	PlaneFitting planeFit;
	DATAINFO fittingInfo;
	OPT oopt;
	LogData logData;
	BM **similarityResults;
	LISTPARAM simList;
	float simCheckRuntime;
	PLANEEQ inputPlane;
	PLANEEQ fittedPlane;
	FP functionalParameters;
	float angleError;
	float distanceError;
};


#endif // HANDLE_DATA