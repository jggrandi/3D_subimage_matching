#ifndef SIMILARITYASSESSMENT
#define SIMILARITYASSESSMENT

#include <iostream>

#include <handle3ddataset.h>
#include <qualityassessment.h>

#include "utils.h"

using namespace std;

class SimilarityAssessment
{
public:
	SimilarityAssessment();
	~SimilarityAssessment();
	vector<sliceRank> checkSimilarity(Handle3DDataset <imgT>dataset1, Handle3DDataset <imgT>dataset2, OPT options);
    void checkWithSubSSIM( Handle3DDataset <imgT>dataset1, Handle3DDataset <imgT>dataset2, OPT options);
    void splitDatasetCPU(Handle3DDataset <imgT>dataset, vector<Mat> &cv_dataset);
    void splitIntoSubImages(Mat img, vector<Mat> &subImgs, DATAINFO imgInfo, int KERNEL );    
    bool isBlackImage (Mat image);
    void saveDataRaw( Mat image, int ttt,  int id, int iw, int ih);
private:
    Scalar similarityResult;
    sliceRank sliceAndDistance;
    vector<sliceRank> bestMatches;
    QualityAssessment qualAssess;
    
};





#endif // QUALITY_ASSESSMENT