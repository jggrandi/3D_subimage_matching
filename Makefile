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

LCGAL = -lCGAL

I_ARRAY = -I../../my_libs/Array
I_HANDLE3DDATASETS = -I../../my_libs/Handle3DDataset/src 
I_QUALITYASSESSMENT = -I../../my_libs/QualityAssessment/src 


$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(CDEBUG) $(WARNINGOFF) --std=c++11 $(I_ARRAY) $(I_HANDLE3DDATASETS) $(I_QUALITYASSESSMENT) -o $(PROG) $(SRC) $(OPENMP) $(OPENCV) $(LCGAL)


clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

