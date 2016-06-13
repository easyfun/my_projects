CC = gcc
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E
CXXFLAGS = -O3 -Wall -Wno-deprecated -fPIC -std=c++11
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

PROJECT_INC=-I../packets -I../public
VPATH=../packets ../public

#OUTPUT_PATH=../../output/access_server
OUTPUT_PATH=./

access_server:main.o server_entry.o global.o manager_client.o manager_server.o load_connection.o
	cd $(OUTPUT_PATH) && \
	$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/access_server \
		$^ $(TBSDK_LIB) $(JSON_LIB) -lpthread -lrt

main.o:main.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) \
		$(JSON_INC) -lpthread -lrt

server_entry.o:server_entry.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC)

global.o:global.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC)

manager_client.o:manager_client.cpp		
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC)

manager_server.o:manager_server.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC)

load_connection.o:load_connection.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC)

all:access_server
.PHONY:access_server
