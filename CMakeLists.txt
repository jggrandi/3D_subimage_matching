cmake_minimum_required(VERSION 2.8)
project(3DSubImageMatching)


find_package(OpenMP)
find_package(Threads)
find_package(OpenCV REQUIRED )
find_package(CGAL REQUIRED)
find_package(ITK REQUIRED)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
endif()

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#set(CMAKE_CXX_FLAGS "-g -Wall -w -std=c++11 -fpermissive")

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-w -std=c++11 -fpermissive")

set(ARRAY ../../my_libs/Array)
set(QASSESSMENT ../../my_libs/QualityAssessment/src)
set(HANDLE3DDATA ../../my_libs/Handle3DDataset/src)

include_directories(${ARRAY} ${QASSESSMENT} ${HANDLE3DDATA})

set(HEADER_FILES ${ARRAY}/vector3f.h ${QASSESSMENT}/qualityassessment.h ${HANDLE3DDATA}/handle3ddataset.h)

add_executable (3DSubImgMatch MACOSX_BUNDLE main.cpp handledata.cpp similarityassessment.cpp logdata.cpp planefitting.cpp ${HEADER_FILES})

target_link_libraries (3DSubImgMatch ITKReview ${ITK_LIBRARIES} ${OpenCV_LIBS} ${CGAL_LIBRARY}   )











