CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O3
WARNINGOFF = -w
PROG = ../bin/3DSubImgMatch

SRC = main.cpp handledata.cpp similarityassessment.cpp logdata.cpp

CTHREAD = -std=c++11 -pthread
OPENMP = -fopenmp -lgomp

OPENGL = -lglut
OPENCV = -L /opt/cuda/lib64 `pkg-config opencv --libs --cflags`

I_ARRAY = -I../../my_libs/Array
L_ARRAY = -L../../my_libs/Array

I_HANDLE3DDATASETS = -I../../my_libs/Handle3DDataset/src 
L_HANDLE3DDATASETS = -L../../my_libs/Handle3DDataset/bin

I_QUALITYASSESSMENT = -I../../my_libs/QualityAssessment/src 
L_QUALITYASSESSMENT = -L../../my_libs/QualityAssessment/bin




$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(CDEBUG) --std=c++11 $(I_ARRAY) $(I_HANDLE3DDATASETS) $(I_QUALITYASSESSMENT) $(L_ARRAY) $(L_HANDLE3DDATASETS) $(L_QUALITYASSESSMENT)  -o $(PROG) $(SRC) $(OPENMP) $(OPENGL) $(OPENCV) 


clean:
	rm -f $(PROG)


all:
	make clean
	make $(PROG)

