#ifndef UTILS
#define UTILS

#include <opencv2/gpu/gpu.hpp> 

#include <CGAL/Simple_cartesian.h>
#include <CGAL/linear_least_squares_fitting_3.h>

#include <vector>

using namespace cv;

//typedef unsigned char imgT;
typedef unsigned short imgT;

//cgal typedefs
typedef double                      FT;
typedef CGAL::Simple_cartesian<FT>  K;
typedef K::Line_3                   Line;
typedef K::Plane_3                  Plane;
typedef K::Point_3                  Point3;



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
    imgT    *bmImg;
}BM;

typedef struct  functionalParameters
{
    int FACTOR;
    int KERNEL;
    int OFFSET;
    int PLANES;
    int SAMPLING;
    int INITIAL_SLICE;
}FP;

inline std::ostream &operator<<( std::ostream &out, const BM bm ) 
{
    return out << "[" << bm.bmCoord.z << ","<< bm.bmCoord.x << "," << bm.bmCoord.y << "] (" << bm.bmSimValue << "; " << bm.bmPlane << ")";
} 


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

