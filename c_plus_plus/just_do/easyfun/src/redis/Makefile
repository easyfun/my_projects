CC = gcc
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E
CXXFLAGS = -O3 -Wall -Wno-deprecated -fPIC
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

PROJECT_INC=-I../packets -I../protocol -I../utility
VPATH=../packets ../utility

OUTPUT_PATH=./

all:test.exe
.PHONY:test.exe

test.exe:test_redis.o redis_client_manager.o redis_client.o
	$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
	$(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) \
	$(JSON_INC) $(JSON_LIB) $(PYTHON_BOOST_INC) \
	$(REDIS_INC) $(REDIS_LIB) \
	$(PYTHON_BOOST_LIB) -lpthread -lrt

test_redis.o:test_redis.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
	$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) $(PYTHON_BOOST_INC) \
	$(REDIS_INC)

redis_client.o:redis_client.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
	$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) $(PYTHON_BOOST_INC) \
	$(REDIS_INC)

redis_client_manager.o:redis_client_manager.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
	$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) $(PYTHON_BOOST_INC) \
	$(REDIS_INC)

