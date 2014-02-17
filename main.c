#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFF_SIZE 4096

int connect_to_socket(const char* ip, struct sockaddr_in* dest, int port);
int download_file(int socket, const char* filepath);
int rem_headers(char *buffer);

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		puts("usage: ./download <url> <filepath>");
		return 1;
	}

	struct hostent *h = gethostbyname(argv[1]);
	char* ip = inet_ntoa(*((struct in_addr*)h->h_addr));

	struct sockaddr_in dest;
	int my_socket;
	if ((my_socket = connect_to_socket(ip, &dest, 80)) == -1)
	{
		perror("couldn't connect. ");
		return 1;
	}
	puts("connected.");

	if (download_file(my_socket, argv[2]))
	{
		perror("unable to download");
		return 1;
	}
	puts("download completed");
	return 0;
}
	

int connect_to_socket(const char* ip, struct sockaddr_in* dest, int port)
{
	int my_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (my_socket < 0)
	{
		perror("socket allocation failed");
		return -1;
	}
	memset (dest, 0, sizeof(dest));
	dest->sin_family = AF_INET;
	dest->sin_port = htons(port);
	dest->sin_addr.s_addr = inet_addr(ip);

	if (connect(my_socket, (struct sockaddr*)dest, (socklen_t)sizeof(*dest)) == -1)
		return -1;
	return my_socket;
}

int download_file(int socket, const char* filepath)
{
	char buff[200];
	strcpy(buff, "GET /");
	strcat(buff, filepath);
	strcat(buff, " HTTP/1.0\n\n");
	char* bu = malloc(sizeof(char)*strlen(buff)+1);
	strcat(bu, buff);
	
	if (send(socket, bu, strlen(bu), 0) < 1)
	{
		free(bu);
		return 1;
	}
	free(bu);
	FILE* file = fopen("download", "wb");
	if (!file)
		return 1;

	char buffer[BUFF_SIZE];
	int size;
	int init = 1;
	int isinit = 1;
	int cont = 1;
	unsigned long total = 0;
	
	while (cont && ((size = recv(socket, buffer, sizeof(buffer)-1, 0)) > 0))
	{
		buffer[size] = 0;
		if (init) { size -= rem_headers(buffer); isinit = 0; }

		if (!init && strstr(buffer, "\r\n\r\n") != NULL)
		{
			cont = 0;
			size -= 8;
		}		
		fwrite(buffer, 1, size, file);
		total += size;
		init = isinit;
	}
	fclose(file);
	printf("total size downloaded: %lu\n", total);
	return 0;
}

int rem_headers(char *buffer)
{
	int i;
	for (i = 0; i < BUFF_SIZE; ++i) {
		if (buffer[i]=='\r' && buffer[i+1] == '\n' &&
		    buffer[i+2]=='\r' && buffer[i+3] =='\n') 
		{
			i+=4; 
			break; 
		}
	}

	strcpy(buffer, &buffer[i]);
	return i;
}
