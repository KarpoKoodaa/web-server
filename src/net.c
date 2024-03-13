#include <stdlib.h>
#include "nwheaders.h"
#include "net.h"

SOCKET create_socket (const char *host, const char *port) 
{
    struct addrinfo hints;
    memset (&hints, 0, sizeof(hints));

    // TODO: add support for IPv6
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo (host, port, &hints, &bind_address);

    SOCKET socket_listen;

    socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (socket_listen < 0) {
        fprintf(stderr, "socket() failed, (%d)\n", errno);
        exit(1);
    }

    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen) < 0) {
        fprintf(stderr, "bind() failed, (%d)\n", errno);
        exit(1);
    }
    freeaddrinfo(bind_address);

    if(listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed, (%d)\n", errno);
        exit(1);
    }

    return socket_listen;
}
