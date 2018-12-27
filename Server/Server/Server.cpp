#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

int make_socket(SOCKET *sock) {

	*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*sock == INVALID_SOCKET)
	{
		printf("Error at socket().\n");
		WSACleanup();
		return 1;
	}
	else
	{
		return 0;
		printf("\nSocket is succesfully created.\n");
	}
}
void make_server(struct sockaddr_in *server) {
	server->sin_addr.s_addr = INADDR_ANY;
	server->sin_family = AF_INET;
	server->sin_port = htons(DEFAULT_PORT);
}
int make_bind(SOCKET sock, struct sockaddr_in server) {
	if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("Bind failed. Error\n");
		return 1;
	}
	puts("Bind done\n");
	return 0;
}
int recv_msg(SOCKET sock, char message[]) {
	int read_size;
	if (read_size = (recv(sock, message, DEFAULT_BUFLEN, 0)) > 0) {
		puts(message);
		return 0;
	}
	else if (read_size == 0) {
		puts("Client Disconnected\n");
		return 1;
	}
	else {
		puts("Recv failed\n");
		return 1;
	}
}
int send_fun(SOCKET sock, char message[DEFAULT_BUFLEN]) {
	if (send(sock, message, strlen(message), 0) < 0) {
		puts("Send failed\n");
		return 1;
	}
	return 0;
}

FILE *safe_fopen(char *filename, const char *mode, int error_code) {
	FILE *fp = fopen(filename, mode);
	if (fp == NULL) {
		printf("Can't open '%s'!\n", filename);
		exit(error_code);
	}
	return fp;
}

int main(int argc, char *argv[])
{
	// Initialize Winsock, request the Winsock 2.2
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup().\n");
		return 1;
	}
	else
	{
		printf("WSA library is succesfully initilized.\n");
	}

	SOCKET socket_desc, client_sock, c;
	struct sockaddr_in server, client;
	char message[DEFAULT_BUFLEN] = "\0";
	char *out_filename = argv[1];

	make_socket(&socket_desc);

	make_server(&server);

	make_bind(socket_desc, server);

	listen(socket_desc, 3);

	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);

	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");

	FILE *out = safe_fopen(out_filename, "w", 1);

	int fr_block = 0;

	while (fr_block = recv(client_sock, message, DEFAULT_BUFLEN, 0)) {
		if (fr_block < 0) {
			perror("Error receiving file from client to server.\n");
		}
		int write_sz = fwrite(message, sizeof(char), fr_block, out);
		if (write_sz < fr_block) {
			perror("File write failed on server.\n");
		}
	}
	printf("OK received from proxy_server! \n");

	fclose(out);
	closesocket(socket_desc);
	closesocket(client_sock);
	WSACleanup();

	return 0;
}