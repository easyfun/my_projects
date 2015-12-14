#include "tbsys.h"
#include "tbnet.h"
#include "connection_manager_to_server.hpp"
#include "packet_factory.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace lljz::disk;

int main(int argc, char* argv[]) {
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);

    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);
    resp_json.Accept(resp_writer);
    std::string resp_data=resp_buffer.GetString();
    printf("1 data:%s\n", resp_data.c_str());

    Value& s=resp_doc["error_msg"];
    s.SetString(StringRef("hello world"));
    printf("2\n");
    return 1;
    resp_buffer.Clear();
//    resp_writer.Reset();
    resp_json.Accept(resp_writer);
    resp_data=resp_buffer.GetString();
    printf("1 data:%s\n", resp_data.c_str());
    return 0;
}

/*
int main(int argc, char* argv[]) {
    tbnet::Transport transport;
    tbnet::DefaultPacketStreamer packet_streamer;
    PacketFactory packet_factory;
    packet_streamer.setPacketFactory(&packet_factory);

    ConnectionManagerToServer cmts(&transport,&packet_streamer,NULL);
    if (!cmts.start("tcp:127.0.0.1:10000")) {
        printf("start failed\n");
        return 1;
    }
    cmts.wait();
    printf("exit\n");

    return 0;
}
*/