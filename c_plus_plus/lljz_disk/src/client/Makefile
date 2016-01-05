CC = gcc
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E
CXXFLAGS = -O3 -Wall -Wno-deprecated -fPIC
CXXLINK=$(CXX) -g
CXXCOMPILE=$(CXX) -c -g


#TBSDK_LIBRARY_PATH=/home/trxh/lib
#TBSDK_LIB=$(TBSDK_LIBRARY_PATH)/lib/libtbnet.a $(TBSDK_LIBRARY_PATH)/lib/libtbsys.a
#TBSDK_INC = -I$(TBSDK_LIBRARY_PATH)/include
#JSONCPP_INC=-I/home/trxh/my_projects/jsoncpp/include
#JSONCPP_LIB=/home/trxh/my_projects/jsoncpp/libs/linux-gcc-4.8/libjson_linux-gcc-4.8_libmt.a
#RAPIDJSON_INC=-I/home/trxh/my_projects/rapidjson/include
#RAPIDJSON_LIB=
#JSON_INC=$(RAPIDJSON_INC)
#JSON_LIB=$(RAPIDJSON_LIB)

TBSDK_LIBRARY_PATH=/home/lljz/lib
TBSDK_LIB=$(TBSDK_LIBRARY_PATH)/lib/libtbnet.a $(TBSDK_LIBRARY_PATH)/lib/libtbsys.a
TBSDK_INC = -I$(TBSDK_LIBRARY_PATH)/include
JSONCPP_INC=-I/home/lljz/projects/jsoncpp-master/include
JSONCPP_LIB=/home/lljz/projects/jsoncpp-master/libs/linux-gcc-4.9.2/libjson_linux-gcc-4.9.2_libmt.a
RAPIDJSON_INC=-I/home/lljz/my_projects/rapidjson/include
RAPIDJSON_LIB=
JSON_INC=$(RAPIDJSON_INC)
JSON_LIB=$(RAPIDJSON_LIB)

PROJECT_INC=-I../packets -I../protocol -I../utility
VPATH=../packets ../utility

EXE_OBJ=client.o
#OUTPUT_PATH=../../output/client
OUTPUT_PATH=./

client:client.cpp
	$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
	$(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) \
	$(JSON_INC) $(JSON_LIB) -lpthread -lrt

client_pending:client_pending.cpp	
	$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
	$(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) \
	$(JSON_INC) $(JSON_LIB) -lpthread -lrt

client_one:client_one.cpp	
	$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
	$(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) \
	$(JSON_INC) $(JSON_LIB) -lpthread -lrt

client_test_echo:client_test_echo.cpp
	$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
	$(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) \
	$(JSON_INC) $(JSON_LIB) -lpthread -lrt

client_test_echo_register:client_test_echo_register.cpp
	$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
	$(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) \
	$(JSON_INC) $(JSON_LIB) -lpthread -lrt

lock_test:lock_test.cpp
	$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
	$(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) \
	$(JSON_INC) $(JSON_LIB) -lpthread -lrt

all:client
.PHONY:client
