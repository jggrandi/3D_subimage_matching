#include "similarityassessment.h"

#define ijn(a,b,n) ((a)*(n))+b


SimilarityAssessment::SimilarityAssessment(){}

SimilarityAssessment::~SimilarityAssessment(){}


void SimilarityAssessment::checkSimilarity(Handle3DDataset <imgT>data_vol1, Handle3DDataset <imgT>data_vol2, OPT options)
{

    DATAINFO imgInfoDataset1 = data_vol1.getDatasetInfo(0);
    DATAINFO imgInfoDataset2 = data_vol2.getDatasetInfo(0); 

    imgT **raw_vol1 = data_vol1.getDataset(0);
    //imgT **raw_vol1 = data_vol1.changePlane('s');
    imgT **raw_vol2 = data_vol2.getDataset(0);

    fP.FACTOR = options.subDivideFactor;
    fP.KERNEL = imgInfoDataset1.resWidth / fP.FACTOR;
    fP.OFFSET = fP.KERNEL;
    fP.PLANES = 9;
    fP.SAMPLING = 32;
    fP.INITIAL_SLICE = 0;

    imgT *imgPlane = (imgT*)calloc(imgInfoDataset1.resWidth*imgInfoDataset1.resHeight,sizeof(imgT*));
    buildImagePlanes(0,0,0,imgInfoDataset1.resWidth,imgInfoDataset1.resWidth,raw_vol1,0,imgPlane);

    DATAINFO imgInfoSlice;
    imgInfoSlice.resWidth = imgInfoDataset1.resWidth;
    imgInfoSlice.resHeight = imgInfoDataset1.resHeight;

    bestMatches = checkWithSubSSIM(imgPlane, imgInfoSlice, raw_vol2, imgInfoDataset2, options);

}

BM* SimilarityAssessment::checkWithSubSSIM( imgT *inputImg, DATAINFO infoImg, imgT **inputVol, DATAINFO infoVol, OPT options)
{

    BM *bM = new BM [infoImg.resWidth*infoImg.resHeight];

    if(!options.gpuOptimized) // versao nao otimizada por gpu
    {
        int nextSubImg1 = 0;

        vector<imgT*> subImgs1;
        splitIntoSubImages(inputImg, subImgs1, infoImg);
        
        vector<imgT*> subImgVol;                
        bufferVolumePlanes(inputVol, subImgVol, infoVol);

        waitme();
        
        printme(subImgVol.size());

        for(auto img:subImgs1) // para cada sub imagem da imagem de entrada
        {
            bM[nextSubImg1].bmSimValue = -1111;

            if(!isBlackImage(img, fP.KERNEL, fP.KERNEL,1))
            {
                //waitme();
                BM bestNow;
                bestNow.bmSimValue = -1111;
                
                
                //#pragma omp parallel for
                for (int id2 = 0; id2 < infoVol.resDepth-fP.OFFSET+1; id2+=fP.SAMPLING) // para todas as imagens do segundo dataset
                {            
                    vector<imgT*>::iterator it = subImgVol.begin();    
                    for (int iw2 = 0; iw2 < infoVol.resWidth-fP.OFFSET+1; iw2+=fP.SAMPLING)
                    {
                        for (int ih2 = 0; ih2 < infoVol.resHeight-fP.OFFSET+1; ih2+=fP.SAMPLING) //percorre imagem pixel //coluna
                        {   
                            for (int p = 0; p < fP.PLANES; p++)
                            {
                                if(!isBlackImage(*it,fP.KERNEL,fP.KERNEL))
                                {
                                    //saveData(plane, 444, p, iw2, ih2, fP.KERNEL);
                                    Mat subImg1Cv(fP.KERNEL,fP.KERNEL,CV_16UC1, img);
                                    Mat subImg2Cv(fP.KERNEL,fP.KERNEL,CV_16UC1, *it);
    /*SIM CALCULATION*/             similarityResult = qualAssess.getMSSIM(subImg1Cv, subImg2Cv);

                                    if(similarityResult.val[0] > bestNow.bmSimValue)
                                    {
                                        bestNow.bmSimValue = similarityResult.val[0];
                                        //bestNow.bmColorValue = raw_vol2[id][ijn(iw,ih,infoVol.resWidth)];
                                        bestNow.bmPlane = p;
                                        bestNow.bmCoord.z = id2;
                                        bestNow.bmCoord.x = iw2;
                                        bestNow.bmCoord.y = ih2;
                                    }
                                }
                                it++;
                            }
                        }
                    }
                }
                bM[nextSubImg1] = bestNow;
                printme(bM[nextSubImg1]);
            }
            else
                printme("blackk");
            nextSubImg1++;
        }
    }
    return bM;
}

BM* SimilarityAssessment::getBestMatches()
{
    return bestMatches;
}

