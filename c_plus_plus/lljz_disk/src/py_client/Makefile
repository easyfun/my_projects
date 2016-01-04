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

PROJECT_INC=-I../packets -I../protocol -I../utility
VPATH=../packets ../utility

#OUTPUT_PATH=../../output/py_client
OUTPUT_PATH=./

all:py_client
.PHONY:py_client install

py_client:py_client.so

install:
	#rm $(OUTPUT_PATH)/py_test_client.py
	#rm $(OUTPUT_PATH)/py_test_file_server.py
	cp py_test_client.py $(OUTPUT_PATH)
	cp py_client.ini $(OUTPUT_PATH)
	cp py_test_file_server.py $(OUTPUT_PATH)
	cp modify_property_req.py $(OUTPUT_PATH)
	cp py_upload_file.py $(OUTPUT_PATH)
	cp py_download_file.py $(OUTPUT_PATH)

py_client.so:py_client.o py_client_export.o
	cd $(OUTPUT_PATH) && \
	$(CXXLINK) -shared $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
	$(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) \
	$(JSON_INC) $(JSON_LIB) $(PYTHON_BOOST_INC) \
	$(PYTHON_BOOST_LIB) -lpthread -lrt

py_client.o:py_client.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
	$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) $(PYTHON_BOOST_INC)

py_client_export.o:py_client_export.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
	$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) $(PYTHON_BOOST_INC)
