//STD
#include <memory>
#include <string>
#include <fstream>
#include <iostream>

//LIBEVENT
#include <evhttp.h>

const char* M404 = "<html><body>This page doesn't exist (404)</body></html>";

void HttpReq(evhttp_request* req, void*)
{
    auto OutBuf = evhttp_request_get_output_buffer(req);
    if (!OutBuf) return;

    std::string uri = evhttp_request_get_uri(req);

    if (*(uri.end()-1) == '/')
        uri = uri + "index.html";
    else if (uri.rfind(".") == std::string::npos)
        uri = uri + ".html";

    //erase first '/'
    uri.erase(0,1);

    std::string page;
    int code;

    //get page reading requested file
    std::ifstream infile(uri);
    if(infile.is_open()) {
        page.assign(
            (std::istreambuf_iterator<char>(infile)),
            std::istreambuf_iterator<char>()
        );
        code = HTTP_OK;
    } else {
        page = M404;
        code = HTTP_NOTFOUND;
    }

    evbuffer_add_printf(OutBuf, "%s", page.c_str());
    evhttp_send_reply(req, code, "", OutBuf);
}

int main()
{
    //MHTTP: Minimal single thread server.

    if (!event_init()) {
        std::cerr << "Error trying to initialize the event API" << std::endl;
        return -1;
    }

    std::unique_ptr<evhttp, decltype(&evhttp_free)> EventHttp(
        evhttp_start("127.0.0.1", 8081),
        &evhttp_free
    );

    if (!EventHttp) {
        std::cerr << "Error starting HTTP server" << std::endl;
        return -1;
    }

    evhttp_set_gencb(EventHttp.get(), HttpReq, nullptr);

    if (event_dispatch() == -1) {
        std::cerr << "HTTP server loop failed" << std::endl;
        return -1;
    }

    return 0;
}