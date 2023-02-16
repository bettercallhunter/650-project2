#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "server.hpp"

using namespace std;
// ringmaster<port_num><num_players><num_hops>
// (example: ./ringmaster 1234 3 100)
int main(int argc, char* argv[]) {
    if (argc != 4) {
        perror("invalid input");
        exit(EXIT_FAILURE);
    }
    char* port = argv[1];
    int num_players = atoi(argv[2]);
    int num_hops = atoi(argv[3]);
    std::vector<int> fd_list;
    for (int i = 0; i < num_players; i++) {
        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        int client_connection_fd;
        client_connection_fd = accept(socket_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
        if (client_connection_fd == -1) {
            cerr << "Error: cannot accept connection on socket" << endl;
            return -1;
        }  // if
        fd_list.push_back(client_connection_fd);

        int* id = &i;
        send(client_connection_fd, id, sizeof(id), 0);


        // cout << "Server received: " << buffer << endl;
    }
}
