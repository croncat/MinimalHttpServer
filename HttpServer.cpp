//SYS
#include <getopt.h>

//STD
#include <memory>
#include <string>
#include <fstream>
#include <iostream>

//LIBEVENT
#include <evhttp.h>

//CONST
const char* M404 = "<html><body>This page doesn't exist (404)</body></html>";

//DEFAULT OPTS
std::string ip = "127.0.0.1";
std::string path = "";
short int port = 8081;

void PrintUsage()
{
    using namespace std;

    cerr << "MHTTP: Minimal HTTP Server" << endl;
    cerr << "\t-i,--ip  \thost ip" << endl;
    cerr << "\t-p,--port\thost port" << endl;
    cerr << "\t-u,--path\tpath for the uri (without last '/')" << endl;
    cerr << "\t-h,--help\thelp" << endl;
}

void HttpReq(evhttp_request* req, void*)
{
    auto OutBuf = evhttp_request_get_output_buffer(req);
    if (!OutBuf) return;

    std::string uri = path + evhttp_request_get_uri(req);

    if (*(uri.end()-1) == '/')
        uri = uri + "index.html";
    else if (uri.rfind(".") == std::string::npos)
        uri = uri + ".html";

    //erase first '/'
    if (path.empty())
        uri.erase(0,1);

    std::string page;
    int code;

    //get page reading requested file
    std::ifstream infile(uri);
    if (infile.is_open()) {
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

int main(int argc, char* argv[])
{
    //OPTS
    int c;
    while (true) {
        static struct option long_options[] = {
            {"ip", required_argument, 0, 'i'},
            {"port", required_argument, 0, 'p'},
            {"path", required_argument, 0, 'u'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long(argc, argv, "i:p:u:h", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 'i':
                ip = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'u':
                path = optarg;
                break;
            case 'h':
                PrintUsage();
                exit(1);
            default:
                PrintUsage();
                exit(1);
        }
    }

    //MHTTP
    if (!event_init()) {
        std::cerr << "Error trying to initialize the event API" << std::endl;
        return -1;
    }

    std::unique_ptr<evhttp, decltype(&evhttp_free)> EventHttp(
        evhttp_start(ip.c_str(), port),
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