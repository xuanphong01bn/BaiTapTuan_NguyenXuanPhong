#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include <time.h>
void error(const char *msg){
	perror(msg);
	exit(1);
}

int main(int argc,char *argv[]){
	
	int sockfd,portno,n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	if(argc < 2){
		fprintf(stderr,"usage %s hotname port\n",argv[0]);
		exit(1);
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	if(sockfd < 0){
		error("Error opening socket");
	}

	server = gethostbyname(argv[1]);
	if(server == NULL){
		fprintf(stderr,"no Such host");
        exit(0);
	}
	bzero((char *) &serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);
	if(connect(sockfd, (struct sockaddr *) &serv_addr , sizeof(serv_addr)) < 0){
		error("Connection Failed !!");
	}
      	
  
	// Tên máy tính
    char Name[255];
	n = read(sockfd,Name,255);
	if(n < 0){
		error("Error reading From Socket");
	}

	printf("Server - %s\n",Name);
	scanf ("%[^\n]%*c", Name);
	write(sockfd,Name,255);

	//CDisk
    char CDisk[255];
	n = read(sockfd,CDisk,255);
	if(n < 0){
		error("Error reading From Socket");
	}

	printf("Server - %s\n",CDisk);
	scanf ("%[^\n]%*c", CDisk);
	write(sockfd,CDisk,255);

	//DDisk
    char DDisk[255];
	n = read(sockfd,DDisk,255);
	if(n < 0){
		error("Error reading From Socket");
	}

	printf("Server - %s\n",DDisk);
	scanf ("%[^\n]%*c", DDisk);
	write(sockfd,DDisk,255);

	//EDisk
    char EDisk[30];
	n = read(sockfd,EDisk,30);
	if(n < 0){
		error("Error reading From Socket");
	}

	printf("Server - %s\n",EDisk);
	scanf ("%[^\n]%*c", EDisk);
	write(sockfd,EDisk,30);

	//IP

    // char IP[30];
	// n = read(sockfd,IP,30);
	// if(n < 0){
	// 	error("Error reading From Socket");
	// }
	// // printf("Server - %s\n",IP);
	// // IP[0]=argv[1];
	// write(sockfd,argv[1],30);

	// time_t now = time(NULL);
    // struct tm *t = localtime(&now);
    // char time[100];
    // strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", t);
    // // printf("%s\n", time);

	// // get time now
	// char time_now[100];
	// n = read(sockfd,time_now,30);
	// if(n < 0){
	// 	error("Error reading From Socket");
	// }
	// // printf("Server - %s\n",IP);
	// // IP[0]=argv[1];
	// write(sockfd,time,30);

	close(sockfd);
	return 0;

}