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
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <postgresql/libpq-fe.h>
#include <iostream>

#define PORTNUM 3425
#define BUFSIZE 256
char* askdb(char* word, int tablenum);
int main(int argc, char** argv)
{
	int pid, status, len;
	char buf[BUFSIZE];
	struct sockaddr files;
	files.sa_family = AF_UNIX;
	int sem_id, sem_id2;
	//string dbout;
	struct sembuf SB;
	sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT);
	sem_id2 = semget(IPC_PRIVATE, 1, IPC_CREAT);
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

	unlink("socket.soc");
	printf("First Worker init\n");

	if (!(pid = fork()))
	{
		//char buf[BUFSIZE];
		struct sockaddr_in serv;
		serv.sin_family = AF_INET;

		serv.sin_addr.s_addr = htonl(INADDR_ANY ); //all IPs of PC
		serv.sin_port = htons(PORTNUM);
		printf("Server starts\n");
		//struct sockaddr files;
		//files.sa_family = AF_UNIX;
		strcpy(files.sa_data, "socket1.soc");
		//connection with Worker1
		int sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
		bind(sockidW1, &files,
				(strlen(files.sa_data) + sizeof(files.sa_family)));
		listen(sockidW1, 1);
		int chanW1 = accept(sockidW1, NULL, NULL);
		//connection with Worker2
		strcpy(files.sa_data, "socket2.soc");
		int sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
		bind(sockidW2, &files,
				(strlen(files.sa_data) + sizeof(files.sa_family)));
		listen(sockidW2, 1);
		int chanW2 = accept(sockidW2, NULL, NULL);
		//connection with Worker3
		strcpy(files.sa_data, "socket3.soc");
		int sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
		bind(sockidW3, &files,
				(strlen(files.sa_data) + sizeof(files.sa_family)));
		listen(sockidW3, 1);
		int chanW3 = accept(sockidW3, NULL, NULL);
		//connection with Client
		//!serv.sin_family = AF_INET;
		//!int sockidC = socket(AF_INET, SOCK_STREAM, 0);
		//!bind(sockidC,(struct sockaddr*)  &serv, sizeof(serv));
		//!listen(sockidC, 1);
		//!int chanC = accept(sockidC, NULL, NULL);
		//!recv(chanC, (char*) buf, BUFSIZE, 0);
		strcpy(buf, "dol");        //
		//printf("Waiting for connection...\n");
		write(chanW1, (char*) buf, BUFSIZE);
		write(chanW2, (char*) buf, BUFSIZE);
		write(chanW3, (char*) buf, BUFSIZE);
		char bufout[BUFSIZE * 3] = "";
		strcpy(buf, "");    	//
		read(chanW1, (char*) buf, BUFSIZE);
		strcat(bufout, buf);
		read(chanW2, (char*) buf, BUFSIZE);
		strcat(bufout, buf);
		read(chanW3, (char*) buf, BUFSIZE);
		strcat(bufout, buf);

		//!close(chanC);
		SB.sem_num = 0;
		SB.sem_op = -3;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);
		std::cout << "I am server, your info:\n" << bufout << "\n";
		//!send(chanC, (char*)bufout, BUFSIZE*3, 0);
		close(chanW1);
		unlink("socket1.soc");
		close(chanW2);
		unlink("socket2.soc");
		close(chanW3);
		unlink("socket3.soc");

		exit(0);
	}

	/*SB.sem_num = 0;
	SB.sem_op = 1;
	SB.sem_flg = 0;
	semop(sem_id, &SB, 1);*/

	if (!(pid = fork()))
	{
		//strcpy(files.sa_data, "socket.soc");

		//int sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
		//connect(sockidW2, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
		//recv(sockidW2, (char*) buf, BUFSIZE,0);
		/*SB.sem_num = 0;
		 SB.sem_op = -1;
		 SB.sem_flg = 0;*/
		printf("Worker2 starts searching\n");
		strcpy(files.sa_data, "socket2.soc");
		int sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
		connect(sockidW2, &files,
				(strlen(files.sa_data) + sizeof(files.sa_family)));
		read(sockidW2, (char*) buf, BUFSIZE);
		std::cout << "w2 buf=" << buf << "\n";
		write(sockidW2, askdb(buf, 2), BUFSIZE);
		/*dbout = askdb(argv[1], 2);
		 semop(sem_id, &SB, 1);
		 printf("Worker2\n");
		 std::cout << "table2:\n";
		 std::cout << dbout;
		 SB.sem_num = 0;
		 SB.sem_op = 1;
		 SB.sem_flg = 0;
		 semop(sem_id, &SB, 1);*/

		SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);
		exit(0);
		//send(sockidW2, askdb(buf,2), BUFSIZE,0);
	}

	if (!(pid = fork()))
	{
		//strcpy(files.sa_data, "socket.soc");

		//int sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
		//connect(sockidW3, &files,(strlen(files.sa_data) +	sizeof(files.sa_family)));
		//recv(sockidW3, (char*) buf, BUFSIZE,0);
		/*SB.sem_num = 0;
		 SB.sem_op = -1;
		 SB.sem_flg = 0;*/
		printf("Worker3 starts searching\n");
		strcpy(files.sa_data, "socket3.soc");
		int sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
		connect(sockidW3, &files,
				(strlen(files.sa_data) + sizeof(files.sa_family)));
		read(sockidW3, (char*) buf, BUFSIZE);

		write(sockidW3, askdb(buf, 3), BUFSIZE);

		/*dbout = askdb(argv[1], 3);
		 semop(sem_id, &SB, 1);
		 printf("Worker3\n");
		 std::cout << "table3:\n";
		 std::cout << dbout;
		 SB.sem_num = 0;
		 SB.sem_op = 1;
		 SB.sem_flg = 0;
		 semop(sem_id, &SB, 1);*/

		SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);
		exit(0);
		//send(sockidW3, askdb(buf,3), BUFSIZE,0);
	}

	//strcpy(files.sa_data, "socket.soc");

	//int sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
	//connect(sockidW1, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
	//recv(sockidW1, (char*) buf, BUFSIZE,0);
	/*SB.sem_num = 0;
	 SB.sem_op = -1;
	 SB.sem_flg = 0;*/
	printf("Worker1 starts searching\n");
	strcpy(files.sa_data, "socket1.soc");
	int sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
	connect(sockidW1, &files,
			(strlen(files.sa_data) + sizeof(files.sa_family)));
	read(sockidW1, (char*) buf, BUFSIZE);

	write(sockidW1, askdb(buf, 1), BUFSIZE);

	/*dbout = askdb(argv[1], 1);
	 semop(sem_id, &SB, 1);
	 printf("Worker1\n");
	 std::cout << "table1:\n";
	 std::cout << dbout;
	 SB.sem_num = 0;
	 SB.sem_op = 1;
	 SB.sem_flg = 0;
	 semop(sem_id, &SB, 1);*/

	SB.sem_num = 0;
	SB.sem_op = 1;
	SB.sem_flg = 0;
	semop(sem_id, &SB, 1);
	SB.sem_num = 0;
	SB.sem_op = -1;
	SB.sem_flg = 0;
	semop(sem_id2, &SB, 1);
	//send(sockidW1, askdb(buf,1), BUFSIZE,0);
	//delete[] dbout;

	std::cout << "Enter something to exit..\n";
	getchar();
	return 0;
}
