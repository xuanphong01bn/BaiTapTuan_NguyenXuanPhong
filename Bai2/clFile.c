#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

void error(const char *msg){
	perror(msg);
	exit(1);
}

int main(int argc,char *argv[]){
	
	int sockfd,portno,n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	if(argc < 3){
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
      	
  
	// MSSV
    char MSSV[255];
	n = read(sockfd,MSSV,255);
	if(n < 0){
		error("Error reading From Socket");
	}

	printf("Server - %s\n",MSSV);
	scanf("%s",MSSV);
	write(sockfd,MSSV,255);

	//hoten
    char hoten[255];
	n = read(sockfd,hoten,255);
	if(n < 0){
		error("Error reading From Socket");
	}

	printf("Server - %s\n",hoten);
	scanf("%s",hoten);
	write(sockfd,hoten,255);

	//ngaysinh
    char ngaysinh[255];
	n = read(sockfd,ngaysinh,255);
	if(n < 0){
		error("Error reading From Socket");
	}

	printf("Server - %s\n",ngaysinh);
	scanf("%s",ngaysinh);
	write(sockfd,ngaysinh,255);

	//GPA
    char GPA[30];
	n = read(sockfd,GPA,30);
	if(n < 0){
		error("Error reading From Socket");
	}

	printf("Server - %s\n",GPA);
	scanf("%s",GPA);
	write(sockfd,GPA,30);

	//IP
	// char IP[255]
	// // char IP[255]=argv[1];
	// write(sockfd,IP,30);
	printf("Cong %s:",argv[1]);
	close(sockfd);
	return 0;

}