#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

void error(const char *msg){
	perror(msg);
	exit(1);
}

// argc is the total number of parameter we are passing 

int main(int argc,char *argv[]){
	if(argc < 2){
		fprintf(stderr , "Port not providede . Program Terminated \n");
		exit(1);
	}

	int sockfd,newsockfd,portno,n;
	char buffer[255]; // to store msg ot send

	struct sockaddr_in serv_addr , cli_addr;
	socklen_t clilen; //socklen_t is a datatype in socket.h 32 bit 

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		// if sock fd is resultaing in failure
		error("Error opening socket");
	} 
	
	bzero((char *) &serv_addr,sizeof(serv_addr)); // it clears all the data to what it it reference to 
      
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if(bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		error("Binding Failed");

	}

	// next step is our server is trying to connect
	
	listen(sockfd,5);
	clilen = sizeof(cli_addr);

	newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr , & clilen);
	if(newsockfd < 0){
		error("Error on Accept");
	}

	bzero(buffer,255);
	//Name
	char Name[255];
	n = write(newsockfd,"Enter name computer : ",strlen("Enter name computer : "));
	if(n < 0){
		error("Error Writing to socket");
	}
	read(newsockfd,Name,255);

	//CDisk
	char CDisk[255];
	n = write(newsockfd,"Enter C Disk : ",strlen("Enter hoten : "));
	if(n < 0){
		error("Error Writing to socket");
	}
	read(newsockfd,CDisk,255);

	//DDisk
	char DDisk[255];
	n = write(newsockfd,"Enter D disk : ",strlen("Enter D disk : "));
	if(n < 0){
		error("Error Writing to socket");
	}
	read(newsockfd,DDisk,255);

	//EDisk
	char EDisk[255];
	n = write(newsockfd,"Enter E Disk : ",strlen("Enter E Disk : "));
	if(n < 0){
		error("Error Writing to socket");
	}
	read(newsockfd,EDisk,255);


	printf("Ten may tinh : %s\n So o dia: 3 \n \t C - %s \n \t D - %s \n \t E - %s \n", Name, CDisk, DDisk, EDisk );


	close(newsockfd);
	close(sockfd);
	return 0;
}