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

//#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */

void error(char *msg) {
    perror(msg);
    exit(0);
}

 struct __attribute__((packed))  Robot //send
  {
      float velo;
      float theta;
    //  int mode;
  }*Robot_t;

   struct __attribute__((packed)) Info //receive
  {
      float xpos; //odo
      float ypos;
      float zpos;
      float ax;   //accelerator
      float ay;
      float az;
      float mx;   //magnetometer
      float my;
      float mz;
      float theta; //heading
     
  }*Info_t;

	
int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    initscr();
	  cbreak();
    noecho();
	  keypad(stdscr, TRUE);

    /* check command line arguments */
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

    /* get a message from the user */
   // bzero(buf, BUFSIZE);
    //printf("Please enter msg: ");
    //fgets(buf, BUFSIZE, stdin);
    memset(&Robot_t, 0, sizeof(Robot_t));
   // scanf("%f %f %d", &(Robot_t.velo), &(Robot_t.theta), &(Robot_t.mode));
	
	
    while(1)
    {
	      int ch = getch();
        switch (ch)
        {
   		      case KEY_BACKSPACE: 
				          Robot_t->velo = 0;
                  break;
	          case KEY_UP:
                  if (Robot_t->velo > 255)
                      Robot_t->velo = 255;
                  else
                    Robot_t->velo += 10;  
                  break;            
            case KEY_DOWN: 
                  if (Robot_t->velo < 30)
                      Robot_t->velo = 0;
                  else
                    Robot_t->velo -= 10; 
                  break; 
            case KEY_LEFT: 
                  Robot_t->theta -=15;
                  break;
            case KEY_RIGHT:  
                  Robot_t->theta += 15;
                  break;
            case 'W': 
                  Robot_t->theta = 5; 
                  break;
            case 'A':
                  Robot_t->theta = 270;
                  break;
            case 'S':
                  Robot_t->theta = 180;
                  break;     
            case 'D': 
                  Robot_t->theta = 90;
                  break; 
            default:
	                break;   

          }

	
    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, &Robot_t, sizeof(Robot_t), 0, &serveraddr, serverlen);
    if (n < 0) 
      error("ERROR in sendto");
    
    /* print the server's reply */
   n = recvfrom(sockfd, &Info_t, sizeof(Info_t), 0, &serveraddr, &serverlen);
   if (n < 0) 
     error("ERROR in recvfrom");
   printf("Echo from server: \n");
   printf( "xpos, ypos, zpos: %f %f %f \n", Info_t->xpos, Info_t->ypos, Info_t->zpos);
   printf( "ax, ay, az: %f %f %f \n", Info_t->ax, Info_t->ay, Info_t->az);
   printf( "mx, my, mz: %f %f %f \n", Info_t->mx, Info_t->my, Info_t->mz);
   printf("heading:  %f\n", Info_t->theta);
	  refresh();
}
endwin();
    return 0;
}
