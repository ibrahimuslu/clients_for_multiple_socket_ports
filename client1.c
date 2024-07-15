// Author: Ibrahim USLU
// Description: This is the client1 for multiple socket ports. It is used to connect to the server reads the message from the server and prints it to the screen.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#define PORT1 4001
#define PORT2 4002
#define PORT3 4003

struct sockaddr_in server_addr;
typedef struct Client_data
{
    int server_socket;
    char output[1024];
} Client_data;


Client_data cd = {
.server_socket = 0, 
.output = {'-', '-', '\0'}
};

void create_socket(int* server_socket){
    *server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(*server_socket < 0){
        perror("Socket creation failed");
        exit(1);
    }
}

void connect_to_server(int* server_socket, int PORT){

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Client is connected to the server %d %d \n", *server_socket, PORT);
    if(connect(*server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("Connection failed");
        exit(1);
    }
}
static void *read_data(void *arg){
    struct Client_data * client_data = (struct Client_data*)arg;
    char data[1024];
    while(1){
        read(client_data->server_socket, data, sizeof(data));
   
        // Trim the newline character from the data
        char* trimmed_data = strtok(data, "\n");
        strcpy(client_data->output, trimmed_data);
        bzero(data, sizeof(data));
    }
    return NULL;
}

void close_socket(int server_socket){
    close(server_socket);
}

int main(int argc, char **argv){
    struct Client_data client_data1;
    struct Client_data client_data2;
    struct Client_data client_data3;
    create_socket(&client_data1.server_socket);
    create_socket(&client_data2.server_socket);
    create_socket(&client_data3.server_socket);

    connect_to_server(&client_data1.server_socket, PORT1);
    connect_to_server(&client_data2.server_socket, PORT2);
    connect_to_server(&client_data3.server_socket, PORT3);

    // create thread from read_data function
    pthread_t thread1, thread2, thread3;
    pthread_create(&thread1, NULL, &read_data, (void *)&client_data1);
    pthread_create(&thread2, NULL, &read_data, (void *)&client_data2);
    pthread_create(&thread3, NULL, &read_data, (void *)&client_data3);

    while (1)
    {
        // read data but print it to the screen at every 100 ms
        usleep(100000);
        // print timestamp and message
        time_t current_time;
        time(&current_time);
        printf("{\"timestamp\": %ld, \"out1\":\"%s\", \"out2\":\"%s\", \"out3\":\"%s\"}\n", current_time, client_data1.output, client_data2.output, client_data3.output);
        // after consuming the data, reset the data
        client_data1.output[0] = '-';
        client_data1.output[1] = '-';
        client_data1.output[2] = '\0';
        client_data2.output[0] = '-';
        client_data2.output[1] = '-';
        client_data2.output[2] = '\0';
        client_data3.output[0] = '-';
        client_data3.output[1] = '-';
        client_data3.output[2] = '\0';

    }
    close_socket(client_data1.server_socket);
    close_socket(client_data2.server_socket);
    close_socket(client_data3.server_socket);
    return 0;
}