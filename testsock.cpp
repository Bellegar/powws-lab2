#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
using namespace std;

int main()
{
	int server_sockfd;
	int client_sockfd;
	int server_len;
	int client_len, pid;
	sockaddr_un server_address;
	sockaddr_un client_address;
	int sem_id;
	struct sembuf SB;
	sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT);

	unlink("server_socket");
	server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	server_address.sun_family = AF_UNIX;
	strcpy(server_address.sun_path, "server_socket");
	server_len = sizeof(server_address);

	if ((pid = fork()) == 0)
	{
		int sockfd;
		int len;
		sockaddr_un address;
		int result;
		char ch[100];

		sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

		address.sun_family = AF_UNIX;
		strcpy(address.sun_path, "server_socket");
		len = sizeof(address);
		SB.sem_num = 0;
		SB.sem_op = -1;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);
		result = connect(sockfd, (sockaddr*) &address, len);

		if (result == -1)
		{
			perror("no connect in client1/n");
			exit(1);
		}

		read(sockfd, ch, sizeof(ch));
		strcat(ch, "lolo");
		write(sockfd, ch, sizeof(ch));
		//printf("char from server = %c\n", ch);
		close(sockfd);
		exit(0);

	}

	if (bind(server_sockfd, (sockaddr*) &server_address, server_len) < 0)
	{
		std::cout << "bind error\n";
		return 1;
	}

	if (listen(server_sockfd, 5) < 0)
	{
		std::cout << "listen error\n";
		return 1;
	}

	char ch[100];
	strcpy(ch, "popoh");
	// printf("server waiting\n");

	client_sockfd = accept(server_sockfd, (sockaddr*) &client_address,
			(socklen_t*) &client_len);
	SB.sem_num = 0;
		SB.sem_op = 1;
		SB.sem_flg = 0;
		semop(sem_id, &SB, 1);
	if (client_sockfd < 0)
	{
		std::cout << "accept error\n";
		return 1;
	}
	write(client_sockfd, ch, sizeof(ch));
	read(client_sockfd, ch, sizeof(ch));
	std::cout << ch << "\n";
	close(client_sockfd);

}
