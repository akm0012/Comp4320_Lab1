/*	
*	Comp4320: Lab 1
*
*	File: ClientTCP.c	
*	Author: Andrew K. Marshall (akm0012)
*	Group ID: 15
*	Date: 9/20/14
*	Version: 0.1
*	Version Notes: Starting point. Proof of concept.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define GROUP_PORT "10025"	// Port should be 10010 + Group ID (15)

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

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAX_PACKET_LEN];
	struct addrinfo hints, *servinfo, *p;
	int status;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) 
	{
		fprintf(stderr, "Usage Error: Params\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(argv[1], GROUP_PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 1;
	}

	// Loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("Socket error");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("Connect error");
			continue;
		}

		break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "Failed to connect!\n");
		return 2;
	}

	if (DEBUG) {
		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

		printf("Connected to: %s\n", s);
	}

	freeaddrinfo(servinfo); 	// All done with this structure

	// Sending a string (test123)
	char buf_out[] = "Test123";

	if (send(sockfd, buf_out, strlen(buf_out), 0) == -1)
	{
		perror("Send Error");
	}

	if ((numbytes = recv(sockfd, buf, MAX_PACKET_LEN, 0)) == -1) {
		perror("recv error");
		exit(1);
	}

	buf[numbytes] = '\0';
	
	printf("Message Received: '%s'\n", buf);

	close(sockfd);

	return 0;
}

