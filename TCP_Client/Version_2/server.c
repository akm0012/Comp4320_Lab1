/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "10025"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

#define GROUP_PORT "10025"	// Port should be 10010 + Group ID

#define MAX_MESSAGE_LEN 1024
#define MAX_PACKET_LEN 1029	// 1Kb for message, and 5 bytes for header
#define V_LENGTH 85	// Operation: Count vowels
#define DISEMVOWEL 170	// Operation: Remove vowels

#define DEBUG 0	// Used for debugging: 1 = ON; 0 = OFF

// Prototypes
int count_vowels(char*);
char* disemvowel(char*); 

// Struct that will be received from the client
struct received_packet
{
	unsigned short TML;	// Total Message Length (2 bytes)
	unsigned short RID;	// Request ID (2 bytes)
	unsigned char operation;	//operation (1 byte)
	char message[MAX_MESSAGE_LEN];	// Message (Limited to 1 Kb)
} __attribute__((__packed__));

typedef struct received_packet rx_packet;	
	

// Struct that will be sent to the client, if client requested vLength
struct transmitted_packet_vLength
{
	unsigned short TML;	// Total Message Length (2 bytes)
	unsigned short RID;	// Request ID (2 bytes)
	unsigned short vLength;	// The number of vowels 
}__attribute__((__packed__));	

typedef struct transmitted_packet_vLength tx_vLength;

// Struct that will be sent to the client, if client requested diemvowelment
struct transmitted_packet_disemvowel
{
	unsigned short TML;	// Total Message Length (2 bytes)
	unsigned short RID;	// Request ID (2 bytes)
	char message[MAX_MESSAGE_LEN];	// The number of vowels 
}__attribute__((__packed__));	

typedef struct transmitted_packet_disemvowel tx_disVowel;

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);
		
		int numbytes;
		char newBuf[1000];
	
		rx_packet packet_in;


		if((numbytes = recv(new_fd, (char *)&packet_in, MAX_PACKET_LEN, 0)) == -1) 
		{
			perror("recv_error");
			exit(1);
		}

		packet_in.message[numbytes - 5] = '\0';

		printf("Message Recieved: %s\n", newBuf);
		
		printf("packet_in.TML: %d\n", ntohs(packet_in.TML));	
		printf("packet_in.RID: %d\n", ntohs(packet_in.RID));	
		printf("packet_in.operation: %d\n", packet_in.operation);	
		printf("packet_in.message: %s\n", packet_in.message);	

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener

			if (packet_in.operation == 85) {			

				tx_vLength packet_out_vLength;
				packet_out_vLength.TML = htons(6);
				packet_out_vLength.RID = htons(ntohs(packet_in.RID));
				packet_out_vLength.vLength = htons(9);
				

				if (send(new_fd, (char *)&packet_out_vLength, packet_out_vLength.TML, 0) == -1)
					perror("send");
				close(new_fd);
				exit(0);
			}
			
			if (packet_in.operation == 170) {			

				char test[] = "Test";
				
				tx_disVowel packet_out_disVowel;
				packet_out_disVowel.TML = htons(8);
				packet_out_disVowel.RID = htons(ntohs(packet_in.RID));
				strcpy(packet_out_disVowel.message, test);
				

				if (send(new_fd, (char *)&packet_out_disVowel, ntohs(packet_out_disVowel.TML), 0) == -1)
					perror("send");
				close(new_fd);
				exit(0);
			}
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}


















