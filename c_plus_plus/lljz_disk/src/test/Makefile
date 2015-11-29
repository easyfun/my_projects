CC = gcc
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E
CXXFLAGS = -O3 -Wall -Wno-deprecated -fPIC
CXXLINK=$(CXX)
CXXCOMPILE=$(CXX) -c

TBSDK_LIBRARY_PATH=/home/trxh/lib
TBSDK_LIB=$(TBSDK_LIBRARY_PATH)/lib/libtbnet.a $(TBSDK_LIBRARY_PATH)/lib/libtbsys.a
TBSDK_INC = -I$(TBSDK_LIBRARY_PATH)/include

PROJECT_INC=-I../packets -I../utility
VPATH=../packets ../utility

EXE_OBJ=test.o
OUTPUT_PATH=.

test:test.o connection_manager_to_server.o
	$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) -lpthread -lrt

test.o:test.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(PROJECT_INC)

connection_manager_to_server.o:connection_manager_to_server.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/../test/$@ $^ $(TBSDK_INC) $(PROJECT_INC)

#test_client:$(EXE_OBJ)
#	cd $(OUTPUT_PATH) && $(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(TBSDK_LIB) -lpthread -lrt

#main.o:main.cpp
#	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(PROJECT_INC)

all:test
.PHONY:test
