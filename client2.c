// Author: Ibrahim USLU
// Description: This is the client2 for multiple socket ports. 
// It is used to connect to the server reads the message from the server 
// and sends message to control port to alter settings and prints it to the screen.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>


#define PORT_C 4000 // control port

#define PORT1 4001 // server output 1 port
#define PORT2 4002 // server output 2 port
#define PORT3 4003 // server output 3 port

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

void create_socket(int* server_socket, int type){
    *server_socket = socket(AF_INET, type, 0);

    printf("Socket created %d\n", *server_socket);
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
// Function to convert hex string to binary data
void hex_to_bin(const char *hex, unsigned char *bin, size_t bin_len) {
    for (size_t i = 0; i < bin_len; ++i) {
        sscanf(hex + 2 * i, "%2hhx", &bin[i]);
    }
}
void write_data(struct Client_data * client, const char* data){
    size_t hex_len = strlen(data);
    size_t bin_len = hex_len / 2;
    unsigned char bin_data[bin_len];

    // Convert hex string to binary data
    hex_to_bin(data, bin_data, bin_len);
    if(sendto(client->server_socket, bin_data, bin_len,0,(const struct sockaddr *)&client->server_addr, sizeof(client->server_addr)) < 0){
        perror("Error in sending data");
    }
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
        create_socket(&clients[i].server_socket, SOCK_STREAM);
        connect_to_server(&clients[i].server_socket, &clients[i].server_addr, PORT_C +1+ i);
        pthread_create(&thread[i], NULL, &read_data, (void *)&clients[i]);
    }
    create_socket(&clients[3].server_socket, SOCK_DGRAM);
    clients[3].server_addr.sin_family = AF_INET;
    clients[3].server_addr.sin_port = htons(4000);
    clients[3].server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1)
    {
        // print timestamp and message
        gettimeofday(&tv, NULL);
        long long milliseconds = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
        // read data but print it to the screen at every 20 ms
        if (milliseconds - last_time >= 20)
        {
            last_time = milliseconds;
            printf("{\"timestamp\": %lld, \"out1\":\"%s\", \"out2\":\"%s\", \"out3\":\"%s\"}\n", milliseconds, clients[0].output, clients[1].output, clients[2].output);
            // after consuming the data, reset the data
            if(clients[2].output[0] == '-'){
                continue;
            }
            else if (atof(clients[2].output) >= 3.0)
            {
                write_data(&clients[3], "0002000100FF03E8");
                write_data(&clients[3], "0002000100AA1F40");
            }
            else if (atof(clients[2].output) < 3.0)
            {
                write_data(&clients[3], "0002000100FF01F4");
                write_data(&clients[3], "0002000100AA0FA0");
            }
            
            
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