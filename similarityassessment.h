#ifndef SIMILARITYASSESSMENT
#define SIMILARITYASSESSMENT

#include <iostream>
#include <omp.h>

#include <handle3ddataset.h>
#include <qualityassessment.h>

#include "utils.h"

using namespace std;

class SimilarityAssessment
{
public:
	SimilarityAssessment();
	~SimilarityAssessment();
	vector<sliceRank> checkSimilarity(Handle3DDataset <imgT>data_vol1, Handle3DDataset <imgT>data_vol2, OPT options);
    void checkWithSubSSIM( Handle3DDataset <imgT>data_vol1, Handle3DDataset <imgT>data_vol2, OPT options);
    void splitDatasetCPU(Handle3DDataset <imgT>dataset, vector<Mat> &cv_dataset);
    void splitIntoSubImages(Mat img, vector<Mat> &subImgs, DATAINFO imgInfo, int KERNEL );    
    void splitIntoSubImages(imgT *img, vector<imgT*> &subImgs, DATAINFO imgInfo, int KERNEL );        
    bool isBlackImage (Mat image);
    bool isBlackImage(imgT *image, int resW, int resH);
    void saveDataCV( Mat image, int ttt,  int id, int iw, int ih);
    void saveDataRaw( imgT *imgRaw, int ttt, int id, int iw, int ih, int imgSize);
    void cvMatToRaw(Mat cvImg, imgT *&rawImg);
    void rawToCvMat(imgT *rawImg, int resW, int resH, Mat &cvImg);
private:
    Scalar similarityResult;
    sliceRank sliceAndDistance;
    vector<sliceRank> bestMatches;
    QualityAssessment qualAssess;
    
};





#endif // QUALITY_ASSESSMENT