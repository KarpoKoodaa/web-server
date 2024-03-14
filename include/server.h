#ifndef SERVER_H
#define SERVER_H


#define MAX_REQUEST_SIZE 2047 
struct client_info {
    socklen_t addr_length;
    struct sockaddr_storage address;
    char address_buffer[128];
    SOCKET socket;
    char request[MAX_REQUEST_SIZE + 1];
    int received;
    struct client_info *next;
};


void serve_resource(struct client_info **client_list, struct client_info *client, const char *path);




#endif
