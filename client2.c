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

void create_socket(int* server_socket, int type){
    *server_socket = socket(AF_INET, type, 0);

    printf("Socket created %d\n", *server_socket);
    if(*server_socket < 0){
        perror("Socket creation failed");
        exit(1);
    }
}

void connect_to_server(int* server_socket, int PORT){
    bzero(&server_addr, sizeof(server_addr));
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

void write_data(int server_socket, char* data){
    printf("Data to be sent: %s\n", data);
    if(sendto(server_socket, data, strlen(data),0,(struct sockaddr *)&server_addr, sizeof(server_addr))==1){
        perror("Error in sending data");
        exit(1);
    }
}

void close_socket(int server_socket){
    close(server_socket);
}

struct timeval tv;
long long last_time = 0;
int main(int argc, char **argv){
    struct Client_data client_data1 = cd;
    struct Client_data client_data2 = cd;
    struct Client_data client_data3 = cd;
    struct Client_data client_data_control;
    create_socket(&client_data_control.server_socket, SOCK_DGRAM);
    create_socket(&client_data1.server_socket, SOCK_STREAM);
    create_socket(&client_data2.server_socket, SOCK_STREAM);
    create_socket(&client_data3.server_socket, SOCK_STREAM);

    connect_to_server(&client_data_control.server_socket, PORT_C);
    connect_to_server(&client_data1.server_socket, PORT1);
    connect_to_server(&client_data2.server_socket, PORT2);
    connect_to_server(&client_data3.server_socket, PORT3);

    // create thread from read_data function
    pthread_t thread1, thread2, thread3;
    pthread_create(&thread1, NULL, &read_data, (void *)&client_data1);
    pthread_create(&thread2, NULL, &read_data, (void *)&client_data2);
    pthread_create(&thread3, NULL, &read_data, (void *)&client_data3);

    // send message to control port to alter settings in binary 6 bytes
    char control_value[6] = {0x00,0x01,0x00,0x01,0x00,0xFF};
    // convert the double to char array
    write_data(client_data_control.server_socket, control_value);
    while (1)
    {
        // print timestamp and message
        gettimeofday(&tv, NULL);
        long long milliseconds = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
        // read data but print it to the screen at every 20 ms
        if (milliseconds - last_time > 20)
        {
            last_time = milliseconds;
            printf("{\"timestamp\": %lld, \"out1\":\"%s\", \"out2\":\"%s\", \"out3\":\"%s\"}\n", milliseconds, client_data1.output, client_data2.output, client_data3.output);
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
        
       

    }
    close_socket(client_data_control.server_socket);
    close_socket(client_data1.server_socket);
    close_socket(client_data2.server_socket);
    close_socket(client_data3.server_socket);
    return 0;
}