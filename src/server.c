/*
 * Basic TCP Server
 * Based on Lewis Van Winkle's "Hands-On Network Programming with C"
 *
 *
 */

#include "nwheaders.h"
#include "net.h"
#include <ctype.h>
#include <time.h>

#define BUF_SIZE 1024


int main() 
{


    SOCKET server = create_socket(0,"8080");

    fd_set master;
    FD_ZERO(&master);
    FD_SET(server, &master);
    SOCKET max_socket = server;

    printf("Waiting for connections...\n");

    while(1) {
        fd_set reads;
        reads = master;
        if(select(max_socket+1, &reads, 0, 0,0) < 0) {
            fprintf(stderr, "select() failed. %d\n", GETSOCKETERRNO());
            return 1;
        }

        SOCKET i;
        for (i = 1; i <= max_socket; ++i) {
            if(FD_ISSET(i, &reads)) {
                // Handle socket
                if (i == server) {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(server,(struct sockaddr*) &client_address, &client_len);
                    if(!ISVALIDSOCKET(socket_client)) {
                        fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    if(socket_client > max_socket) {
                        max_socket = socket_client;
                    }

                    char address_buffer[100];
                    getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
                    printf("New connection from %s\n", address_buffer);
                }
                else {
                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    printf("Bytes received: %d\n", bytes_received);
                    printf("%.*s", bytes_received, read);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        continue;
                    }
                    printf("Sending response...\n");
                    const char *response = 
                        "HTTP/1.1 200 OK\r\n"
                        "Connection: close\r\n"
                        "Content-Type: text/plain\r\n\r\n"
                        "Local time is: ";
                    int bytes_sent = send(i, response, strlen(response), 0);
                    printf("Send %d of %d bytes.\n", bytes_sent, (int)strlen(response));

                    // For testing purposes local time is send back to client
                    time_t timer;
                    time(&timer);
                    char *time_msg = ctime(&timer);
                    bytes_sent = send(i, time_msg, strlen(time_msg), 0);
                    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));
                
                }
            }
        }
    }
    printf("Closing socket...\n");
    CLOSESOCKET(server);

    printf("Finished.\n");



    return 0;

}
