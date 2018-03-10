#ifndef TEST2DB
#define TEST2DB
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
//#include "askdbfunc.cpp"
using namespace std;
//namespace askdbfunc {char* askdb(char*,int);};
char* askdb(char* word, int tablenum);
int main(int argc, char** argv)
{
	int pid, status, len, sem_id, sem_id2;
	//char* dbout = new char[512];
	string dbout;
	struct sembuf SB;
	if (argc < 2)
	{
		cout << "Enter word as argument for searching\n";
		return 0;
	}

	sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT);
	sem_id2 = semget(IPC_PRIVATE, 1, IPC_CREAT);
	if (sem_id == -1)
	{
		cout << "sem1 error\n";
		return 1;
	}
	else if (sem_id2 == -1)
	{
		cout << "sem2 error\n";
		return 1;
	}
	SB.sem_num = 0;
	SB.sem_op = 1;
	SB.sem_flg = 0;
	semop(sem_id, &SB, 1);
	//int chan[2];
	//char buf[300];
	if (!(pid = fork()))
	{
		//strcpy(files.sa_data, "socket.soc");

		//int sockidW2 = socket(AF_UNIX, SOCK_STREAM, 0);
		//connect(sockidW2, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
		//recv(sockidW2, (char*) buf, BUFSIZE,0);
		SB.sem_num = 0;
		SB.sem_op = -1;
		SB.sem_flg = 0;

		dbout = askdb(argv[1], 2);
		semop(sem_id, &SB, 1);
		printf("Worker2\n");
		std::cout << "table2:\n";
		std::cout << dbout;
		SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);

		SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id2, &SB, 1);
		exit(0);
		//send(sockidW2, askdb(buf,2), BUFSIZE,0);
	}

	if (!(pid = fork()))
	{
		//strcpy(files.sa_data, "socket.soc");

		//int sockidW3 = socket(AF_UNIX, SOCK_STREAM, 0);
		//connect(sockidW3, &files,(strlen(files.sa_data) +	sizeof(files.sa_family)));
		//recv(sockidW3, (char*) buf, BUFSIZE,0);
		SB.sem_num = 0;
		SB.sem_op = -1;
		SB.sem_flg = 0;
		dbout = askdb(argv[1], 3);
		semop(sem_id, &SB, 1);
		printf("Worker3\n");
		std::cout << "table3:\n";
		std::cout << dbout;
		SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);

		SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id2, &SB, 1);
		exit(0);
		//send(sockidW3, askdb(buf,3), BUFSIZE,0);
	}

	//strcpy(files.sa_data, "socket.soc");

	//int sockidW1 = socket(AF_UNIX, SOCK_STREAM, 0);
	//connect(sockidW1, &files, (strlen(files.sa_data) +	sizeof(files.sa_family)));
	//recv(sockidW1, (char*) buf, BUFSIZE,0);
	SB.sem_num = 0;
	SB.sem_op = -1;
	SB.sem_flg = 0;

	dbout = askdb(argv[1], 1);
	semop(sem_id, &SB, 1);
	printf("Worker1\n");
	std::cout << "table1:\n";
	std::cout << dbout;
	SB.sem_num = 0;
	SB.sem_op = 1;
	SB.sem_flg = 0;
	semop(sem_id, &SB, 1);

	SB.sem_num = 0;
	SB.sem_op = -2;
	SB.sem_flg = 0;
	semop(sem_id2, &SB, 1);
	//send(sockidW1, askdb(buf,1), BUFSIZE,0);
	//delete[] dbout;
	return 0;
}

#endif
