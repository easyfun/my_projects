CC = gcc
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E
CXXFLAGS = -g -O3 -Wall -Wno-deprecated -fPIC
CXXLINK=$(CXX)
CXXCOMPILE=$(CXX) -c

TBSDK_LIBRARY_PATH=/home/trxh/lib
TBSDK_LIB=$(TBSDK_LIBRARY_PATH)/lib/libtbnet.a $(TBSDK_LIBRARY_PATH)/lib/libtbsys.a
TBSDK_INC = -I$(TBSDK_LIBRARY_PATH)/include
JSONCPP_INC=-I/home/trxh/my_projects/jsoncpp/include
JSONCPP_LIB=/home/trxh/my_projects/jsoncpp/libs/linux-gcc-4.8/libjson_linux-gcc-4.8_libmt.a
RAPIDJSON_INC=-I/home/trxh/my_projects/rapidjson/include
RAPIDJSON_LIB=
JSON_INC=$(RAPIDJSON_INC)
JSON_LIB=$(RAPIDJSON_LIB)


#TBSDK_LIBRARY_PATH=/home/lljz/lib
#TBSDK_LIB=$(TBSDK_LIBRARY_PATH)/lib/libtbnet.a $(TBSDK_LIBRARY_PATH)/lib/libtbsys.a
#TBSDK_INC = -I$(TBSDK_LIBRARY_PATH)/include
#JSONCPP_INC=-I/home/lljz/projects/jsoncpp-master/include
#JSONCPP_LIB=/home/lljz/projects/jsoncpp-master/libs/linux-gcc-4.9.2/libjson_linux-gcc-4.9.2_libmt.a
#RAPIDJSON_INC=-I/home/lljz/my_projects/rapidjson/include
#RAPIDJSON_LIB=
#JSON_INC=$(RAPIDJSON_INC)
#JSON_LIB=$(RAPIDJSON_LIB)

PROJECT_INC=-I../packets -I../utility
VPATH=../packets ../utility

EXE_OBJ=test.o
OUTPUT_PATH=.

test_json:test.cpp
	$(CXXLINK) $(CXXFLAGS) -o $@ $^ $(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) $(JSON_INC) $(JSON_LIB) -lpthread -lrt

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
