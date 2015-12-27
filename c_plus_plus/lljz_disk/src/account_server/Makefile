CC = gcc
CPP = gcc -E
CXX = g++
CXXCPP = g++ -E
CXXFLAGS = -O3 -Wall -Wno-deprecated -fPIC
CXXLINK=$(CXX) -g
CXXCOMPILE=$(CXX) -c -g

TBSDK_LIBRARY_PATH=/home/trxh/lib
TBSDK_LIB=$(TBSDK_LIBRARY_PATH)/lib/libtbnet.a $(TBSDK_LIBRARY_PATH)/lib/libtbsys.a
TBSDK_INC = -I$(TBSDK_LIBRARY_PATH)/include
JSONCPP_INC=-I/home/trxh/my_projects/jsoncpp/include
JSONCPP_LIB=/home/trxh/my_projects/jsoncpp/libs/linux-gcc-4.8/libjson_linux-gcc-4.8_libmt.a
RAPIDJSON_INC=-I/home/trxh/my_projects/rapidjson/include
RAPIDJSON_LIB=
JSON_INC=$(RAPIDJSON_INC)
JSON_LIB=$(RAPIDJSON_LIB)
REDIS_INC=-I/usr/local/include/hiredis
REDIS_LIB=/usr/local/lib/libhiredis.a


#TBSDK_LIBRARY_PATH=/home/lljz/lib
#TBSDK_LIB=$(TBSDK_LIBRARY_PATH)/lib/libtbnet.a $(TBSDK_LIBRARY_PATH)/lib/libtbsys.a
#TBSDK_INC = -I$(TBSDK_LIBRARY_PATH)/include
#JSONCPP_INC=-I/home/lljz/projects/jsoncpp-master/include
#JSONCPP_LIB=/home/lljz/projects/jsoncpp-master/libs/linux-gcc-4.9.2/libjson_linux-gcc-4.9.2_libmt.a
#RAPIDJSON_INC=-I/home/lljz/my_projects/rapidjson/include
#RAPIDJSON_LIB=
#JSON_INC=$(RAPIDJSON_INC)
#JSON_LIB=$(RAPIDJSON_LIB)
#REDIS_INC=-I/home/lljz/lib/hiredis/include
#REDIS_LIB=/home/lljz/lib/hiredis/lib/libhiredis.a

PROJECT_INC=-I../packets -I../protocol -I../utility -I../public_handler -I./account_server_handler -I../redis
VPATH=../packets ../utility ../public_handler ./account_server_handler ../redis

#OUTPUT_PATH=../../output/account_server
OUTPUT_PATH=./
all:account_server
.PHONY:account_server

account_server:main.o account_server.o global.o connection_manager_from_client.o \
connection_manager_to_server.o load_connection_manager.o packet_handler_to_server.o \
handler_router.o public_echo_test_req_handler.o redis_client.o redis_client_manager.o \
register_account_req.o set_account_info_req.o modify_login_password_req.o \
login_req.o logout_req.o account_server_handler.o
	cd $(OUTPUT_PATH) && \
		$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(TBSDK_LIB) \
		$(JSON_INC) $(JSON_LIB) \
		$(REDIS_INC) $(REDIS_LIB) -lpthread -lrt

main.o:main.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(TBSDK_LIB) $(PROJECT_INC) -lpthread -lrt

#account_server.o:account_server.cpp account_server.hpp base_packet.hpp packet_factory.hpp request_packet.hpp response_packet.hpp
account_server.o:account_server.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) \
		$(JSON_INC) $(REDIS_INC)

global.o:global.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC)

connection_manager_to_server.o:connection_manager_to_server.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC)

connection_manager_from_client.o:connection_manager_from_client.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC)

load_connection_manager.o:load_connection_manager.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC)

packet_handler_to_server.o:packet_handler_to_server.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC)

handler_router.o:handler_router.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC)

public_echo_test_req_handler.o:public_echo_test_req_handler.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC)

redis_client_manager.o:redis_client_manager.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) \
		$(REDIS_INC)

redis_client.o:redis_client.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) \
		$(REDIS_INC)

register_account_req.o:register_account_req.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) \
		$(REDIS_INC)

set_account_info_req.o:set_account_info_req.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) \
		$(REDIS_INC)

modify_login_password_req.o:modify_login_password_req.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) \
		$(REDIS_INC)

login_req.o:login_req.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) \
		$(REDIS_INC)

logout_req.o:logout_req.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) \
		$(REDIS_INC)

account_server_handler.o:account_server_handler.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) \
		$(REDIS_INC)
