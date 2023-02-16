#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

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
    recv(socket_fd, &id, 10, 0);
    cout << "id is :" << id << endl;
    return 0;
}
