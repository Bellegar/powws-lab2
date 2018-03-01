/*
 * server.cpp
 *
 *  Created on: 28.02.2018
 *      Author: bellegar
 */
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
//#include <sys/un.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <postgresql/libpq-fe.h>
#include <iostream>

#define PORTNUM 3425
#define BUFSIZE 256
char* askdb(char* word, int tablenum);
int main(int argc, char** argv)
{
    int pid,status,len;
    //char buf[BUFSIZE];
    struct sockaddr files;
    files.sa_family = AF_UNIX;
    //strcpy(files.sa_data, "socket.soc");
    unlink("socket.soc");
    printf("First Worker init\n");
    if (!(pid = fork()))
    {
    	char buf[BUFSIZE];
    	struct sockaddr_in serv;
    	serv.sin_family = AF_INET;

    	serv.sin_addr.s_addr = htonl(INADDR_ANY); //all IPs of PC
    	serv.sin_port = htons(PORTNUM);
    	printf("Server\n");
    	//struct sockaddr files;
    	//files.sa_family = AF_UNIX;
    	strcpy(files.sa_data, "socket1.soc");
    	//connection with Worker1
    	int sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
    	bind(sockidW1, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
    	listen(sockidW1, 1);
    	int chanW1 = accept(sockidW1, NULL, NULL);
    	//connection with Worker2
    	strcpy(files.sa_data, "socket2.soc");
    	int sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
       	bind(sockidW2, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
       	listen(sockidW2, 1);
       	int chanW2 = accept(sockidW2, NULL, NULL);
       	//connection with Worker3
       	strcpy(files.sa_data, "socket3.soc");
       	int sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
        bind(sockidW3, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
      	listen(sockidW3, 1);
        int chanW3 = accept(sockidW3, NULL, NULL);
        //connection with Client
        //!serv.sin_family = AF_INET;
        //!int sockidC = socket(AF_INET, SOCK_STREAM, 0);
        //!bind(sockidC,(struct sockaddr*)  &serv, sizeof(serv));
        //!listen(sockidC, 1);
        //!int chanC = accept(sockidC, NULL, NULL);
        //!recv(chanC, (char*) buf, BUFSIZE, 0);
        strcpy(buf,"dol");//
    	//printf("Waiting for connection...\n");
    	write(chanW1, (char*)buf, BUFSIZE);
    	write(chanW2, (char*)buf, BUFSIZE);
    	write(chanW3, (char*)buf, BUFSIZE);
    	char bufout[BUFSIZE*3] = "";
    	strcpy(buf,"");//
    	read(chanW1, (char*) buf, BUFSIZE);
    	strcat(bufout,buf);
    	read(chanW2, (char*) buf, BUFSIZE);
    	strcat(bufout,buf);
    	read(chanW3, (char*) buf, BUFSIZE);
    	strcat(bufout,buf);
    	std::cout << "I am server, your info:\n" << bufout<< "\n";
    	//!send(chanC, (char*)bufout, BUFSIZE*3, 0);
    	close(chanW1);
    	unlink("socket1.soc");
    	close(chanW2);
    	unlink("socket2.soc");
    	close(chanW3);
    	unlink("socket3.soc");
    	//!close(chanC);
    	kill(getpid(), SIGTERM);
    }
    else
    {
    	if (!(pid = fork()))
    	{
    		char buf[BUFSIZE];
    		//strcpy(files.sa_data, "socket.soc");
    		printf("Worker2 init and searching\n");
    		strcpy(files.sa_data, "socket2.soc");
    		int sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
    		connect(sockidW2, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
    		read(sockidW2, (char*) buf, BUFSIZE);
    		std::cout<<"w2 buf="<<buf<<"\n";
    		write(sockidW2, askdb(buf,2), BUFSIZE);
    		std::cout<<"w2ask\n"<<askdb(buf,2)<<"\n";
    		kill(getpid(), SIGTERM);
    	}
    	else
    	{
    		if (!(pid = fork()))
    		{
    			char buf[BUFSIZE];
    			//strcpy(files.sa_data, "socket.soc");
        		printf("Worker3 init and searching\n");
        		strcpy(files.sa_data, "socket3.soc");
        		int sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
        		connect(sockidW3, &files,(strlen(files.sa_data) +	sizeof(files.sa_family)));
        		read(sockidW3, (char*) buf, BUFSIZE);

        		write(sockidW3, askdb(buf,3), BUFSIZE);
        		kill(getpid(), SIGTERM);
    		}
    		else
    		{
    			char buf[BUFSIZE];
    			//strcpy(files.sa_data, "socket.soc");
    	    	printf("Worker1 is searching\n");
    	    	strcpy(files.sa_data, "socket1.soc");
    	    	int sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
    	    	connect(sockidW1, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
    	    	read(sockidW1, (char*) buf, BUFSIZE);

        		write(sockidW1, askdb(buf,1), BUFSIZE);
    		}
    	}
    }
    std::cout << "Enter something to exit..";
    getchar();
    return 0;
}
