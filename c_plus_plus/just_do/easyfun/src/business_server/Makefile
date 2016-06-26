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


PROJECT_INC=-I../packets -I../public_handler -I./handler -I../redis -I../public
VPATH=../packets ../public_handler ./handler ../redis ../public

#OUTPUT_PATH=../../output/business_server
OUTPUT_PATH=./
all:business_server
.PHONY:business_server

business_server:main.o server_entry.o manager_client.o global.o \
connection_manager_to_access.o \
handler_router.o public_echo_test_req_handler.o redis_client.o redis_client_manager.o \
handler.o register_account_req.o set_account_info_req.o modify_login_password_req.o \
login_req.o logout_req.o
	cd $(OUTPUT_PATH) && \
		$(CXXLINK) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_LIB) $(JSON_LIB) $(REDIS_LIB) -lpthread -lrt

main.o:main.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) $(REDIS_INC)

server_entry.o:server_entry.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) \
		$(JSON_INC) $(REDIS_INC)

connection_manager_to_access.o:connection_manager_to_access.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) \
		$(JSON_INC) $(REDIS_INC)

manager_client.o:manager_client.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) \
		$(JSON_INC) $(REDIS_INC)

global.o:global.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC)

handler_router.o:handler_router.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC)

public_echo_test_req_handler.o:public_echo_test_req_handler.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ \
		$^ $(TBSDK_INC) $(PROJECT_INC) $(JSON_INC)

redis_client_manager.o:redis_client_manager.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) \
		$(REDIS_INC)

redis_client.o:redis_client.cpp
	$(CXXCOMPILE) $(CXXFLAGS) -o $(OUTPUT_PATH)/$@ $^ \
		$(TBSDK_INC) $(PROJECT_INC) $(JSON_INC) \
		$(REDIS_INC)

handler.o:handler.cpp
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
