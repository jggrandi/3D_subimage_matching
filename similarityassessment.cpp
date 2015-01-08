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
    int INITIAL_SLICE = 0;

    int NRO_OF_SUBIMAGES = (imgInfoDataset1.resWidth*imgInfoDataset1.resHeight)/(KERNEL*KERNEL);

    int simID = 0;
    float simMax = 0;

    imgT **raw_vol1 = data_vol1.getDataset(0);
    imgT **raw_vol2 = data_vol2.getDataset(0);

    //printf("%d\n", NRO_OF_SUBIMAGES);
    if(!options.gpuOptimized) // versao nao otimizada por gpu
    {
        for( int id = INITIAL_SLICE; id < imgInfoDataset1.resDepth*3; id++) // para todas as imagens do primeiro dataset
        {

            float *simAverage = (float*)calloc(imgInfoDataset1.resDepth*3,sizeof(float)); // armazena a média das similaridades das subimagens da imagem
            
            float **simScores = (float**)calloc(imgInfoDataset1.resDepth*3,sizeof(float*)); //aloca espaço para salvar scores 
            for (int i=0; i < imgInfoDataset1.resDepth*3; i++)
                simScores[i] = (float*)calloc(NRO_OF_SUBIMAGES, sizeof(float));

            // vector<Mat> subImgs1;
            // splitIntoSubImagesCV(cvD1[id], subImgs1, imgInfoDataset1, KERNEL); // separa uma imagem cvD1[id] em sub imagens (salvas em subImgs1). KERNEL*KERNEL é a resolução total da subimagem 

            vector<imgT*> subImgs1;
            splitIntoSubImages(raw_vol1[id], subImgs1, imgInfoDataset1, KERNEL);

            
            #pragma omp parallel for //realiza em paralelo na CPU as comparações
            for (int id2 = 0; id2 < imgInfoDataset2.resDepth*3; id2++) // para todas as imagens do segundo dataset
            {

                int simScoreiT1 = 0;
                int nextSubImg1 = 0;

                vector<Mat> subImgs2;
                splitIntoSubImages(cvD2[id2], subImgs2, imgInfoDataset2, KERNEL); // separa uma imagem cvD2[id2] em sub imagens (salvas em subImgs2). KERNEL*KERNEL é a resolução total da subimagem 

                for (int iw = 0; iw < imgInfoDataset1.resWidth; iw+=KERNEL) //percorre imagem pixel //linha
                {
                    for (int ih = 0; ih < imgInfoDataset1.resHeight; ih+=KERNEL) //percorre imagem pixel //coluna
                    {
                        int nextSubImg2 = 0;

                        if(!isBlackImage(subImgs1[nextSubImg1], KERNEL, KERNEL))
                        {
                            for (int iw2 = 0; iw2 < imgInfoDataset2.resWidth; iw2+=KERNEL)
                            {
                                for (int ih2 = 0; ih2 < imgInfoDataset2.resHeight; ih2+=KERNEL) //percorre imagem pixel //coluna
                                {   
                                    if(!isBlackImage(subImgs2[nextSubImg2]))
                                    {
                                        Mat subImg1Cv(KERNEL,KERNEL,CV_16UC1,subImgs1[nextSubImg1]);

        /*SIM CALCULATION*/             similarityResult = qualAssess.getMSSIM(subImg1Cv,subImgs2[nextSubImg2]);

                                        if(similarityResult.val[0] > simScores[id2][simScoreiT1])
                                            simScores[id2][simScoreiT1] = similarityResult.val[0];
                                    }
                                    nextSubImg2++;
                                }
                            }
                            simScoreiT1++;    
                        }
                        nextSubImg1++;
                    }
                }

                //printf("%d\n",simScoreiT1 );

                for( int ii = 0; ii < simScoreiT1; ii++)
                {
                    //printf("%f\n", simScores[id2][ii]);
                    simAverage[id2] = simAverage[id2] + simScores[id2][ii];
                }

                simAverage[id2] = simAverage[id2] / simScoreiT1;
            
                if (simAverage[id2] >= simMax)
                {
                    simMax = simAverage[id2];
                    simID  = id2;
                }

                //free(simAverage);
               // simAverage = NULL;
            }

            sliceAndDistance.sliceNumber = id;
            sliceAndDistance.distanceToOptimal = simID-id;
            sliceAndDistance.value=simMax;
            bestMatches.push_back(sliceAndDistance);
            
            if(options.verbose) printf("[%d]=[%d]-%f\n",id,simID,simMax );
            
            simMax = 0;
            simID = 0;
            free(simScores);
            simScores = NULL;
        }
    }
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
            //saveDataRaw( savedSubImg, 123, iw, ih, ih, KERNEL);
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

    d = dataset.changePlane('s');

    for( int i = 0; i < imgInfo.resDepth; i++ )
    {
        
        Mat slice(imgInfo.resHeight,imgInfo.resWidth,CV_16UC1,d[i]);
        cv_dataset.push_back(slice);
    }

    d = dataset.changePlane('c');

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

bool SimilarityAssessment::isBlackImage(imgT *image, int resW, int resH)
{

    bool isBlack = false; 
    int blackImage = 0;
    int blackLimit =  (resW*resH)*0.85;

    for (int i = 0; i < resW; i++)
        for (int j = 0; j < resH; j++)
            if(image[ijn(i,j,resW)] == 0)
                blackImage++;
    
    if(blackImage >= blackLimit) isBlack = true;
    else                         isBlack = false;

    return isBlack;
}


void SimilarityAssessment::saveDataCV( Mat image, int ttt, int id, int iw, int ih)
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

void SimilarityAssessment::saveDataRaw( imgT *imgRaw, int ttt, int id, int iw, int ih, int imgSize)
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