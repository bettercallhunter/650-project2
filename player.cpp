#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

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
    int fd_left;
    int fd_left_t;
    int fd_right;
    Potato potato;
    recv(socket_fd, &id, sizeof(id), 0);
    cout << "id is :" << id << endl;

    Server player_server;
    char new_port[512];
    int player_server_fd = player_server.init("", new_port);
    char name[512] = "";
    gethostname(name, sizeof(name));
    // store info in player, send it to master
    Player player;
    player.id = id;
    strncpy(player.hostname, name, strlen(name));
    strncpy(player.port, new_port, strlen(new_port));
    send(socket_fd, &player, sizeof(Player), 0);
    // receive info from master
    Player left_player;
    Player right_player;
    recv(socket_fd, &left_player, sizeof(Player), 0);
    recv(socket_fd, &right_player, sizeof(Player), 0);
    // connect with left and right neighbor
    int left_fd = Player_client.init(left_player.hostname, left_player.port);
    int right_fd = Player_client.init(right_player.hostname, right_player.port);

    return 0;
}