void SimilarityAssessment::bufferVolumePlanes(imgT **&raw_vol, vector<imgT*> &subImgVol, DATAINFO imgInfo)
{
    for (int id2 = 0; id2 < imgInfo.resDepth-fP.OFFSET+1; id2+=fP.SAMPLING)            
    {    
        for (int i = 0; i < imgInfo.resWidth-fP.OFFSET+1; i+=fP.SAMPLING)
        {
            for (int j = 0; j < imgInfo.resHeight-fP.OFFSET+1; j+=fP.SAMPLING)
            {
                for (int p = 0; p < fP.PLANES; p++)
                {
                    imgT *subImgVolTmp = (imgT*)calloc(fP.KERNEL*fP.KERNEL,sizeof(imgT*));//sub imagens do volume de entrada        
                    buildImagePlanes(id2,i,j,imgInfo.resWidth,fP.KERNEL,raw_vol,p,subImgVolTmp);
                    subImgVol.push_back(subImgVolTmp);
                }
            }
        }
    }   
}

void SimilarityAssessment::buildImagePlanes(int d, int w, int h, int resW, int PBASE, imgT **&raw_vol, int diag_type, imgT *&planeResult)
{
    int iC1,jC1,jC2;
    iC1=jC1=jC2=0;

    const int DELTA = 0;
    for(int i = 0; i < PBASE; i++)
    {
        for(int j = 0; j < PBASE; j++)
        {
            switch(diag_type)
            {
                case 0:
                    iC1 = d+DELTA;
                    jC1 = w+i;
                    jC2 = h+j;
                break;
                case 1:
                    iC1 = d+i;
                    jC1 = w+j;
                    jC2 = h+j;                 
                break;
                case 2:
                    iC1 = d+i;
                    jC1 = w+j;
                    jC2 = h+(PBASE-j-1);               
                break;
                case 3:
                    iC1 = d+j;
                    jC1 = w+DELTA;
                    jC2 = h+i;             
                break;
                case 4:
                    iC1 = d+i;
                    jC1 = w+j;
                    jC2 = h+i;             
                break;
                case 5:
                    iC1 = d+(PBASE-i-1);
                    jC1 = w+j;
                    jC2 = h+(PBASE-i-1);               
                break;
                case 6:
                    iC1 = d+j;
                    jC1 = w+i;
                    jC2 = h+DELTA;             
                break;
                case 7:
                    iC1 = d+i;
                    jC1 = w+i;
                    jC2 = h+j;             
                break;
                case 8:
                    iC1 = d+(PBASE-i-1);
                    jC1 = w+(PBASE-i-1);
                    jC2 = h+j;             
                break;          
                default:
                break;
            }
            // if(diag_type == 2)       
            //     cout << iC1 << " " << jC1 << " " << jC2 << endl;

            planeResult[ijn(i,j,PBASE)] = raw_vol[iC1][ijn(jC1,jC2,resW)];

        }           
    }
    // if(diag_type == 2)
    // {
         //saveData(planeResult, 444, d, w, h, PBASE);
         //exit(0);
    // }

}


void SimilarityAssessment::splitIntoSubImages(imgT *img, vector<imgT*> &subImgs, DATAINFO imgInfo)       
{    
    Mat slice(imgInfo.resHeight,imgInfo.resWidth,CV_16UC1,img);

    for (int iw = 0; iw < imgInfo.resWidth-fP.OFFSET+1; iw+=fP.SAMPLING) //percorre imagem pixel //linha
    {
        for (int ih = 0; ih < imgInfo.resHeight-fP.OFFSET+1; ih+=fP.SAMPLING) //percorre imagem pixel //coluna
        {
            Mat tempSubImage(slice, Rect(iw, ih, fP.KERNEL, fP.KERNEL)); // monta subimagem
            
            imgT *savedSubImg = (imgT*)malloc(sizeof(imgT) * (fP.KERNEL*fP.KERNEL));
            cvMatToRaw(tempSubImage, savedSubImg);
            subImgs.push_back(savedSubImg);
            //saveData( savedSubImg, 123, iw, ih, ih, fP.KERNEL);
        }
    }
    slice.release();
    free(img);
    img = NULL;
}


void SimilarityAssessment::splitIntoSubImages(Mat img, vector<Mat> &subImgs, DATAINFO imgInfo )
{
    
    for (int iw = 0; iw < imgInfo.resWidth; iw+=fP.KERNEL) //percorre imagem pixel //linha
    {
        for (int ih = 0; ih < imgInfo.resHeight; ih+=fP.KERNEL) //percorre imagem pixel //coluna
        {
            Mat tempSubImage(img, Rect(iw, ih, fP.KERNEL, fP.KERNEL)); // monta subimagem
            subImgs.push_back(tempSubImage);
            //saveDataCV( tempSubImage, 666, iw, ih, ih);
        }
        
    }
}

void SimilarityAssessment::splitDatasetCPU(Handle3DDataset <imgT>dataset, vector<Mat> &cv_dataset)
{
    
    DATAINFO imgInfo = dataset.getDatasetInfo(0);   
    imgT** d = dataset.getDataset(0);

    for( int i = 0; i < imgInfo.resDepth; i++ )
    {
        Mat slice(imgInfo.resHeight,imgInfo.resWidth,CV_16UC1,d[i]);
        cv_dataset.push_back(slice);
    }

}


