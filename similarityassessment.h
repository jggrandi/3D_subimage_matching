#ifndef SIMILARITYASSESSMENT
#define SIMILARITYASSESSMENT

#include <iostream>
#include <omp.h>

#include <handle3ddataset.h>
#include <qualityassessment.h>

#include "utils.h"

using namespace std;


#define printme(e){cout << #e << "=" <<(e) << endl;}
#define waitme(){int x; cout << "Press something to continue...\n"; cin >> x;}

typedef struct point3int
{
    int x;
    int y;
    int z;
}P3i;

typedef struct bestMatch
{
    double  bmSimValue;
    char    bmColorValue;
    int     bmPlane;
    P3i     bmCoord; 
}BM;

inline std::ostream &operator<<( std::ostream &out, const BM bm ) {
return out << "(" << bm.bmSimValue << "; " << bm.bmPlane << ")" << endl;
} 


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
    bool isBlackImage(imgT *image, int resW, int resH, int show = 0);
    void saveData( Mat image, int ttt,  int id, int iw, int ih);
    void saveData( imgT *imgRaw, int ttt, int id, int iw, int ih, int imgSize);
    void cvMatToRaw(Mat cvImg, imgT *&rawImg);
    void rawToCvMat(imgT *rawImg, int resW, int resH, Mat &cvImg);
    void buildImagePlanes(int d, int w, int h, int resW, int PBASE, imgT **&raw_vol1, int diag_type, imgT *&t);


private:
    Scalar similarityResult;
    sliceRank sliceAndDistance;
    vector<sliceRank> bestMatches;
    QualityAssessment qualAssess;
    
};





#endif // QUALITY_ASSESSMENT