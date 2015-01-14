CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O3
WARNINGOFF = -w
PROG = ../bin/3DSubImgMatch

SRC = main.cpp handledata.cpp similarityassessment.cpp logdata.cpp planefitting.cpp

CTHREAD = -std=c++11 -pthread
OPENMP = -fopenmp -lgomp

OPENGL = -lglut
OPENCV = -L /opt/cuda/lib64 `pkg-config opencv --libs --cflags`

ICGAL = -I/usr/include/CGAL
LCGAL = -lCGAL

I_ARRAY = -I../../my_libs/Array
L_ARRAY = -L../../my_libs/Array

I_HANDLE3DDATASETS = -I../../my_libs/Handle3DDataset/src 
L_HANDLE3DDATASETS = -L../../my_libs/Handle3DDataset/bin

I_QUALITYASSESSMENT = -I../../my_libs/QualityAssessment/src 
L_QUALITYASSESSMENT = -L../../my_libs/QualityAssessment/bin





$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(CDEBUG) $(WARNINGOFF) --std=c++11 $(I_ARRAY) $(I_HANDLE3DDATASETS) $(I_QUALITYASSESSMENT) $(LCGAL) $(L_ARRAY) $(L_HANDLE3DDATASETS) $(L_QUALITYASSESSMENT) $(LCGAL)  -o $(PROG) $(SRC) $(OPENMP) $(OPENGL) $(OPENCV) 


clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

