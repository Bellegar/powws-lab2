#ifndef TEST2DB
#define TEST2DB
#include <unistd.h>
#include <sys/types.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
//#include "askdbfunc.cpp"
using namespace std;
//namespace askdbfunc {char* askdb(char*,int);};
char* askdb(char* word, int tablenum);
int main(int argc, char** argv)
{
    int pid,status,len;
    //int chan[2];
    //char buf[300];
    if (!(pid = fork()))
    	{
    		//strcpy(files.sa_data, "socket.soc");
    		printf("Worker2\n");
    		//int sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
    		//connect(sockidW2, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
    		//recv(sockidW2, (char*) buf, BUFSIZE,0);
                std::cout << askdb(argv[1],2);
    		//send(sockidW2, askdb(buf,2), BUFSIZE,0);

    	}
    	else
    	{
    		if (!(pid = fork()))
    		{
    			//strcpy(files.sa_data, "socket.soc");
        		printf("Worker3\n");
        		//int sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
        		//connect(sockidW3, &files,(strlen(files.sa_data) +	sizeof(files.sa_family)));
        		//recv(sockidW3, (char*) buf, BUFSIZE,0);
                std::cout << askdb(argv[1],3);
        		//send(sockidW3, askdb(buf,3), BUFSIZE,0);
    		}
    		else
    		{
    			//strcpy(files.sa_data, "socket.soc");
    	    	printf("Worker1\n");
    	    	//int sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
    	    	//connect(sockidW1, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
    	    	//recv(sockidW1, (char*) buf, BUFSIZE,0);
                std::cout << askdb(argv[1],1);
        		//send(sockidW1, askdb(buf,1), BUFSIZE,0);
    		}
    	}
}

#endif
