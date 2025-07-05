#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

#define report(func, err) fprintf(stderr, "%s:%d (%s): %s\n", __FILE__, \
		__LINE__, func, err)
#define trace(func) fprintf(stderr, "%s:%d (%s)\n", __FILE__, \
		__LINE__, func)


int
setup_listening(int *sockfd, struct addrinfo **info)
{
	int tmp = 1;
	int status;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, "8080", &hints, info);

	if (status != 0) {
		report(__func__, gai_strerror(status));
		return EXIT_FAILURE;
	}

	*sockfd = socket(hints.ai_family, hints.ai_socktype, 0);

	if (*sockfd == -1) {
		report(__func__, strerror(errno));
		return EXIT_FAILURE;
	}

	status = setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &tmp,
			sizeof tmp);

	if (status == -1) {
		report(__func__, strerror(errno));
		return EXIT_FAILURE;
	}

	status = bind(*sockfd, (*info)->ai_addr, (*info)->ai_addrlen);

	if (status != 0) {
		report(__func__, strerror(errno));
		return EXIT_FAILURE;
	}

	status = listen(*sockfd, 20);

	if (status != 0) {
		report(__func__, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int
accept_connection(int sockfd, struct sockaddr_storage *peer_address,
		socklen_t *peer_addrlen, int *peer_sockfd)
{
	*peer_addrlen = sizeof(*peer_address);
	*peer_sockfd = accept(sockfd, (struct sockaddr *)peer_address,
			peer_addrlen);

	if (*peer_sockfd == -1) {
		report(__func__, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int
main(void)
{
	int status;
	int sockfd;
	struct addrinfo *info;

	status = setup_listening(&sockfd, &info);

	if (status == EXIT_FAILURE) {
		trace(__func__);
		return EXIT_FAILURE;
	}

	struct sockaddr_storage peer_address;
	socklen_t peer_addrlen;
	int peer_sockfd;

	while (1) {
		status = accept_connection(sockfd, &peer_address,
				&peer_addrlen, &peer_sockfd);

		if (status == EXIT_FAILURE) {
			trace(__func__);
			return EXIT_FAILURE;
		}

		char m[10000];
		recv(peer_sockfd, m, 10000, 0); 

		close(peer_sockfd);
	}

	close(sockfd);
	freeaddrinfo(info);

	return 0;
}
