/*
 * Basic TCP Server
 * Based on Lewis Van Winkle's "Hands-On Network Programming with C"
 *
 *
 */

#include "nwheaders.h"
#include "net.h"
#include "client.h"
#include "error_msg.h"
#include "mime.h"
#include "server.h"
#include <ctype.h>
#include <time.h>
#include <string.h>

#define BUF_SIZE 1024
// #define MAX_REQUEST_SIZE 2047 


void serve_resource(struct client_info **client_list, struct client_info *client, const char *path)
{
    printf("server: serve resource %s %s\n", get_client_address(client), path);

    if (strcmp(path, "/") == 0) path = "/index.html";

    if (strlen(path) > 100) {
        send_404(client_list, client);
        return;
    }

    if (strstr(path, "..")) {
        send_404(client_list, client);
        return;
    }

    char full_path[128];
    sprintf(full_path, "public%s", path);

    FILE *fp = fopen(full_path, "rb");

    if (!fp) {
        send_404(client_list, client);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    long cl = ftell(fp);
    rewind(fp);

    const char *content_type = get_mime_type(full_path);

#define BSIZE 1024
    char buffer[BSIZE];

    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Connection: close\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Length: %lu\r\n", cl);
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "Content-Type: %s\r\n", content_type);
    send(client->socket, buffer, strlen(buffer), 0);

    sprintf(buffer, "\r\n");
    send(client->socket, buffer, strlen(buffer), 0);

    size_t r = fread(buffer, 1, BSIZE, fp);
    while(r) {
        send(client->socket, buffer, r, 0);
        r = fread(buffer, 1, BSIZE, fp);
    }

    fclose(fp);
    drop_client(client_list, client);
}


int main() 
{


    SOCKET server = create_socket(0,"8080");

    struct client_info *client_list = 0;

    while(1) {
        fd_set reads;
        reads = wait_on_clients(&client_list, server);

        if (FD_ISSET(server, &reads)) {
            struct client_info *client = get_client(&client_list, -1);

            client->socket = accept(server, 
                    (struct sockaddr*) &(client->address),
                    &(client->addr_length));

            if (!ISVALIDSOCKET(client->socket)) {
                fprintf(stderr, "socket() failed. (%d)", errno);
                return 1;
            }

            printf("server: new connection from %s.\n", get_client_address(client));
        }

        struct client_info *client = client_list;
        while(client) {
            struct client_info *next = client->next;

            if (FD_ISSET(client->socket, &reads)) {

                if (MAX_REQUEST_SIZE == client->received) {
                    send_400(&client_list, client);
                    client = next;
                    continue;
                }

                int r = recv(client->socket, 
                        client->request + client->received, 
                        (size_t)(MAX_REQUEST_SIZE - client->received), 0);

                if (r < 1) {
                    printf("Unexpected disconnect from %s.\n", get_client_address(client));
                    drop_client(&client_list, client);

                } else {
                    client->received += r;
                    client->request[client->received] = 0;

                    char *q = strstr(client->request, "\r\n\r\n");
                    if(q) {
                        *q = 0;

                        if (strncmp("GET /", client->request, 5)) {
                            send_400(&client_list, client);
                        } else {
                            char *path = client->request + 4;
                            char *end_path = strstr(path, " ");
                            if(!end_path) {
                                send_400(&client_list, client);
                            } else {
                                *end_path = 0;
                                serve_resource(&client_list, client, path);
                            }
                        }
                    } // if(q)
                }
            }
            client = next;
        }

    } //while(1)

    printf("Closing socket...\n");
    CLOSESOCKET(server);

    printf("Finished.\n");



    return 0;

}
