#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

#include "potato.hpp"
#include "server.hpp"

using namespace std;
// player<ma`chine_name><port_num>(example
// ./ player vcm - xxxx.vm.duke.edu 1234)
int main(int argc, char* argv[]) {
    if (argc != 3) {
        perror("invalid input");
        return EXIT_FAILURE;
    }
    char* host = argv[1];
    char* port = argv[2];
    Client Player_client;
    int socket_fd = Player_client.init(host, port);

    int id;
    int num_players;
    int fd_left;
    int fd_left_t;
    int fd_right;

    recv(socket_fd, &id, sizeof(int), 0);

    recv(socket_fd, &num_players, sizeof(int), 0);
    cout << "Connected as player " << id << " out of " << num_players << " total players" << endl;
    srand((unsigned int)time(NULL) + id);
    Server player_server;
    char new_port[512];
    int player_server_fd = player_server.init(new_port);
    char name[512] = "";
    gethostname(name, sizeof(name));
    // store info in player, send it to master
    Player player;
    player.id = id;
    strcpy(player.hostname, name);

    strcpy(player.port, new_port);
    send(socket_fd, &player, sizeof(Player), 0);
    // receive info from master
    Player left_player;
    Player right_player;
    recv(socket_fd, &left_player, sizeof(Player), 0);
    recv(socket_fd, &right_player, sizeof(Player), 0);
    // connecting neighbors
    //  connect right
    int right_fd = Player_client.init(right_player.hostname, right_player.port);
    // listen to left
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int left_fd = accept(player_server_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
    if (right_fd == -1) {
        cerr << "Error: cannot accept connection on socket" << endl;
        return -1;
    }  // if
    /************** testing connection*************/
    // std::cout << "connected" << endl;
    // char message[512] = "hello left from id ";
    // const char* id_str = std::to_string(id).c_str();
    // strcat(message, id_str);
    // send(left_fd, message, strlen(message) * sizeof(char), 0);
    // cout << "send message:" << message << endl;
    // char buffer[512];
    // recv(right_fd, buffer, strlen(message) * sizeof(char), 0);
    // buffer[strlen(message)]='\0';
    // cout << "Server received: " << buffer << endl;
    /*********************************************/
    Potato potato;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(left_fd, &read_fds);
    FD_SET(right_fd, &read_fds);
    FD_SET(socket_fd, &read_fds);

    int max_fd = max(left_fd, max(right_fd, socket_fd));

    int hasPotato = 0;

    // passing potatos
    while (1) {
        fd_set tmp_fds = read_fds;
        if (select(max_fd + 1, &tmp_fds, NULL, NULL, NULL) == -1) {
            cerr << "Error: select failed" << endl;
            return -1;
        }
        if (FD_ISSET(left_fd, &tmp_fds)) {
            if (recv(left_fd, &potato, sizeof(Potato), 0) == 0) {
                return 0;
            }
            // std::cout << "receive potato from left, hops is " << potato.hops << endl;
            hasPotato = 1;
        } else if (FD_ISSET(right_fd, &tmp_fds)) {
            if (recv(right_fd, &potato, sizeof(Potato), 0) == 0) {
                return 0;
            };
            // std::cout << "receive potato from right, hops is " << potato.hops << endl;
            hasPotato = 1;
        } else if (FD_ISSET(socket_fd, &tmp_fds)) {
            if (recv(socket_fd, &potato, sizeof(Potato), 0) == 0) {
                return 0;
            }

            // std::cout << "receive potato from server, hops is " << potato.hops << endl;
            hasPotato = 1;
        }
        // receiving potato
        if (hasPotato == 1) {
            if (potato.hops <= 0) {
                potato.path[potato.index] = id;
                potato.index++;
                send(socket_fd, &potato, sizeof(Potato), 0);
                close(left_fd);
                close(right_fd);
                close(socket_fd);
                std::cout << "I'm it" << endl;
                return 0;
            } else {
                if (potato.hops <= 0) {
                    return 0;
                }
                potato.hops -= 1;
                potato.path[potato.index] = id;
                potato.index += 1;
                int next_player = rand() % 2;
                int next_id;
                if (next_player == 0) {
                    next_id = (id - 1 + num_players) % num_players;
                    send(left_fd, &potato, sizeof(Potato), 0);
                    // std::cout << "send potato to" << endl;
                } else {
                    next_id = (id + 1 + num_players) % num_players;
                    send(right_fd, &potato, sizeof(Potato), 0);
                    // id = (id + 1 + num_players) % num_players;
                    // std::cout << "send to the right" << endl;
                }
                hasPotato = 0;
                std::cout << "send potato to " << next_id << endl;
            }
        }
    }
    return 0;
}
