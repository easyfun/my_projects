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

PROJECT_INC=-I../packets
VPATH=../packets

EXE_OBJ=client.o
OUTPUT_PATH=./

BUILD=load_connection_manager.o

load_connection_manager.o:load_connection_manager.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(PROJECT_INC)

connection_manager_to_server.o:connection_manager_to_server.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(PROJECT_INC)

all:$(BUILD)
