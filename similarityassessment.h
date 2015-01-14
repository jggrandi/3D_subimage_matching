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
	void checkSimilarity(Handle3DDataset <imgT>data_vol1, Handle3DDataset <imgT>data_vol2, OPT options);
    BM*  checkWithSubSSIM( imgT *inputImg, DATAINFO infoImg, imgT **inputVol, DATAINFO infoVol, OPT options);
    void splitDatasetCPU(Handle3DDataset <imgT>dataset, vector<Mat> &cv_dataset);
    void splitIntoSubImages(Mat img, vector<Mat> &subImgs, DATAINFO imgInfo);    
    void splitIntoSubImages(imgT *img, vector<imgT*> &subImgs, DATAINFO imgInfo);
    void bufferVolumePlanes(imgT **&raw_vol, vector<imgT*> &subImgs, DATAINFO imgInfo);      
    bool isBlackImage (Mat image);
    bool isBlackImage(imgT *image, int resW, int resH, int show = 0);
    void saveData( Mat image, int ttt,  int id, int iw, int ih);
    void saveData( imgT *imgRaw, int ttt, int id, int iw, int ih, int imgSize);
    void saveData( imgT **volRaw, int ttt, int id, int iw, int ih, int imgSize);
    void cvMatToRaw(Mat cvImg, imgT *&rawImg);
    void rawToCvMat(imgT *rawImg, int resW, int resH, Mat &cvImg);
    void buildImagePlanes(int d, int w, int h, int resW, int PBASE, imgT **&raw_vol, int diag_type, imgT *&planeResult);
    BM* getBestMatches();
    DATAINFO getFittingInfo();

private:
    Scalar similarityResult;
    BM *bestMatches;
    DATAINFO imgInfoSlice;
    QualityAssessment qualAssess;
    FP fP;
    
};





#endif // QUALITY_ASSESSMENT