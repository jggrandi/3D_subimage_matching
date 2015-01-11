#include "similarityassessment.h"

#define ijn(a,b,n) ((a)*(n))+b


SimilarityAssessment::SimilarityAssessment(){}

SimilarityAssessment::~SimilarityAssessment(){}


vector<sliceRank> SimilarityAssessment::checkSimilarity(Handle3DDataset <imgT>data_vol1, Handle3DDataset <imgT>data_vol2, OPT options)
{
    //essa função existe para suportar outras métricas de similaridade no futuro. Atualmente só SSIM é suportado para 3D.
    checkWithSubSSIM(data_vol1, data_vol2, options);

    return bestMatches;
}

void SimilarityAssessment::checkWithSubSSIM(Handle3DDataset <imgT>data_vol1, Handle3DDataset <imgT>data_vol2, OPT options)
{
    vector<Mat> cvD1, cvD2;

    if(options.gpuOptimized) { printf("Sub image SSIM calculations in GPU is not implemented yet...\n");}
    else    { splitDatasetCPU(data_vol1,cvD1); splitDatasetCPU(data_vol2,cvD2);}

    DATAINFO imgInfoDataset1 = data_vol1.getDatasetInfo(0);
    DATAINFO imgInfoDataset2 = data_vol2.getDatasetInfo(0); 

    int FACTOR = options.subDivideFactor;
    int KERNEL = imgInfoDataset1.resWidth / FACTOR;
    int OFFSET = KERNEL;
    int PLANES = 9;
    int SAMPLING = 16;
    int INITIAL_SLICE = 16;

    int NRO_OF_SUBIMAGES = (imgInfoDataset1.resWidth*imgInfoDataset1.resHeight)/(KERNEL*KERNEL);

    int simID = 0;
    float simMax = 0;

    imgT **raw_vol1 = data_vol1.getDataset(0);
    imgT **raw_vol2 = data_vol2.getDataset(0);

    //printf("%d\n", NRO_OF_SUBIMAGES);
    if(!options.gpuOptimized) // versao nao otimizada por gpu
    {
        for( int id = INITIAL_SLICE; id < imgInfoDataset1.resDepth; id+=16) // para todas as imagens do primeiro dataset
        {

            float *simAverage = (float*)calloc(imgInfoDataset1.resDepth,sizeof(float)); // armazena a média das similaridades das subimagens da imagem
            
            // float **simScores = (float**)calloc(imgInfoDataset1.resDepth,sizeof(float*)); //aloca espaço para salvar scores 
            // for (int i=0; i < imgInfoDataset1.resDepth; i++)
            //     simScores[i] = (float*)calloc(NRO_OF_SUBIMAGES, sizeof(float));

            // BM **bestMatches = (BM**)calloc(imgInfoDataset1.resWidth, sizeof(BM*));
            // for (int i=0; i < imgInfoDataset1.resWidth; i++)
            //     bestMatches[i] = (BM*)calloc(imgInfoDataset1.resHeight, sizeof(BM));

            BM *bestMatches = new BM [imgInfoDataset1.resWidth*imgInfoDataset1.resHeight];

            // vector<Mat> subImgs1;
            // splitIntoSubImagesCV(cvD1[id], subImgs1, imgInfoDataset1, KERNEL); // separa uma imagem cvD1[id] em sub imagens (salvas em subImgs1). KERNEL*KERNEL é a resolução total da subimagem 

            vector<imgT*> subImgs1;
            splitIntoSubImages(raw_vol1[id], subImgs1, imgInfoDataset1, KERNEL);

            //saveData( subImgs1[7], 123, iw, ih, ih, KERNEL);

            
        //#pragma omp parallel for //realiza em paralelo na CPU as comparações

            int simScoreiT1 = 0;
            int nextSubImg1 = 0;
            
            vector<imgT*> subImgVol;                
            

            for (int id2 = 0; id2 < imgInfoDataset2.resDepth-OFFSET+1; id2+=SAMPLING)            
            {    
                printme(id2);
                for (int i = 0; i < imgInfoDataset2.resWidth-OFFSET+1; i+=SAMPLING)
                {
                    for (int j = 0; j < imgInfoDataset2.resHeight-OFFSET+1; j+=SAMPLING)
                    {
                        for (int p = 0; p < PLANES; p++)
                        {
                            imgT *subImgVolTmp = (imgT*)calloc(KERNEL*KERNEL,sizeof(imgT*));//sub imagens do volume de entrada        
                            //memset(subImgVolTmp,0,KERNEL*KERNEL*sizeof(imgT*));
                            buildImagePlanes(id2,i,j,imgInfoDataset2.resWidth,KERNEL,raw_vol2,p,subImgVolTmp);
                            subImgVol.push_back(subImgVolTmp);
                            //saveData(subImgVolTmp, 444, p, i, j, KERNEL);
                        }
                    }
                }
            }   
            waitme();

            // Mat subImg1Cv(KERNEL,KERNEL,CV_16UC1, subImgs1[0]);
            // Mat subImg2Cv(KERNEL,KERNEL,CV_16UC1, subImgVol[0]);
            // similarityResult = qualAssess.getMSSIM(subImg1Cv, subImg2Cv);
            // printme(similarityResult.val[0]);     
            // waitme();    
            // saveData(subImgs1[0], 444, 1, 1, 1, KERNEL);
            // saveData(subImgVol[0], 444, 2, 2, 2, KERNEL);
            
            printme(subImgVol.size());

            for(auto img:subImgs1)
            {
            // for (int iw = 0; iw < imgInfoDataset1.resWidth; iw+=KERNEL) //percorre imagem pixel //linha
            // {
            //     for (int ih = 0; ih < imgInfoDataset1.resHeight; ih+=KERNEL) //percorre imagem pixel //coluna
            //     {
                    bestMatches[nextSubImg1].bmSimValue = -1111;
                    cout << "IIIIIMMMMAAGGEEE" << endl;

//                    if(!isBlackImage(subImgs1[nextSubImg1], KERNEL, KERNEL))
                    if(!isBlackImage(img, KERNEL, KERNEL,1))
                    {
                        //waitme();
                        BM bestNow;
                        bestNow.bmSimValue = -1111;

                        int nextSubImg2 = 0;
                        vector<imgT*>::iterator it = subImgVol.begin();
                        
                        for (int id2 = 0; id2 < imgInfoDataset2.resDepth-OFFSET; id2+=SAMPLING) // para todas as imagens do segundo dataset
                        //for (int id2 = 0; id2 < 1; id2+=SAMPLING) // para todas as imagens do segundo dataset
                        {
                            for (int iw2 = 0; iw2 < imgInfoDataset2.resWidth-OFFSET; iw2+=SAMPLING)
                            {
                                for (int ih2 = 0; ih2 < imgInfoDataset2.resHeight-OFFSET; ih2+=SAMPLING) //percorre imagem pixel //coluna
                                {   
                                    for (int p = 0; p < PLANES; p++)
                                    {
                                        //buildImagePlanes(id2,iw2,ih2,imgInfoDataset2.resWidth,KERNEL,raw_vol2,p,subImgVol);

                                       // if(!isBlackImage(subImgVol[nextSubImg2],KERNEL,KERNEL))
                                        if(!isBlackImage(*it,KERNEL,KERNEL))
                                        {
                                            //saveData(plane, 444, p, iw2, ih2, KERNEL);
                                            Mat subImg1Cv(KERNEL,KERNEL,CV_16UC1, img);
                                            Mat subImg2Cv(KERNEL,KERNEL,CV_16UC1, *it);
            /*SIM CALCULATION*/             similarityResult = qualAssess.getMSSIM(subImg1Cv, subImg2Cv);

                                            if(similarityResult.val[0] > bestNow.bmSimValue)
                                            {
                                                bestNow.bmSimValue = similarityResult.val[0];
                                                //bestNow.bmColorValue = raw_vol2[id][ijn(iw,ih,imgInfoDataset2.resWidth)];
                                                bestNow.bmPlane = p;
                                                bestNow.bmCoord.x = id2;
                                                bestNow.bmCoord.y = iw2;
                                                bestNow.bmCoord.z = ih2;
                                            }
                                        }
                                        it++;
                                        nextSubImg2++;

                                    }
                                }
                            }
                            //exit(0);
                            //simScoreiT1++;
                        }
                        bestMatches[nextSubImg1] = bestNow;
                        printme(bestMatches[nextSubImg1]);
                    }
                    else
                        printme("blackk");
                    nextSubImg1++;
                }

                //printf("%d\n",simScoreiT1 );

                // for( int ii = 0; ii < simScoreiT1; ii++)
                // {
                //     //printf("%f\n", simScores[id2][ii]);
                //     simAverage[id2] = simAverage[id2] + simScores[id2][ii];
                // }

                // simAverage[id2] = simAverage[id2] / simScoreiT1;
            
                // if (simAverage[id2] >= simMax)
                // {
                //     simMax = simAverage[id2];
                //     simID  = id2;
                // }

                //free(simAverage);
               // simAverage = NULL;
            //}



            // sliceAndDistance.sliceNumber = id;
            // sliceAndDistance.distanceToOptimal = simID-id;
            // sliceAndDistance.value=simMax;
            // bestMatches.push_back(sliceAndDistance);
            
            if(options.verbose) printf("[%d]=[%d]-%f\n",id,simID,simMax );
            
            simMax = 0;
            simID = 0;
        }
    }
}


