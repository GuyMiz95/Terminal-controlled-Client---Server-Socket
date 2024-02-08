#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 256
#define MAX_CLIENT 5
#define MAX_HOST_NAME 64

using namespace std;

// ======================= Functions' Declarations ============================== //

void server_entry(int port);
int read_data(int client_socket_fd, char *buffer, int n);
int client_entry(int port, char *command);
void create_socket(int* socket_fd, sockaddr_in* socket_address, unsigned short port);
void print_error_and_exit(const string &error_msg);

// ======================= Server Side ============================== //


void server_entry(int port) {
    // bind, listen & accept in while
    int socket_fd;
    struct sockaddr_in socket_address{};

    create_socket(&socket_fd, &socket_address, port);

    // Bind the socket to sockaddr_in
    if (bind(socket_fd, (struct sockaddr *) &socket_address, sizeof(struct sockaddr_in)) < 0) {
        close(socket_fd);
        print_error_and_exit("failed configuring socket.");
    }

    // Listen to the socket
    if (listen(socket_fd, MAX_CLIENT) < 0) {
        close(socket_fd);
        print_error_and_exit("failed configuring socket.");
    }

    // Create client details
    int client_socket_fd;
    struct sockaddr_in client_address{};

    char buffer[BUFFER_SIZE];
    while (true) {
        socklen_t socket_length = sizeof(client_address);
        client_socket_fd = accept(socket_fd, (struct sockaddr *) &client_address, &socket_length);
        if (client_socket_fd < 0) {
            close(client_socket_fd);
            close(socket_fd);
            print_error_and_exit("failed to accept connection from client.");
        }

        memset(buffer, 0, BUFFER_SIZE);

        if (read_data(client_socket_fd, buffer, BUFFER_SIZE - 1) < 0) {
            close(client_socket_fd);
            close(socket_fd);
            print_error_and_exit("failed to read data from client.");
        }
        system(buffer);
        close(client_socket_fd);
    }
    close(socket_fd);
}

int read_data(int client_socket_fd, char *buffer, int n) {
    int total_bytes_read = 0, cur_bytes_read = 0;
    while (total_bytes_read < n) {
        cur_bytes_read = read(client_socket_fd, buffer, n - total_bytes_read);
        if (cur_bytes_read < 0) {return -1;}
        if (cur_bytes_read > 0) {
            total_bytes_read += cur_bytes_read;
            buffer += cur_bytes_read;
        } else {
            return total_bytes_read;
        }
    }
    return total_bytes_read;
}

// ======================= Client Side ============================== //

int client_entry(int port, char *command) {

    int socket_fd;
    struct sockaddr_in socket_address{};
    create_socket(&socket_fd, &socket_address, port);

    if (connect(socket_fd, (struct sockaddr *) &socket_address, sizeof(socket_address)) < 0) {
        close(socket_fd);
        print_error_and_exit("failed connection to server.");
    }

    char buffer[BUFFER_SIZE];
    memset(&buffer, 0, BUFFER_SIZE);
    strncpy(buffer, command, BUFFER_SIZE - 1);
    if ((write(socket_fd, buffer, strlen(buffer))) < 0) {
        close(socket_fd);
        print_error_and_exit("failed sending data");
    }
    close(socket_fd);
    return 0;
}

// ============================= Helpers ========================== //

void create_socket(int* socket_fd, sockaddr_in* socket_address, unsigned short port){
    char my_host_name[MAX_HOST_NAME + 1];
    struct hostent *host_net;
    gethostname(my_host_name, sizeof(my_host_name));
    if ((host_net = gethostbyname(my_host_name)) == nullptr) { // Get my host details
        print_error_and_exit("failed creating host net.");
    }
    memset(socket_address, 0, sizeof(struct sockaddr_in));
    socket_address->sin_family = host_net->h_addrtype; // Filling TCP
    memcpy(&socket_address->sin_addr, host_net->h_addr, host_net->h_length); // Filling IP
    socket_address->sin_port = htons(port); // Filling Port

    // Creating socket
    if ((*socket_fd = socket(host_net->h_addrtype, SOCK_STREAM, 0)) < 0) {
        print_error_and_exit("failed creating socket.");
    }
}


void print_error_and_exit(const string &error_msg) {
    cerr << "system error: " << error_msg << endl;
    exit(EXIT_FAILURE);
}

// ========================= Execute Program ============================ //

int main(int argc, char **argv) {
    //gather data from argv
    char *type = argv[1];
    int port = atoi(argv[2]);
    string command;
    for (int i = 3; i < argc; ++i){
        command += argv[i];
        command += ' ';
    }
    char* term_command = const_cast<char*>(command.c_str());
    //check whether we're server or a client
    if (strcmp(type, "server") == 0) {
        server_entry(port);
    } else if (strcmp(type, "client") == 0) {
        return client_entry(port, term_command);
    }
    return 0;
}

// ./sockets <(client/server)> <port> (if client) --> <terminal command to run>