#ifndef UTILS
#define UTILS

#include <opencv2/gpu/gpu.hpp> 
#include <vector>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/linear_least_squares_fitting_3.h>

using namespace cv;

//typedef unsigned char imgT;
typedef unsigned short imgT;

//cgal typedefs
typedef double                      FT;
typedef CGAL::Simple_cartesian<FT>  K;
typedef K::Line_3                   Line;
typedef K::Plane_3                  Plane;
typedef K::Point_3                  Point;


typedef struct options
{
	bool verbose;
	bool logdata;
    int  subDivideFactor;
    bool gpuOptimized;
	char *logfilename;
}OPT;



static char usage[] =
"\n"
"  (C) 2014 3D Sub Image Matching\n"
"\n"
"  usage: ./3DSubImgMatch [options] <imagedata1> <width> <height> <stack init> <stack end> <imagedata2> <widht> <height> <stack init> <stack end>\n"
"\n"
"  Common options:\n"
"\n"
"      -g                 : Calculations on GPU \n"
"      -l <file>          : log data to a file\n"
"      -f <1,2,4,8...>    : Subdivide factor\n"
"      -v                 : verbose mode\n"
"\n";


#endif // UTILS

