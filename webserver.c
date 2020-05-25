#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

char webpage[] = 
"HTTP/1.1 200 OK\n"
"Content-Type: text/html; character=UTF-16\n\n"
"<!DOCTYPE html>\n"
"<html><head><title>Web Server</title><link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\"></head>\n"
"<body><center><img src=\"Syllabus.jpg\">\n"
"</center></body></html>\n";


char not_webpage[] = 
"HTTP/1.1 200 OK\n"
"Content-Type: text/html; character=UTF-16\n\n"
"<!DOCTYPE html>\n"
"<html><head><title>Web Server</title><link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\"></head>\n"
"<body><center><h1>404</h1>\n"
"<p>Page not found</p>\n"
"</center></body></html>\n";

int main(int argc, char *argv[])
{

	// sockaddr_in is a structure to deal with internet addresses.
	struct sockaddr_in server_addr, client_addr;
	socklen_t sin_len = sizeof(client_addr);

	// File descriptor for the server, client and image
	int fd_server, fd_client, fd_img;

	// variable used to store the connection requests extracted from the server
	char  buf[2048];

	int on = 1;

	// socket() simply creates and endpoint for communication and returns a file 
	// descriptor that refers to that endpoint.
	// -> The domain argument (first) specifies a communication domain. AD_INET = IPv4 
	//    Internet Protocols
	// -> The type argument (second) specifies the communication semantics. 
	//    SOCK_STREAM = The connection is established and the two parties have a 
	//    conversation until the connection is terminated by one of the parties
	//    or by a network error
	// -> The protocol argument (third) specifies a particular protocol to be used
	//    with the socket. Normally only a single protocol exists to support a 
	//    particular socket type within a given protocol family, in which case
	//    protocol must be specified as 0.
	fd_server = socket(AF_INET, SOCK_STREAM, 0);

	if(fd_server < 0)
	{
		perror("socket");
		exit(1);	
	}

	// The setsockopt() provides an application program with the means to control 
	// socket behavior. An application program can use setsockopt() to allocate buffer
	// space, control timeouts, or permit socket data broadcasts.
	// -> The socket argument (first) describes the socket associated with the file
	//    descriptor
	// -> The level argument (second) specifies the protocol level at which the option 
	//    resides. SOL_SOCKET is the socket layer itself. It is used for options that are
	//    protocol independent.
	// -> The option_name argument (third) indicates that the rules used in validating addresses 
	//    supplied in a bind() call should allow reuse of local addresses.
	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

	// sin_family contains a code for the address family. AF_INET = Internet Protocol v4 
	// addresses
	server_addr.sin_family = AF_INET;

	// sin_addr.s_addr contains the ip address of the host. The inet_addr function shall convert 
	// the string pointed, in the standard IPv4 dotted decimal notation
	server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

	// sin_port containts the number of the port in which the requests will be handled. The htons() 
	// function converts the unsigned short integer from host byte order to network byte 
	// order.
	server_addr.sin_port = htons(8080);


	// The bind() function binds a socket to an address, in this case the address of 
	// the current host and port number on which the server will run. It takes 3
	// arguments, the socket file descriptor, the address to which is bound, and the
	// size of the address to wich it is bound. Returns 0 on success and -1 on failure.
	if(bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind");
		close(fd_server);
		exit(1);
	}

	// listen() marks the socket referred to by fd_server as a passive socket,
	// that is, as a socket that will be used to accept incoming connection 
	// requests using accept(). On success, zero is returned. On error, -1 is 
	// returned. 
	// -> The first argument is the socket file descriptor.
	// -> The second argument is the size of the backlog queue, the number of 
	//    connections that can be waiting while the proccess is handling a 
	//    particular connection.
	if(listen(fd_server, 1000) == -1)
	{
		perror("listen");
		close(fd_server);
		exit(1);
	}

	while(1)
	{
		// the function accept() causes the process to block until a client connects
		// to the server. Thus, it wakes up the process when a connection from a client
		// has been successfully established. It returns a new file descriptor, and all
		// communication on this connections should be done using the new file descriptor.
		fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);

		if(fd_client == -1)
		{
			perror("Connection failed...\n");
			continue;
		}

		printf("Got client connection....\n");

		// Divide the socket 
		if (!fork())
		{
			/* child process */
			close(fd_server);
			memset(buf, 0, 2048);
			read(fd_client, buf, 2047);

			printf("%s\n", buf);


			if(strncmp(buf, "GET /TC2025.html HTTP/1.1", 25) != 0)
			{
				if (!strncmp(buf, "GET /Syllabus.jpg", 17))
				{
					fd_img = open("Syllabus.jpg", O_RDONLY);
					sendfile(fd_client, fd_img, NULL, 950000);
					close(fd_img);
				}
				else {
					write(fd_client, not_webpage, sizeof(webpage) - 1);
				}
		
			}
			else {
				write(fd_client, webpage, sizeof(webpage) -1);
			}

			close(fd_client);
			printf("closing...\n");

			exit(0);

		}
		/* parent process */
		close(fd_client);

	}

	return 0;
}