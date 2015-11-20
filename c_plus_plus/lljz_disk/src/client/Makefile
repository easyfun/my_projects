CC = gcc
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E
CXXFLAGS = -O3 -Wall -Wno-deprecated -fPIC
CXXLINK=$(CXX)
CXXCOMPILE=$(CXX) -c

TBSDK_LIBRARY_PATH=/home/lljz/lib
TBSDK_LIB=$(TBSDK_LIBRARY_PATH)/lib/libtbnet.a $(TBSDK_LIBRARY_PATH)/lib/libtbsys.a
TBSDK_INC = -I$(TBSDK_LIBRARY_PATH)/include

PROJECT_INC=-I../packets
VPATH=../packets

EXE_OBJ=client.o
OUTPUT_PATH=../../output/test_client

test_client:client.cpp
	$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) -lpthread -lrt

#test_client:$(EXE_OBJ)
#	cd $(OUTPUT_PATH) && $(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(TBSDK_LIB) -lpthread -lrt

#main.o:main.cpp
#	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(PROJECT_INC)

all:test_client
