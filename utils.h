#ifndef UTILS
#define UTILS

#include <opencv2/gpu/gpu.hpp> 
#include <vector>


using namespace cv;

//typedef unsigned char imgT;
typedef unsigned short imgT;


typedef struct options
{
	bool verbose;
	bool logdata;
    bool gpuOptimized;
	char *logfilename;
}OPT;


struct sliceRank
{
    int sliceNumber;
    int distanceToOptimal;
    float value;
};


static char usage[] =
"\n"
"  (C) 2014 3D Sub Image Matching\n"
"\n"
"  usage: ./3DSubImgMatch [options] <imagedata1> <width> <height> <stack init> <stack end> <imagedata2> <widht> <height> <stack init> <stack end>\n"
"\n"
"  Common options:\n"
"\n"
"      -g           : Calculations on GPU \n"
"      -l <file>    : log data to a file\n"
"      -v           : verbose mode\n"
"\n";


#endif // UTILS

