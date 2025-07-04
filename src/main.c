#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

int
main(void)
{
	int status, sockfd;
	struct addrinfo hints;
	struct addrinfo *info;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, "9012", &hints, &info);

	if (status != 0) {
		fputs("lascou", stderr);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		fputs("lascou", stderr);
		printf("%d", __LINE__);
		exit(1);
	}

	int y = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &y, sizeof y);
	status = bind(sockfd, info->ai_addr, info->ai_addrlen);

	if (status != 0) {
		fputs("lascou", stderr);
		printf("%d", __LINE__);
		perror("fudeu");
	}

	status = listen(sockfd, 20);

	if (status != 0) {
		fputs("lascou", stderr);
		printf("%d", __LINE__);
		exit(1);
	}

	struct sockaddr_storage bagui_deles;
	socklen_t len_deles;
	int fd_dos_cara;

	while (1) {
		perror("coiso\n");
		len_deles = sizeof(bagui_deles);
		fd_dos_cara = accept(sockfd, (struct sockaddr *)&bagui_deles,
			&len_deles);

		if (fd_dos_cara > 0) {
			char m[10000];
			recv(fd_dos_cara, m, 10000, 0); 
			char meusbagui[] =
"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
"<html>\r\n"
"<head><title>Oiiii.</title></head>\r\n"
"<body>\r\n"
"<h1 style='color:red'>Oi</h1>"
"</body>\r\n"
"</html>\r\n";
			send(fd_dos_cara, meusbagui, strlen(meusbagui), 0);
		}

		close(fd_dos_cara);
	}

	close(sockfd);
	freeaddrinfo(info);

	return 0;
}