bool SimilarityAssessment::isBlackImage(Mat image)
{

    bool isBlack = false; 
    int blackImage = 0;
    int blackLimit =  (image.rows*image.cols)*0.85;

    for (int i = 0; i < image.rows; i++)
        for (int j = 0; j < image.cols; j++)
            if(image.at<imgT>(i,j) == 0)
                blackImage++;
    
    if(blackImage >= blackLimit) isBlack = true;
    else                         isBlack = false;

    return isBlack;
}

bool SimilarityAssessment::isBlackImage(imgT *image, int resW, int resH, int show)
{
    // static int idx = 0;
    // saveData( image, 123, resW, resH, idx++, 1);

    bool isBlack = false; 
    int blackImage = 0;
    int blackLimit =  (resW*resH)*0.75;

    for (int i = 0; i < resW; i++)
        for (int j = 0; j < resH; j++)
            if(image[ijn(i,j,resW)] == 0)
                blackImage++;
    
    if(blackImage >= blackLimit) isBlack = true;
    else                         isBlack = false;

    
    // if(show > 0)
    //     printme((float)blackImage/(resW*resH));
    
    return isBlack;
}


void SimilarityAssessment::saveData( Mat image, int ttt, int id, int iw, int ih)
{

    imgT *imgRaw = (imgT*)malloc(sizeof(imgT) * (image.rows*image.cols));
    cvMatToRaw(image, imgRaw);
    
    stringstream output;
    output << "subimages/subImgD1_"<<ttt<<"x"<< id <<"x"<<iw<<"x"<<ih<< ".raw";
    string sulfix = output.str();
    const char* ss = sulfix.c_str();

    DATAINFO savePixels;
    savePixels.fileName = (char *) malloc(100);
    strcpy(savePixels.fileName, ss);
    savePixels.resWidth = image.cols;
    savePixels.resHeight = image.rows;
    savePixels.resDepth = savePixels.resWidth * savePixels.resHeight;
    ///*if(*/data_vol1.saveModifiedImage<imgT>(subImg1, savePixels);//) printf("Image saved (%s)!\n", savePixels.fileName);
    //printf("A\n");
    Handle3DDataset <imgT>tosave(savePixels);
    /*if(*/tosave.saveModifiedImage(imgRaw, savePixels);//) printf("Image saved (%s)!\n", savePixels.fileName);
    //printf("A\n");
    free(savePixels.fileName);
    savePixels.fileName=NULL; 
}


void SimilarityAssessment::saveData( imgT **volRaw, int ttt, int id, int iw, int ih, int imgSize)
{

    stringstream output;
    output << "subimages/subVol_"<<ttt<<"x"<< id <<"x"<<iw<<"x"<<ih<< ".raw";
    string sulfix = output.str();
    const char* ss = sulfix.c_str();

    DATAINFO savePixels;
    savePixels.fileName = (char *) malloc(100);
    strcpy(savePixels.fileName, ss);
    savePixels.resWidth = imgSize;
    savePixels.resHeight = imgSize;
    savePixels.resDepth = imgSize;
    ///*if(*/data_vol1.saveModifiedImage<imgT>(subImg1, savePixels);//) printf("Image saved (%s)!\n", savePixels.fileName);
    //printf("A\n");
    Handle3DDataset <imgT>tosave(savePixels);
    /*if(*/tosave.saveModifiedDataset(volRaw, savePixels);//) printf("Image saved (%s)!\n", savePixels.fileName);
    //printf("A\n");
    free(savePixels.fileName);
    savePixels.fileName=NULL; 
}


void SimilarityAssessment::saveData( imgT *imgRaw, int ttt, int id, int iw, int ih, int imgSize)
{

    stringstream output;
    output << "subimages/subImgD1_"<<ttt<<"x"<< id <<"x"<<iw<<"x"<<ih<< ".raw";
    string sulfix = output.str();
    const char* ss = sulfix.c_str();

    DATAINFO savePixels;
    savePixels.fileName = (char *) malloc(100);
    strcpy(savePixels.fileName, ss);
    savePixels.resWidth = imgSize;
    savePixels.resHeight = imgSize;
    savePixels.resDepth = imgSize;
    ///*if(*/data_vol1.saveModifiedImage<imgT>(subImg1, savePixels);//) printf("Image saved (%s)!\n", savePixels.fileName);
    //printf("A\n");
    Handle3DDataset <imgT>tosave(savePixels);
    /*if(*/tosave.saveModifiedImage(imgRaw, savePixels);//) printf("Image saved (%s)!\n", savePixels.fileName);
    //printf("A\n");
    free(savePixels.fileName);
    savePixels.fileName=NULL; 
}




void SimilarityAssessment::cvMatToRaw(Mat cvImg, imgT *&rawImg)
{
    for (int i = 0; i < cvImg.rows; i++)
        for (int j = 0; j < cvImg.cols; j++)
            rawImg[ijn(i,j,cvImg.rows)] = cvImg.at<imgT>(i,j);    
}

void SimilarityAssessment::rawToCvMat(imgT *rawImg, int resW, int resH, Mat &cvImg)
{
    Mat temp(resH,resW,CV_16UC1,rawImg);
    cvImg = temp.clone();   

}