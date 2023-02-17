#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

#include "potato.hpp"
#include "server.hpp"
using namespace std;
// ringmaster<port_num><num_players><num_hops>
// (example: ./ringmaster 1234 3 100)
int main(int argc, char* argv[]) {
    if (argc != 4) {
        perror("invalid input");
        exit(EXIT_FAILURE);
    }
    Server master;
    char ptr[100];
    char* port = argv[1];
    int num_players = atoi(argv[2]);
    int num_hops = atoi(argv[3]);
    int fd = master.init(port, ptr);
    std::vector<int> fd_list;
    std::vector<string> host_list;
    std::vector<string> port_list;
    std::vector<Player> player_list;
    //connect with clinets
    for (int i = 0; i < num_players; i++) {
        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        int client_connection_fd;
        client_connection_fd = accept(fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
        if (client_connection_fd == -1) {
            cerr << "Error: cannot accept connection on socket" << endl;
            return -1;
        }  // if
        fd_list.push_back(client_connection_fd);

        int id = i;
        send(client_connection_fd, &id, sizeof(id), 0);
    }
    //store players
    for (int i = 0; i < num_players; i++) {
        Player player;
        recv(fd_list[i], &player, sizeof(Player), 0);
        player_list.push_back(player);
    }
    //send neighbor
    for (int i = 0; i < num_players; i++) {
        size_t left_index = (i - 1 + num_players) % num_players;
        size_t right_index = (i + 1) % num_players;
        Player left_player = player_list[left_index];
        Player right_player = player_list[right_index];
        send(fd_list[i], &left_player, sizeof(Player), 0);
        send(fd_list[i], &right_player, sizeof(Player), 0);
    }
    
    Potato potato(num_hops);
    // send(fd_list[0], &potato, sizeof(potato), 0);
}
