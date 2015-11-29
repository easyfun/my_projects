#include "tbsys.h"
#include "tbnet.h"
#include "connection_manager_to_server.hpp"
#include "packet_factory.hpp"

using namespace lljz::disk;

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