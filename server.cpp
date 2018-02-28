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
    //int chan[2];
    char buf[BUFSIZE];
    //char fname[20];
    //FILE *output;

    //struct sockaddr_in dest;
    //PGconn *conn;
    //PGresult *res;
    //int rec_count, row, col;

    struct sockaddr files;
    //int mysocket;
    //socklen_t socksize = sizeof(struct sockaddr_in);
    files.sa_family = AF_UNIX;

    //serv.sin_family = family;
    //serv.sin_addr.s_addr = INADDR_ANY; //all IPs of PC
    //serv.sin_port = htons(PORTNUM);
    printf("First Worker\n");
    if (!(pid = fork()))
    {
    	struct sockaddr_in serv;
    	serv.sin_family = AF_INET;
    	serv.sin_addr.s_addr = htonl(INADDR_ANY); //all IPs of PC
    	serv.sin_port = htons(PORTNUM);
    	printf("Server\n");
    	//connection with Worker1
    	int sockidW1 = socket(AF_INET, SOCK_STREAM, 0);
    	bind(sockidW1,(struct sockaddr*) &serv, sizeof(serv));
    	listen(sockidW1, 1);
    	int chanW1 = accept(sockidW1, NULL, NULL);
    	//connection with Worker2
    	int sockidW2 = socket(AF_INET, SOCK_STREAM, 0);
       	bind(sockidW2,(struct sockaddr*)  &serv, sizeof(serv));
       	listen(sockidW2, 1);
       	int chanW2 = accept(sockidW2, NULL, NULL);
       	//connection with Worker3
       	int sockidW3 = socket(AF_INET, SOCK_STREAM, 0);
        bind(sockidW3,(struct sockaddr*)  &serv, sizeof(serv));
      	listen(sockidW3, 1);
        int chanW3 = accept(sockidW3, NULL, NULL);
        //connection with Client
        serv.sin_family = AF_INET;
        int sockidC = socket(AF_INET, SOCK_STREAM, 0);
        bind(sockidC,(struct sockaddr*)  &serv, sizeof(serv));
        listen(sockidC, 1);
        int chanC = accept(sockidC, NULL, NULL);
        recv(chanC, (char*) buf, BUFSIZE, 0);

    	//printf("Waiting for connection...\n");
    	send(chanW1, (char*)buf, BUFSIZE, 0);
    	send(chanW2, (char*)buf, BUFSIZE, 0);
    	send(chanW3, (char*)buf, BUFSIZE, 0);
    	char bufout[BUFSIZE*3] = "";
    	recv(chanW1, (char*) buf, BUFSIZE, 0);
    	strcat(bufout,buf);
    	recv(chanW2, (char*) buf, BUFSIZE, 0);
    	strcat(bufout,buf);
    	recv(chanW3, (char*) buf, BUFSIZE, 0);
    	strcat(bufout,buf);
    	send(chanC, (char*)bufout, BUFSIZE*3, 0);
    	close(chanW1);
    	close(chanW2);
    	close(chanW3);
    	close(chanC);
    }
    else
    {
    	if (!(pid = fork()))
    	{
    		//files.sa_data="socet.cos";

    		strcpy(files.sa_data, "socket.soc");
    		printf("Worker2\n");
    		int sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
    		connect(sockidW2, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
    		recv(sockidW2, (char*) buf, BUFSIZE,0);

    		send(sockidW2, askdb(buf,2), BUFSIZE,0);

    	}
    	else
    	{
    		if (!(pid = fork()))
    		{
    			strcpy(files.sa_data, "socket.soc");
        		printf("Worker3\n");
        		int sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
        		connect(sockidW3, &files,(strlen(files.sa_data) +	sizeof(files.sa_family)));
        		recv(sockidW3, (char*) buf, BUFSIZE,0);

        		send(sockidW3, askdb(buf,3), BUFSIZE,0);
    		}
    		else
    		{
    			strcpy(files.sa_data, "socket.soc");
    	    	printf("Worker1\n");
    	    	int sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
    	    	connect(sockidW1, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
    	    	recv(sockidW1, (char*) buf, BUFSIZE,0);

        		send(sockidW1, askdb(buf,1), BUFSIZE,0);
    		}
    	}
    }
    std::cout << "vvedi chtoto";
    std::cin >> row;;
    return 0;
}

char* askdb(char* word, int tablenum)
{
		PGconn *conn;
	    PGresult *res;
		char out[300] ="";
		int col, row;
    	conn = PQconnectdb("dbname=bellegar host=localhost user=bellegar password=bugboom");
    	if (conn == NULL)
    	{
    		strcpy(out, "no connection to bellegar DB\n");
    		return out;
    	}
    	//std::cout << PQdb(conn);
    	string namecol;
    	switch (tablenum)
    	{
    	case 1:
    		namecol = "name";
    		break;
    	case 2:
    		namecol = "type";
    		break;
    	case 3:
    		namecol = "book";
    		break;
    	}
    	string comm = "select * from table" + itoa(tablenum) + " where " + string(namecol)
    			+ " like '%" + string(word) + "%';";
    	res = PQexec(conn, comm.c_str());
    	row = PQntuples(res);
    	col = PQnfields(res);

	for (int icol = 0; icol < col; icol++)
	{
			strcat(out,PQfname(res,icol));
			strcat(out,"\t");
	}
	strcat(out,"\n");
	for (int irow=0; irow<row; irow++)
	{
		for (int icol=0; icol < col; icol++)
		{
			strcat(out, PQgetvalue(res, irow, icol));
			strcat(out,"\t");
		}
	strcat(out,"\n");
	}
}
