/*
 * server.cpp
 *
 *  Created on: 28.02.2018
 *      Author: bellegar
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
//#include <postgresql/libpq-fe.h>
#include <iostream>

#define PORTNUM 3425
#define BUFSIZE 256
void askdb(char* word, int tablenum, char* output);
int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "need IP adress as argument\n";
		return 1;
	}
	int pid, status, len, rer;
	char buf[BUFSIZE];
	std::string bufstr;
	//struct sockaddr files;
	int sockidW1, sockidW2, sockidW3;
	int SAsize, CLsize;
	sockaddr_un files;
	sockaddr_un clfiles;
	files.sun_family = AF_UNIX;

	int sem_id, sem_id2;
	//string dbout;
	struct sembuf SB;
	sem_id = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
	sem_id2 = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
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
	//std::cout << "semval2 " << semctl(sem_id2, 0, GETVAL) << "\n";
	//std::cout << "semval1 " << semctl(sem_id, 0, GETVAL) << "\n";
	if (!(pid = fork()))
	{
		struct sockaddr_in serv;
		int sockINT, intcon;
		printf("Server starts\n");
		serv.sin_family = AF_INET;
		serv.sin_addr.s_addr = htonl(INADDR_ANY ); //all IPs of PC
		serv.sin_port = htons(PORTNUM);
		sockINT = socket(AF_INET, SOCK_STREAM, 0);
		//inet_aton(argv[1], serv.sin_addr);
		bind(sockINT, (sockaddr*) &serv, sizeof(serv));
		listen(sockINT, 2);
		intcon = accept(sockINT, NULL, NULL);
		recv(intcon, buf, BUFSIZE, 0);
		std::cout << "from client " << buf << "\n";

		unlink("server_socket");
		unlink("server_socket2");
		unlink("server_socket3");
		sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
		//strcpy(buf, "dol");
		files.sun_family = AF_UNIX;
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
		//std::cout << "Serv +1 " << semctl(sem_id2, 0, GETVAL) << "\n";
		int chanW1 = accept(sockidW1, (sockaddr*) &clfiles,
				(socklen_t*) &CLsize);
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
		//std::cout << "Serv +2 " << semctl(sem_id2, 0, GETVAL) << "\n";
		int chanW2 = accept(sockidW2, (sockaddr*) &clfiles,
				(socklen_t*) &CLsize);
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
		//std::cout << "Serv +4 " << semctl(sem_id2, 0, GETVAL) << "\n";
		int chanW3 = accept(sockidW3, (sockaddr*) &clfiles,
				(socklen_t*) &CLsize);
		if (chanW3 < 0)
		{
			std::cout << "accept3 error\n";
			return 1;
		}
		send(chanW3, buf, sizeof(buf), 0);

		//=================Receiving results
		char bufout[BUFSIZE * 3] = "";
		strcpy(buf, "");    	//
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
		send(intcon, bufout, BUFSIZE * 3, 0);
		std::cout << "I am server, your info:\n" << bufout << "\n";
		close(sockidW1);
		close(sockidW2);
		close(sockidW2);
		close(intcon);
		exit(0);
	}

	//====================Worker2
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
		printf("Worker2 starts searching\n");
		//std::cout << "W2 -2 " << semctl(sem_id2, 0, GETVAL) << "\n";
		sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
		rer = connect(sockidW2, (sockaddr*) &clfiles, CLsize);
		if (rer < 0)
		{
			std::cout << "connectW2 error\n";
			return 1;
		}

		recv(sockidW2, buf, BUFSIZE, 0);
		std::cout << "w2 buf=" << buf << "\n";

		//char* bf2;
		askdb(buf, 2, askbuf);
		std::cout << "askbuf2 " << askbuf << "\n";
		send(sockidW2, askbuf, strlen(askbuf), 0);

		SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);
		exit(0);
	}
	//====================Worker3
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
		printf("Worker3 starts searching\n");
		//std::cout << "W3 -4 " << semctl(sem_id2, 0, GETVAL) << "\n";
		sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
		rer = connect(sockidW3, (sockaddr*) &clfiles, CLsize);
		if (rer < 0)
		{
			std::cout << "connectW3 error\n";
			return 1;
		}
		recv(sockidW3, buf, BUFSIZE, 0);
		std::cout << "w3 buf=" << buf << "\n";
		askdb(buf, 3, askbuf);
		std::cout << "askbuf3 " << askbuf << "\n";
		//std::cout << "askbuf3 len " << strlen(askbuf) << "\n";
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

	//strcpy(files.sa_data, "socket1.soc");
	SB.sem_num = 0;
	SB.sem_op = -1;
	SB.sem_flg = 0;
	semop(sem_id2, &SB, 1);
	printf("Worker1 starts searching\n");
	//std::cout << "W1 -1 " << semctl(sem_id2, 0, GETVAL) << "\n";
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
	//char bf2[300];
	//strcpy(bf2,bf3);
	std::cout << "askbuf1 " << askbuf << "\n";
	//int sbf = strlen(askbuf);
	//std::cout << "bf1size " << sbf << "\n";
	//send(sockidW1, &sbf, sizeof(int), 0);
	send(sockidW1, &askbuf, strlen(askbuf), 0);

	SB.sem_num = 0;
	SB.sem_op = 1;
	SB.sem_flg = 0;
	semop(sem_id, &SB, 1);
	//std::cout << "Enter something to exit..\n";
	//getchar();
	return 0;
}
