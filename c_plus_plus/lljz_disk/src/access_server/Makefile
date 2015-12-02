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

PROJECT_INC=-I../packets -I../protocol -I../utility
VPATH=../packets

EXE_OBJ=config_server.o
OUTPUT_PATH=./

#exe:$(EXE_OBJ)
#	cd $(OUTPUT_PATH) && $(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/config_server $^ $(TBSDK_INC) $(TBSDK_LIB) -lpthread -lrt

#main.o:main.cpp
#	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(PROJECT_INC)

#access_server.o:access_server.cpp access_server.hpp base_packet.hpp packet_factory.hpp request_packet.hpp response_packet.hpp
access_server.o:access_server.cpp
	echo $(CXXCOMPILE)
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ $(TBSDK_INC) $(PROJECT_INC)

all:
