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
    std::cout << "Potato Ringmaster" << endl;
    std::cout << "Players = " << num_players << endl;
    std::cout << "Hops = " << num_hops << endl;
    int fd = master.init(ptr, port);
    std::vector<int> fd_list;
    std::vector<string> host_list;
    std::vector<string> port_list;
    std::vector<Player> player_list;
    // connect with clinets

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
        send(client_connection_fd, &num_players, sizeof(int), 0);
        std::cout << "Player " << id << " is ready to play" << endl;
    }
    // store players
    for (int i = 0; i < num_players; i++) {
        Player player;
        recv(fd_list[i], &player, sizeof(Player), 0);
        player_list.push_back(player);
    }

    // send neighbor
    for (int i = 0; i < num_players; i++) {
        size_t left_index = (i - 1 + num_players) % num_players;
        size_t right_index = (i + 1) % num_players;
        Player left_player = player_list[left_index];
        Player right_player = player_list[right_index];
        send(fd_list[i], &left_player, sizeof(Player), 0);
        send(fd_list[i], &right_player, sizeof(Player), 0);
    }
    Potato potato(num_hops);
    srand(time(0));
    int first_player = rand() % num_players;
    if (num_hops != 0) {
        std::cout << "Ready to start the game, sending potato to player " << first_player << endl;
        send(fd_list[first_player], &potato, sizeof(potato), 0);
        fd_set read_fds;
        FD_ZERO(&read_fds);
        int max_fd = fd_list[0];
        for (int i = 0; i < num_players; i++) {
            if (fd_list[i] > max_fd) {
                max_fd = fd_list[i];
            }
            FD_SET(fd_list[i], &read_fds);
        }

        fd_set tmp_fds = read_fds;
        if (select(max_fd + 1, &tmp_fds, NULL, NULL, NULL) == -1) {
            cerr << "Error: select failed" << endl;
            return -1;
        }
        for (int i = 0; i < num_players; i++) {
            if (FD_ISSET(fd_list[i], &tmp_fds)) {
                recv(fd_list[i], &potato, sizeof(Potato), 0);
                break;
            }
        }

        potato.printPath();
    }
    for (int i = 0; i < num_players; i++) {
        close(fd_list[i]);
    }
    close(fd);
    return 0;
}
