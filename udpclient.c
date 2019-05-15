/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ncurses.h>

#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

struct __attribute__((__packed__)) Robot
{
  float velo;
  float theta;
};

void send_Robot(struct Robot c, int sockfd, struct sockaddr_in serveraddr);

int main(int argc, char **argv)
{
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    int key;
    struct Robot input_Robot = {0.0f, 0.0f};
    //struct robot_info cur_info;

    /* check Robot line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    scanf("%f %f", &(input_Robot.velo), &(input_Robot.theta));
		send_Robot(input_Robot, sockfd, serveraddr);

    return 0;
}

void send_Robot(struct Robot c, int sockfd, struct sockaddr_in serveraddr)
{
	int serverlen = sizeof(serveraddr);
	/* send the message to the server */
	if (sendto(sockfd, &c, sizeof(c), 0, &serveraddr, serverlen) < 0)
		error("ERROR in sendto");
}


