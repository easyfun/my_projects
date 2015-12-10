/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   lljzfly <1060887552@qq.com>
 *
 */

#include "tbnet.h"
//#include "json/json.h"
#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "packet_factory.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "py_client_export.h"
#include "py_client.h"

namespace lljz {
namespace disk {

PyClient g_py_client;

bool Init() {
    g_py_client.start();
    return true;
}

void Stop() {
    g_py_client.stop();
}

void Wait() {
    g_py_client.wait();
}

uint64_t Connect(std::string spec, bool auto_conn) {
    return g_py_client.Connect(spec, auto_conn);
}

void Disconnect(uint64_t conn_id) {
    g_py_client.Disconnect(conn_id);
}

std::string Send(uint64_t conn_id, std::string req_str) {
    return g_py_client.Send(conn_id, req_str);
}

std::string SendOnce(std::string req) {
    return g_py_client.Send(req);
}

BOOST_PYTHON_MODULE(py_client) {
    boost::python::def("Init", Init);

    boost::python::def("Stop", Stop);

    boost::python::def("Wait", Wait);

    boost::python::def("Connect", Connect,
        (boost::python::arg("spec"),"auto_conn"));
    
    boost::python::def("Disconnect", Disconnect,
        boost::python::arg("conn_id"));
    
    boost::python::def("Send", Send,
        (boost::python::arg("conn_id"),"req_str"));

    boost::python::def("SendOnce", SendOnce,
        boost::python::arg("req"));

}

}
}


