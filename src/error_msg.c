#include <stdlib.h>
#include <string.h>

#include "nwheaders.h"
#include "server.h"
#include "error_msg.h"
#include "net.h"
#include "client.h"


void send_400(struct client_info **client_list, struct client_info *client) 
{
    const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
                        "Connection: close\r\n"
                        "Content-Length: 11\r\n\r\nBad Request";
    send(client->socket, c400, strlen(c400), 0);
    drop_client(client_list, client);

}

void send_404(struct client_info **client_list, struct client_info *client) 
{
    const char *c404 = "HTTP/1.1 404 Not Found\r\n"
                        "Connection: close\r\n"
                        "Content-Length: 9\r\n\r\nNot Found";
    send(client->socket, c404, strlen(c404), 0);
    drop_client(client_list, client);

}
