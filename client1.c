// Author: Ibrahim USLU
// Description: This is the client1 for multiple socket ports. It is used to connect to the server reads the message from the server and prints it to the screen.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>

#define PORT1 4001
#define PORT2 4002
#define PORT3 4003

typedef struct Client_data
{
    int server_socket;
    char output[1024];
    struct sockaddr_in server_addr;
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

void connect_to_server(int* server_socket, struct sockaddr_in * server_addr, int PORT){
    bzero(server_addr, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    server_addr->sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Client is connected to the server %d %d \n", *server_socket, PORT);
    if(connect(*server_socket, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0){
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

struct timeval tv;
long long last_time = 0;

int main(int argc, char **argv){
   
    struct Client_data clients[4] = {cd, cd, cd, cd};
    pthread_t thread[3] = {0};

    for (int i = 0; i < 3; i++)
    {
        create_socket(&clients[i].server_socket);
        connect_to_server(&clients[i].server_socket, &clients[i].server_addr, PORT1 + i);
        pthread_create(&thread[i], NULL, &read_data, (void *)&clients[i]);
    }

    while (1)
    {
         // print timestamp and message
        gettimeofday(&tv, NULL);
        long long milliseconds = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
        // read data but print it to the screen at every 20 ms
        if (milliseconds - last_time > 100)
        {
            last_time = milliseconds;
            printf("{\"timestamp\": %lld, \"out1\":\"%s\", \"out2\":\"%s\", \"out3\":\"%s\"}\n", milliseconds, clients[0].output, clients[1].output, clients[2].output);
            // after consuming the data, reset the data
            for (int i = 0; i < 3; i++)
            {
                clients[i].output[0] = '-';
                clients[i].output[1] = '-';
                clients[i].output[2] = '\0';
            }
        }

    }
    for (int i = 0; i < 3; i++)
    {
        pthread_join(thread[i], NULL);
    }
    for (int i = 0; i < 3; i++)
    {
        close_socket(clients[i].server_socket);
    }
    return 0;
}