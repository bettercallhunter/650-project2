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
    Client Player;
    int socket_fd = Player.init(host, port);
    int id;
    int fd_left;
    int fd_right;
    Potato potato;

    recv(socket_fd, &id, sizeof(id), 0);
    cout << "id is :" << id << endl;
    recv(socket_fd, &fd_left, sizeof(fd_left), 0);
    cout << "left fd is:" << fd_left << endl;
    recv(socket_fd, &fd_right, sizeof(fd_right), 0);
    cout << "right fd is:" << fd_right << endl;
    recv(socket_fd, &potato, sizeof(potato), 0);
    std::cout << potato.hops << endl;
    // send(socket_fd, potato, sizeof(potato), 0);

    return 0;
}
