#include <memory>
#include <evhttp.h>

int main()
{
    if (!event_init()) return -1;

    std::unique_ptr<evhttp, decltype(&evhttp_free)> EventHttp(evhttp_start("127.0.0.1", 8081), &evhttp_free);
    if (!EventHttp) return -1;

    auto GenericReq = [](evhttp_request *req, void *obj)
    {
        auto OutBuf = evhttp_request_get_output_buffer(req);
        if (!OutBuf) return;

        evbuffer_add_printf(OutBuf, "<html><body><h1>Hello World!</h1></body></html>");
        evhttp_send_reply(req, HTTP_OK, "", OutBuf);
    };

    evhttp_set_gencb(EventHttp.get(), GenericReq, nullptr);

    if (event_dispatch() == -1) return -1;

    return 0;
}