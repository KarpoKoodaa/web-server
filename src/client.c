#include "nwheaders.h"
#include "client.h"

#define MAX_REQUEST_SIZE 2047

// Is this here or in header file?
struct client_info {
    socklen_t addr_length;
    struct sockaddr_storage address;
    char address_buffer[128];
    SOCKET socket;
    char request[MAX_REQUEST_SIZE + 1];
    int received;
    struct client_info *next;
};

// get client
struct client_info *get_client(struct client_info **client_list, SOCKET s)
{
    struct client_info *ci = *client_list;

    while (ci) {
        if (ci->socket == s) {
            // Socket found 
            break;
        }
        ci = ci->next;
    }

    if (ci) {
        // Return socket
        return ci;
    }

    // create a new client info structure
    struct client_info *n = 
        (struct client_info*) calloc(1, sizeof(struct client_info));

    if (!n) {
        fprintf(stderr, "Out of memory!\n");
        exit()
            exit(1);
    }

    n->addr_length = sizeof(n->address);
    n->next = *client_list;
    *client_list = n;

    return n;
}


// drop_client
void drop_client(struct client_info **client_list, struct client_info *client) 
{
    close(client->socket);

    struct client_info **p = client_list;

    while (*p) {
        if (*p == client) {
            *p = client->next;
            free(client);
            return;
        }

        p = &(*p)->next;
    }

    fprintf(stderr, "client not found.\n");
    exit(1);
}

// get_client_address
const char *get_client_address(struct client_info *ci)
{
    getnameinfo((struct sockaddr*)ci->address,
            ci->addr_length,
            ci->address_buffer, sizeof(address_buffer), 0, 0, 
            NI_NUMERICHOST);

    return ci->address_buffer;

}


// wait_on_clients
fd_set wait_on_clients (struct client_info **client_list, SOCKET server) 
{
    fd_set reads;
    FD_ZERO (&reads);
    FD_SET(server, &reads);
    SOCKET max_socket = server;

    struct client_info *ci = *client_list;

    while(ci) {
        FD_SET(ci->socket, &reads);
        if(ci->socket > max_socket) {
            max_socket = ci->socket;
        }
        ci = ci->next;
    }

    if (select(max_socket + 1, &reads, 0, 0, 0) < 0) {
        fprintf(stderr, "select() failed. (%d)\n", errno);
        exit(1);
    }

    return reads;
}

