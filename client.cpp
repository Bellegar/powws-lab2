//***Client for sending to server keyword for searching date in certain db and receiving results
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <iostream>

#define PORTNUM 3425
#define BUFSIZE 256

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		std::cout << "need IP adress as 1 argument and WORD as 2 argument\n";
		return 1;
	}
	int sockid;
	struct sockaddr_in cli;
	char result[BUFSIZE * 3];
	sockid = socket(AF_INET, SOCK_STREAM, 0);
	cli.sin_family = AF_INET;
	cli.sin_port = htons(PORTNUM);
	inet_aton(argv[1], &cli.sin_addr);
	if (connect(sockid, (sockaddr*) &cli, sizeof(cli)) < 0)
	{
		std::cout << "connect error\n";
		return 1;
	}

	if (send(sockid, argv[2], BUFSIZE, 0) < 0)
	{
		std::cout << "send error\n";
		return 1;
	}
	if (recv(sockid, result, BUFSIZE * 3, 0) < 0)
	{
		std::cout << "recv error\n";
		return 1;
	}
	std::cout << "Searching result:\n" << result << "\n";
	close(sockid);
	return 0;
}
