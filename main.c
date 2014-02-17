#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <ctypes.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

int connect_to_socket(const char* ip, struct sockaddr_in* dest, int port);
int download_file(int socket, const char* filepath);
void rem_headers(char *buffer)

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		puts("usage: ./download <url> <filepath>");
		return 1;
	}

	struct hostent *h = gethostbyname(argv[1]);
	char* ip = inet_ntoa(*((struct in_addr*)h->h_addr));

	sockaddr_in dest;
	int my_socket;
	if (my_socket = connect_to_socket(ip, &dest, 80))
	{
		perror("couldn't connect. ");
		return 1;
	}

	puts("connected.");

	if (download_file(my_socket, filepath))
	{
		puts("unable to download");
		return 1;
	}
	puts("download completed");
	return 0;
}
	

int connect_to_socket(const char* ip, struct sockaddr_in* dest, int port)
{
	int my_socket = socket(AF_INET, SOCK_STREAM, NO_SOCK_FLAGS);
	if (my_socket < 0)
	{
		perror("socket allocation failed");
		return 1;
	}
	memset (dest, 0, sizeof(dest));
	dest->sin_family = AF_INET;
	dest->sin_port = htons(port);
	dest->sin_addr.s_addr = inet_addr(ip);

	connect(my_socket, (struct sockaddr*)dest, (socklen_t)sizeof(*dest));

	return my_socket;
}

int download_file(int socket, const char* filepath)
{
	char buff[200];
	strcpy(buff, "GET ");
	strcat(buff, filepath);
	strcat(buff, " HTTP1.1\n\n");
	if (!send(socket, buff, strlen(buff), NO_FLAGS))
		return 1;
	
	FILE* file = fopen("download", "wb");
	if (!file)
		return 1;

	char buffer[4096];
	int size;
	int init = 0;
	while ((size = recv(my_socket, buffer, sizeof(buffer)-1, NO_FLAGS)) > 0)
	{
		buffer[size] = 0;
		if (!init) { rem_headers(buffer); init = 1; }
		fwrite(buffer, 1, size, file);
	}
	fclose(file);
	return 0;
}

void rem_headers(char *buffer)
{
	int i;
	for (i = 0; i < length; ++i) {
		if (buffer[i]=='\r' && buffer[i+1] == 'n' && buffer[i+2]=='\r' && buffer[i+3] =='\n') 
			{ i+=4; break }
	}
	strcpy(buffer, &buffer[i]);
}