void SimilarityAssessment::buildImagePlanes(int d, int w, int h, int resW, int PBASE, imgT **&raw_vol1, int diag_type, imgT *&t)
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

            t[ijn(i,j,PBASE)] = raw_vol1[iC1][ijn(jC1,jC2,resW)];

        }           
    }
    // if(diag_type == 2)
    // {
    //     saveData(t, 444, d, w, h, PBASE);
    //     exit(0);
    // }

}


void SimilarityAssessment::splitIntoSubImages(imgT *img, vector<imgT*> &subImgs, DATAINFO imgInfo, int KERNEL )       
{    
    Mat slice(imgInfo.resHeight,imgInfo.resWidth,CV_16UC1,img);

    for (int iw = 0; iw < imgInfo.resWidth; iw+=KERNEL) //percorre imagem pixel //linha
    {
        for (int ih = 0; ih < imgInfo.resHeight; ih+=KERNEL) //percorre imagem pixel //coluna
        {
            Mat tempSubImage(slice, Rect(iw, ih, KERNEL, KERNEL)); // monta subimagem
            
            imgT *savedSubImg = (imgT*)malloc(sizeof(imgT) * (KERNEL*KERNEL));
            cvMatToRaw(tempSubImage, savedSubImg);
            subImgs.push_back(savedSubImg);
            //saveData( savedSubImg, 123, iw, ih, ih, KERNEL);
        }
    }
}


void SimilarityAssessment::splitIntoSubImages(Mat img, vector<Mat> &subImgs, DATAINFO imgInfo, int KERNEL )
{
    
    for (int iw = 0; iw < imgInfo.resWidth; iw+=KERNEL) //percorre imagem pixel //linha
    {
        for (int ih = 0; ih < imgInfo.resHeight; ih+=KERNEL) //percorre imagem pixel //coluna
        {
            Mat tempSubImage(img, Rect(iw, ih, KERNEL, KERNEL)); // monta subimagem
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
    int blackLimit =  (resW*resH)*0.85;

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
    savePixels.resDepth = savePixels.resWidth * savePixels.resHeight;
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