#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
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


int main()
{
	printf("creating socket ...");
    int socketServer = socket(AF_INET,SOCK_STREAM, 0);
    if(socketServer < 0)
    {
        perror("socket");
        return 1;
    }
    printf(" [done]\n");

    printf("connecting server at %s:%d...", IP, PORT);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(IP);
    
    if(connect(socketServer, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) < 0 )
    {
        perror("connect");
        return 2;
    }
    printf(" [done]\n");

    char sBuffer[1024];
    // send msg
    char data[1024-sizeof(Message)];
    sprintf(data, "YES");
    Message * m = new Message(Message::PREPARED, 0, 1, strlen(data)+1, data);
    int nRes = sendMessage(socketServer, m);

	// receive msg
	memset(sBuffer, 0, sizeof(sBuffer));
	nRes = recv(socketServer, sBuffer, sizeof(sBuffer), 0);
	printf("%s %s\n", sBuffer, inet_ntoa(server.sin_addr));

    close(socketServer);
    return 0;
}
