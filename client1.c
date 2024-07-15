// Author: Ibrahim USLU
// Description: This is the client1 for multiple socket ports. It is used to connect to the server reads the message from the server and prints it to the screen.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT1 4001
#define PORT2 4002
#define PORT3 4003

struct sockaddr_in server_addr;
int server_socket;
char data[1024];

void create_socket(){
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0){
        perror("Socket creation failed");
        exit(1);
    }
}

void connect_to_server(){
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT1);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("Connection failed");
        exit(1);
    }
}

void read_data(){
    read(server_socket, data, sizeof(data));
   
    // Trim the newline character from the data
    char* trimmed_data = strtok(data, "\n");
    
    // print timestamp and message
    time_t current_time;
    time(&current_time);
    printf("{\"timestamp\": %ld, \"out1\":\"%s\"}\n", current_time, trimmed_data);
}

void close_socket(){
    close(server_socket);
}

int main(int argc, char **argv){
    create_socket();
    connect_to_server();
    while (1)
    {
        read_data();
    }
    close_socket();
    return 0;
}