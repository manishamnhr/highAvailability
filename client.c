/*-----------------------------------------------------------------------------
* Author 	: 
* Purpose	: Client for listening to sockets
* Input		: Port number
* Returns	:
----------------------------------------------------------------------------*/



#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "daemon.h"


static int 
client_socket(int port_no)
{
	int			ret, clientSocket;
	char		sendbuf [MAX_BUF_SIZE];
	char		buf [MAX_BUF_SIZE];
	struct		sockaddr_in addr;
	socklen_t	addr_size;

	//creating the socket
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	addr_size = sizeof(addr);
	memset((void*)&addr, 0, addr_size);
	
	addr.sin_family = AF_INET;		//address family = Internet
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port_no);	//htons gives proper byte order

	//connect socket to server
	if ((ret = connect(clientSocket, (struct sockaddr *)&addr, addr_size)) == 0)
	{
		printf("connection successful\n");
	}
	else
	{
		printf("connection failed\n");
		return -1;
	}
	
	
	
	//sending message
	if ((send(clientSocket, sendbuf, MAX_BUF_SIZE, 0)) < 0)
	{
		printf("error on sending %d\n", errno);
	}
	
	else
	{
		printf("success\n");
	}
	
	printf("1\n");
	//read server message
	if ((recv(clientSocket, buf, MAX_BUF_SIZE, 0)) < 0)
	{
		printf("2\n");
		printf("error on receiving %d\n", errno);
	}
	else if((recv(clientSocket, buf, MAX_BUF_SIZE, 0)) > 0) //message received
	{
		//print contents of the buffer to view
		printf("3\n");
		printf("message received: %s\n", buf);
	}
	else
	{
		printf("4\n");
		printf("client shut down\n");						//orderly shutdown
	}
	printf("5\n");
	return 0;

}


int main()
{

	client_socket(6264);
	printf("in main\n");
	return 0;
}

