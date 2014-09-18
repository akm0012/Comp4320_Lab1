/*	
*	Comp4320: Lab 1
*
*	File: ServerUDP.c	
*	Author: Andrew K. Marshall (akm0012)
*	Group ID: 15
*	Date: 9/17/14
*	Version: 0.3
*	Version Notes: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define GROUP_PORT "10025"	// Port should be 10010 + Group ID

#define MAX_MESSAGE_LEN 1024

#define DEBUG 1	// Used for debugging: 1 = ON; 0 = OFF

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

// Used to determine if we are using IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa) 
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	//else
	return &(((struct sockaddr_in6*)sa)->sin6_addr);

}


int main(int argc, char *argv[])
{
	// Used to check the size of the differnet elements in the struct
	if (DEBUG) {
		rx_packet test;
		printf("packed size of entire packet is %d bytes\n", (int) sizeof(test));
		printf("packed size of TML is %d bytes\n", (int) sizeof(test.TML));
		printf("packed size of RID is %d bytes\n", (int) sizeof(test.RID));
		printf("packed size operation is %d bytes\n", (int) sizeof(test.operation));
		printf("packed size of message is %d bytes\n", (int) sizeof(test.message));
	}
	
	// Variables 
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int status;
	int numbytes;
	char *my_port;
	
	struct sockaddr_storage their_addr;
	char buf[MAX_MESSAGE_LEN + 5];	// Make the buffer big enough for a full 1Kb message + 5 bytes for TML, etc.
	socklen_t addr_len;		

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;	// set to AF_INIT to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;	// use my IP

	// Check to make sure the command line arguments are valid
	if (argc != 2) 
	{
		fprintf(stderr, "Usage Error: Should be 1 argument, the port number.\n");
		exit(1);
	}

	// Get the port number from the command line
	my_port = argv[1];
	
	if (DEBUG) {
		printf("DEBUG: Port number: %s\n", my_port);
	}
	
	printf("Starting Server... to stop, press 'Ctrl + c'\n");
	
	while(1)
	{		
		// 1. getaddrinfo
		if ((status = getaddrinfo(NULL, my_port, 
			&hints, 	// points to a struct with info we have already filled in
			&servinfo)) != 0)	// servinfo: A linked list of results
		{
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
			return 1;
		}

		// Loop through all the results and bind to the first we can
		for (p = servinfo; p != NULL; p = p->ai_next)
		{
			// 2. socket
			if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			{	
				perror("Socket Error!");
				continue;
			}
		
			// 3. bind
			if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
			{
				close(sockfd);
				perror("Bind Error!");
				continue;
			}

			break;
		}
		
		if (p == NULL)
		{
			fprintf(stderr, "Failed to bind socket\n");
			return 2;
		}	
		

		freeaddrinfo(servinfo); 	// Call this when we are done with the struct "servinfo"

		if (DEBUG) {
			printf("Binding complete, waiting to recvfrom...\n");
		}

		addr_len = sizeof their_addr;

		// 4. recvfrom
		if ((numbytes = recvfrom(sockfd, buf, MAX_MESSAGE_LEN + 5, 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1)
		{
			perror("recvfrom");
			exit(1);
		}
	
		printf("Packet Received!\n");

		// Add the null to terminate the string
		buf[numbytes] = '\0';

		if (DEBUG) {
			printf("Packet is %d bytes long.\n", numbytes);
			printf("Buffer = '%s'\n", buf);
		}

		if (DEBUG) {
			printf("Sending Echo...\n");
		}

		// 5. sendto
		if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&their_addr, addr_len) == -1)
		{
			perror("sendto error");
			exit(1);
		}		



		close(sockfd);
	}
	return 0;
} 





































