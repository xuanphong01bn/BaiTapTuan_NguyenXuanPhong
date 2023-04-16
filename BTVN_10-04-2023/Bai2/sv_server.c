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
	//MSSV
	char MSSV[255];
	n = write(newsockfd,"Enter MSSV : ",strlen("Enter MSSV : "));
	if(n < 0){
		error("Error Writing to socket");
	}
	read(newsockfd,MSSV,255);

	//hoten
	char hoten[255];
	n = write(newsockfd,"Enter hoten : ",strlen("Enter hoten : "));
	if(n < 0){
		error("Error Writing to socket");
	}
	read(newsockfd,hoten,255);

	//ngaysinh
	char ngaysinh[255];
	n = write(newsockfd,"Enter ngaysinh : ",strlen("Enter ngaysinh : "));
	if(n < 0){
		error("Error Writing to socket");
	}
	read(newsockfd,ngaysinh,255);

	//GPA
	char GPA[255];
	n = write(newsockfd,"Enter GPA : ",strlen("Enter GPA : "));
	if(n < 0){
		error("Error Writing to socket");
	}
	read(newsockfd,GPA,255);

	char IP[255];
	n = write(newsockfd,"          ",strlen("Enter IP : "));
	if(n < 0){
		error("Error Writing to socket");
	}
	read(newsockfd,IP,255);

	char time_now[255];
	n = write(newsockfd,"           ",strlen("Enter IP : "));
	if(n < 0){
		error("Error Writing to socket");
	}
	read(newsockfd,time_now,255);

	printf("Thong tin sinh vien :\nMSSV: %s\nHo va Ten: %s\nNgay sinh: %s\nGPA: %s\n", MSSV, hoten, ngaysinh, GPA);

	FILE *flog;
	flog=fopen(argv[2],"w");

	fprintf(flog, "%s %s %s %s %s %s",IP, time_now, MSSV, hoten, ngaysinh, GPA);

	close(newsockfd);
	close(sockfd);
	return 0;
}