#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "log_manager.h"
#include "message.h"

#define PORT 5001
#define IP   "192.168.1.149"
//"192.168.1.148"

int sendMessage(int fd, Message * msg) {
    uint32_t packet_size = msg->get_packet_len();
    char data[packet_size];
    msg->to_packet(data);
    uint32_t bytes_sent = 0;
    while (bytes_sent < packet_size) {
        int32_t size = send(fd, data + bytes_sent, packet_size - bytes_sent, 0);
        if (size > 0)
            bytes_sent += size;
    }
    return 0;
}

int main(){
    // create log manager
    LogManager manager = LogManager();

    // create socket
    printf("creating socket ...");
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET; 0 [ or IPPROTO_IP This is IP protocol]
    printf("[done]\n");

    // server addr
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  // fill bytes with zeros
    serv_addr.sin_family = AF_INET;  // use IPv4
    serv_addr.sin_addr.s_addr = inet_addr(IP); //INADDR_ANY; //inet_addr(IP);  // IP addr
    serv_addr.sin_port = htons(PORT);  // port

    // bind socket
    printf("binding socket ...");
    
    int status = bind(server_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (status > 0)
        printf("[done]\n");
    else
        printf("[error]\n");

    // listening
    printf("listening ...\n");
    listen(server_socket, 5);

    // accept request
    char sBuffer[1024];
    struct sockaddr_in clnt_addr;
    int new_sd;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    while(true) {
        // wait for client
        int client_socket = accept(server_socket, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (client_socket == -1)
            continue;
        printf("successful connection: %s:%d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

        // successful connection
        memset(sBuffer, 0, sizeof(sBuffer));
        int nRes = recv(client_socket, sBuffer, sizeof(sBuffer), 0);
        Message * m = manager.handleRequest(sBuffer);

        // send msg to client
        nRes = sendMessage(client_socket, m);
        close(client_socket);
        break;
    }
    // close socket
    close(server_socket);
    return 0;
}