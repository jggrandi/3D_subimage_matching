
#ifndef HANDLE_DATA
#define HANDLE_DATA

#include <fstream>
#include <cstdlib>
#include <cstdio>



#include <handle3ddataset.h>

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
	//void planeFitting();
	void volumetricSimilarityValues();
	float getRunningTime();
private:
	Handle3DDataset <imgT>dataset1;
	Handle3DDataset <imgT>dataset2;
	SimilarityAssessment simAssess;
	PlaneFitting planeFit;
	DATAINFO fittingInfo;
	OPT oopt;
	LogData logData;
	BM **similarityResults;
	float simCheckRuntime;
};


#endif // HANDLE_DATA