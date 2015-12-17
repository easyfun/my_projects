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
#PYTHON_BOOST_INC=-I/usr/local/include -I/usr/include/python2.7
#PYTHON_BOOST_LIB=-L/usr/lib/python2.7 -L/user/local/lib -lboost_python -lpython2.7


TBSDK_LIBRARY_PATH=/home/lljz/lib
TBSDK_LIB=$(TBSDK_LIBRARY_PATH)/lib/libtbnet.a $(TBSDK_LIBRARY_PATH)/lib/libtbsys.a
TBSDK_INC = -I$(TBSDK_LIBRARY_PATH)/include
JSONCPP_INC=-I/home/lljz/projects/jsoncpp-master/include
JSONCPP_LIB=/home/lljz/projects/jsoncpp-master/libs/linux-gcc-4.9.2/libjson_linux-gcc-4.9.2_libmt.a
RAPIDJSON_INC=-I/home/lljz/my_projects/rapidjson/include
RAPIDJSON_LIB=
JSON_INC=$(RAPIDJSON_INC)
JSON_LIB=$(RAPIDJSON_LIB)
PYTHON_BOOST_INC=-I/usr/local/include -I/usr/include/python2.6
PYTHON_BOOST_LIB=-L/usr/lib/python2.6 -L/user/local/lib -lboost_python -lpython2.6
REDIS_INC=-I/home/lljz/lib/hiredis/include
REDIS_LIB=/home/lljz/lib/hiredis/lib/libhiredis.a

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

