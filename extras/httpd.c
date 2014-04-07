/**
  cc -o httpd httpd.c -L/usr/local/lib/ -Wall -O2 -I/usr/local/include -ggdb -levent 
**/

#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <err.h>

#include <sys/queue.h>
#include <evhttp.h>
#define MAX_CONN 10240

struct evhttp_request *requests[MAX_CONN];
struct evbuffer *bufs[MAX_CONN];
int pos[MAX_CONN], num;

void on_close(struct evhttp_connection *req, void *arg) 
{
   int* p = (int*) arg;
   struct evhttp_request *p2 = requests[*p];
   struct evbuffer *p3 = bufs[*p];
   printf("%s closed.\n", p2->remote_host);
   requests[*p] = NULL;
   if (p3 != NULL) evbuffer_free(p3);
}

/* for comet recv */
void comet_handler(struct evhttp_request *req, void *arg) 
{
  int i, j;
  requests[num] = req;
  struct evbuffer *buf; 
  
  req->minor = 0; 
  evhttp_add_header(req->output_headers, "Content-Type", "text/plain");
  evhttp_send_reply_start(req, HTTP_OK, "OK");
 
  /* Set a callback for connection close. */
  evhttp_connection_set_closecb(req->evcon, on_close, &pos[num]);

  buf = evbuffer_new();
  bufs[num] = buf;

  for (j = 0; j < 10; j++)
    evbuffer_add_printf(buf, "Hi %s, this is a comet server based on libevent, visit http://hi.baidu.com/jabber for more information!\r\n", req->remote_host);
  evhttp_send_reply_chunk(req, buf);

    for (i = 0; i < num; i++) 
    {
       if (requests[i] == NULL) 
          continue;
       evbuffer_add_printf(bufs[i], "Oops, %s requests!\r\n", req->remote_host);
       evhttp_send_reply_chunk(requests[i], bufs[i]);
    }
    
    if (num++ == MAX_CONN) err(1, "exceed max conn, TODO.");
}

/* for comet send, GET /id */
void notify_handler(struct evhttp_request *req, void *arg) 
{
    struct evbuffer *buf;
    int nudge;
    
    buf = evbuffer_new();
    if (buf == NULL) 
      err(1, "failed to create response buffer");
    const char *uri = evhttp_request_uri(req);
    evbuffer_add_printf(buf, "Requested: %s, send a nudge to %s\n", uri, uri + 1);

    nudge = atoi(uri + 1);
    if (requests[nudge] != NULL) 
    {
        evbuffer_add_printf(bufs[nudge], "Oops, received a nudge from %s!\r\n", req->remote_host);
        evhttp_send_reply_chunk(requests[nudge], bufs[nudge]);
    }
    evhttp_send_reply(req, HTTP_OK, "OK", buf); // TODO: free buf
}

int main(int argc, char **argv) 
{
    struct evhttp *httpd;
    int i;
    for (i = 0; i < 1024; i++) 
    {
       requests[i] == NULL;
       bufs[i] = NULL;
       pos[i] = i;
    }
    num = 0;

    event_init();
    httpd = evhttp_start("0.0.0.0", 8080);

    /* Set a comet callback for requests to "/". */
    evhttp_set_cb(httpd, "/", comet_handler, NULL);

    /* Set a send callback for all other requests. */ 
    evhttp_set_gencb(httpd, notify_handler, NULL);

    event_dispatch();

    /* Not reached in this code as it is now. */
    evhttp_free(httpd);
    return 0;
}