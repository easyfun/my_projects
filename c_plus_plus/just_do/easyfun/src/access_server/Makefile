CC = gcc
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E
CXXFLAGS = -O3 -Wall -Wno-deprecated -fPIC -std=c++11
CXXLINK=$(CXX) -g
CXXCOMPILE=$(CXX) -c -g

INC_PATH=../../../lib/centos_6.6_x86_64_2.6.32/include
LIB_PATH=../../../lib/centos_6.6_x86_64_2.6.32/lib

TBSDK_LIB=$(LIB_PATH)/libtbnet.a $(LIB_PATH)/libtbsys.a
TBSDK_INC = -I$(INC_PATH)
RAPIDJSON_INC=-I$(INC_PATH)
RAPIDJSON_LIB=
JSON_INC=$(RAPIDJSON_INC)
JSON_LIB=$(RAPIDJSON_LIB)
REDIS_INC=-I$(INC_PATH)
REDIS_LIB=$(LIB_PATH)/libhiredis.a


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
