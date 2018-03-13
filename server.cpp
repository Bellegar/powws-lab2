/*
 * server.cpp
 *
 *  Created on: 28.02.2018
 *      Author: bellegar
 */
 //***Server for searching data in certain tables in db by keyword from client
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

#define PORTNUM 3425
#define BUFSIZE 256
void askdb(char* word, int tablenum, char* output);
int main(int argc, char** argv)
{
	//first Worker is parent
	int pid, status, len, rer;
	char buf[BUFSIZE];
	int sockidW1, sockidW2, sockidW3;
	int SAsize, CLsize;
	sockaddr_un files;
	sockaddr_un clfiles;
	files.sun_family = AF_UNIX;
	int sem_id, sem_id2;
	struct sembuf SB;
	sem_id = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	sem_id2 = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (sem_id == -1)
	{
		std::cout << "sem1 error\n";
		return 1;
	}
	else if (sem_id2 == -1)
	{
		std::cout << "sem2 error\n";
		return 1;
	}
	//server forks
	if (!(pid = fork()))
	{
		struct sockaddr_in serv;
		int sockINT, intcon;
		std::cout << "Server starts\n";
		serv.sin_family = AF_INET;
		serv.sin_addr.s_addr = htonl(INADDR_ANY ); //all IPs of PC
		serv.sin_port = htons(PORTNUM);
		sockINT = socket(AF_INET, SOCK_STREAM, 0);
		//==================connection with Client
		int enable = 1;
		//---if ip/port is already in use, use it
		if (setsockopt(sockINT, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		{
		    std::cout<<"setsockopt(SO_REUSEADDR) failed\n";
		    return 1;
		}
		if (bind(sockINT, (sockaddr*) &serv, sizeof(serv)) < 0)
		{
			std::cout << "interbind error\n";
			return 1;
		}
		if (listen(sockINT, 2) <0)
		{
			std::cout << "interlisten error\n";
			return 1;
		}
		intcon = accept(sockINT, NULL, NULL);
		if (intcon < 0)
		{
			std::cout << "interaccept error\n";
			return 1;
		}
		//---receiving part of string from client for searching
		recv(intcon, buf, BUFSIZE, 0);
		std::cout << "from client: " << buf << "\n";
		unlink("server_socket");
		unlink("server_socket2");
		unlink("server_socket3");
		sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
		files.sun_family = AF_UNIX;
		//==================connection with Worker1
		strcpy(files.sun_path, "server_socket");
		SAsize = sizeof(files);
		if (bind(sockidW1, (sockaddr*) &files, SAsize) < 0)
		{
			std::cout << "bind1 error\n";
			return 1;
		}
		if (listen(sockidW1, 5) < 0)
		{
			std::cout << "listen1 error\n";
			return 1;
		}
		SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id2, &SB, 1);
		int chanW1 = accept(sockidW1, NULL, NULL);
		if (chanW1 < 0)
		{
			std::cout << "accept1 error\n";
			return 1;
		}
		if (send(chanW1, buf, sizeof(buf), 0) < 0)
		{
			std::cout << "send1 error\n";
			return 1;
		}
		//==================connection with Worker2
		strcpy(files.sun_path, "server_socket2");
		SAsize = sizeof(files);
		int sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
		if (bind(sockidW2, (sockaddr*) &files, SAsize) < 0)
		{
			std::cout << "bind2 error\n";
			return 1;
		}
		if (listen(sockidW2, 5) < 0)
		{
			std::cout << "listen2 error\n";
			return 1;
		}
		SB.sem_num = 0;
		SB.sem_op = 2;
		SB.sem_flg = 0;
		semop(sem_id2, &SB, 1);
		int chanW2 = accept(sockidW2, NULL, NULL);
		if (chanW2 < 0)
		{
			std::cout << "accept2 error\n";
			return 1;
		}
		send(chanW2, buf, sizeof(buf), 0);
		//================connection with Worker3
		strcpy(files.sun_path, "server_socket3");
		SAsize = sizeof(files);
		int sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
		if (bind(sockidW3, (sockaddr*) &files, SAsize) < 0)
		{
			std::cout << "bind3 error\n";
			return 1;
		}
		if (listen(sockidW3, 3) < 0)
		{
			std::cout << "listen3 error\n";
			return 1;
		}
		SB.sem_num = 0;
		SB.sem_op = 4;
		SB.sem_flg = 0;
		semop(sem_id2, &SB, 1);
		int chanW3 = accept(sockidW3, NULL, NULL);
		if (chanW3 < 0)
		{
			std::cout << "accept3 error\n";
			return 1;
		}
		send(chanW3, buf, sizeof(buf), 0);
		//=================Receiving results
		char bufout[BUFSIZE * 3] = "";
		strcpy(buf, "");    	
		char bfrecv[BUFSIZE];

		if (recv(chanW1, bfrecv, BUFSIZE, 0) < 0)
		{
			std::cout << "recv1 error \n";
			return 1;
		}
		std::cout << "W1 recv serv " << bfrecv << "\n";
		strcat(bufout, bfrecv);
		strcpy(bfrecv, "");
		if (recv(chanW2, bfrecv, BUFSIZE, 0) < 0)
		{
			std::cout << "recv2 error \n";
			return 1;
		}
		std::cout << "W2 recv serv " << bfrecv << "\n";
		strcat(bufout, bfrecv);
		strcpy(bfrecv, "");
		if (recv(chanW3, bfrecv, BUFSIZE, 0) < 0)
		{
			std::cout << "recv3 error \n";
			return 1;
		}
		std::cout << "W3 recv serv " << bfrecv << "\n";
		strcat(bufout, bfrecv);

		SB.sem_num = 0;
		SB.sem_op = -3;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);
		//---sending search results to client
		send(intcon, bufout, BUFSIZE * 3, 0);
		std::cout << "I am server, client info:\n" << bufout << "\n";
		close(sockidW1);
		close(sockidW2);
		close(sockidW2);
		close(intcon);
		exit(0);
	}
	//====================Worker2 forks
	if (!(pid = fork()))
	{
		strcpy(buf, "");
		char askbuf[BUFSIZE];
		clfiles.sun_family = AF_UNIX;
		strcpy(clfiles.sun_path, "server_socket2");
		CLsize = sizeof(clfiles);
		SB.sem_num = 0;
		SB.sem_op = -2;
		SB.sem_flg = 0;
		semop(sem_id2, &SB, 1);
		std::cout<<"Worker2 starts searching\n";
		sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
		rer = connect(sockidW2, (sockaddr*) &clfiles, CLsize);
		if (rer < 0)
		{
			std::cout << "connectW2 error\n";
			return 1;
		}
		recv(sockidW2, buf, BUFSIZE, 0);
		std::cout << "w2 buf=" << buf << "\n";
		//---searching in table2 in db
		askdb(buf, 2, askbuf);
		std::cout << "askbuf2 " << askbuf << "\n";
		send(sockidW2, askbuf, strlen(askbuf), 0);
		SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);
		exit(0);
	}
	//====================Worker3 forks
	if (!(pid = fork()))
	{
		strcpy(buf, "");
		char askbuf[BUFSIZE];
		clfiles.sun_family = AF_UNIX;
		strcpy(clfiles.sun_path, "server_socket3");
		CLsize = sizeof(clfiles);
		SB.sem_num = 0;
		SB.sem_op = -4;
		SB.sem_flg = 0;
		semop(sem_id2, &SB, 1);
		std::cout << "Worker3 starts searching\n";
		sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
		rer = connect(sockidW3, (sockaddr*) &clfiles, CLsize);
		if (rer < 0)
		{
			std::cout << "connectW3 error\n";
			return 1;
		}
		recv(sockidW3, buf, BUFSIZE, 0);
		std::cout << "w3 buf=" << buf << "\n";
		//---searching in table3 in db
		askdb(buf, 3, askbuf);
		std::cout << "askbuf3 " << askbuf << "\n";
		send(sockidW3, askbuf, strlen(askbuf), 0);
		SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);
		exit(0);
	}
	//====================Worker1 
	strcpy(buf, "");
	char askbuf[BUFSIZE];
	clfiles.sun_family = AF_UNIX;
	strcpy(clfiles.sun_path, "server_socket");
	CLsize = sizeof(clfiles);
	SB.sem_num = 0;
	SB.sem_op = -1;
	SB.sem_flg = 0;
	semop(sem_id2, &SB, 1);
	std::cout << "Worker1 starts searching\n";
	sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
	rer = connect(sockidW1, (sockaddr*) &clfiles, CLsize);
	if (rer < 0)
	{
		std::cout << "connectW1 error\n";
		return 1;
	}
	recv(sockidW1, buf, BUFSIZE, 0);
	std::cout << "w1 buf=" << buf << "\n";
	askdb(buf, 1, askbuf);
	std::cout << "askbuf1 " << askbuf << "\n";
	send(sockidW1, &askbuf, strlen(askbuf), 0);
	SB.sem_num = 0;
	SB.sem_op = 1;
	SB.sem_flg = 0;
	semop(sem_id, &SB, 1);
	return 0;
}
