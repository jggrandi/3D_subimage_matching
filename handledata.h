
#ifndef HANDLE_DATA
#define HANDLE_DATA

#include <fstream>
#include <cstdlib>
#include <cstdio>

#include <handle3ddataset.h>

#include "similarityassessment.h"
#include "utils.h"
#include "logdata.h"

class HandleData
{
public:
	HandleData();
	~HandleData();
	int  loadData(DATAINFO h_img1, DATAINFO h_img2, OPT h_options);
	void similarityCheck();
	void showData();
private:
	Handle3DDataset <imgT>dataset1;
	Handle3DDataset <imgT>dataset2;
	SimilarityAssessment q;
	OPT oopt;
	LogData logData;
	vector<sliceRank> similarityResults;
};


#endif // HANDLE_DATA