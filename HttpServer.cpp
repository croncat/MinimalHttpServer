//STD
#include <memory>
#include <string>
#include <fstream>

//LIBEVENT
#include <evhttp.h>

const char* M404 = "<html><body>This page doesn't exist (404)</body></html>";

int main()
{
    if (!event_init()) return -1;

    std::unique_ptr<evhttp, decltype(&evhttp_free)> EventHttp(
        evhttp_start("127.0.0.1", 8081),
        &evhttp_free
    );

    if (!EventHttp) return -1;

    auto GenericReq = [](evhttp_request* req, void*)
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
    };

    evhttp_set_gencb(EventHttp.get(), GenericReq, nullptr);

    if (event_dispatch() == -1) return -1;

    return 0;
}