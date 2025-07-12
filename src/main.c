#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include "shwc.h"

static int
setup_listening(int *sockfd)
{
	int tmp = 1;
	int status;
	struct addrinfo hints;
	struct addrinfo *info;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, "8080", &hints, &info);

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

	status = bind(*sockfd, info->ai_addr, info->ai_addrlen);

	if (status != 0) {
		report(__func__, strerror(errno));
		return EXIT_FAILURE;
	}

	status = listen(*sockfd, 20);

	if (status != 0) {
		report(__func__, strerror(errno));
		return EXIT_FAILURE;
	}

	freeaddrinfo(info);
	return EXIT_SUCCESS;
}

static int
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

struct request {
	char protocol[10];
	char method[8];
	char *url;
};

static int
process_request(char msg[], size_t msglen, struct request *req)
{
}

static int
handle_request(int peer_sockfd)
{
	int status;
	char msg[1024];
	size_t msglen;
	struct request req;

	status = recv(peer_sockfd, msg, 1024, 0);

	if (status == -1) {
		report(__func__, strerr(errno));
		return EXIT_FAILURE;
	}

	msglen = strlen(msg);

	process_request(msg, msglen, &req);

	return EXIT_SUCCESS;
}

int
main(void)
{
	int status;
	int sockfd;

	status = setup_listening(&sockfd);

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

		handle_request(peer_sockfd);

		close(peer_sockfd);
	}

	close(sockfd);

	return 0;
}
