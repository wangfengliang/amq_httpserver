#include "lwan.h"

static lwan_http_status_t
root_handler(lwan_request_t *request,
            lwan_response_t *response, void *data)
{
    //static const char message[] = "root";
    //response->mime_type = "text/plain";
    //strbuf_set_static(response->buffer, message, sizeof(message) - 1);

    return HTTP_NOT_FOUND;
}
static lwan_http_status_t
hello_world2(lwan_request_t *request,
            lwan_response_t *response, void *data)
{
    static const char message[] = "Hello, World!22222222";
    response->mime_type = "text/plain";
    strbuf_set_static(response->buffer, message, sizeof(message) - 1);

    return HTTP_OK;
}

static lwan_http_status_t
hello_world(lwan_request_t *request,
            lwan_response_t *response, void *data)
{
    static const char message[] = "Hello, World!";

    response->mime_type = "text/plain";
    strbuf_set_static(response->buffer, message, sizeof(message) - 1);

    return HTTP_OK;
}

int
main(void)
{
/*
    const lwan_url_map_t default_map[] = {
        { .prefix = "/", .handler = root_handler },
        { .prefix = "/hello", .handler = hello_world },
        { .prefix = "/hello2", .handler = hello_world2 },
        { .prefix = NULL }
    };
*/
    const lwan_url_handler_t default_map[] = {
        { "/", root_handler, NULL },
        { "/hello", hello_world, NULL },
        { "/hello5", hello_world2, NULL},
        { NULL }
    };
    lwan_t l;
    lwan_config_t config ;
    config.listener = (char*)"127.0.0.1:8083";
    config.keep_alive_timeout = 15;
    config.quiet = false;
    config.reuse_port = false;
    config.expires = 12*3600;
    config.n_threads = 0;

    lwan_init_with_config(&l, &config);
    lwan_set_url_handlers(&l, default_map);
    lwan_main_loop(&l);
    lwan_shutdown(&l);

    return 0;
}

