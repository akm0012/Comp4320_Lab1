Comp 4320 - Lab 1
Due: 9/23/14
Group ID: 15
Group Members: 
	Andrew Marshall (akm0012@auburn.edu), 
	Chris Barrow (csb0017@auburn.edu), 
	John Skandalakis (jes0027@auburn.edu)

Code Status: WORKING

-----------------------------------------------------------------

Compile Instructions:

The easiest way to compile all files is to run "compile_all.cmd" by typing: ./compile_all.cmd into the command line. 
Note: if you do not have run permissions, you may have to type: "chmod 775 compile_all.cmd"

If you want to compile the programs individually, you can do so by typing the following:

	javac ClientUDP.java
	cc -o ServerUDP.exe ServerUDP.c
	cc -o ClientTCP.exe ClientTCP.c
	javac ServerTCP.java

-----------------------------------------------------------------

Run Instructions: 

To run the UDP server/client type:

	./ServerUDP.exe server [PORT NUMBER]
	java ClientUDP client [HOST NAME] [PORT NUMBER] [OPERATION] [MESSAGE]

Example:

	./ServerUDP.exe server 10025
	java ClientUDP client 127.0.0.1 10025 170 ThisIsATest

To run the TCP server/client type:

	java ServerTCP server [PORT NUMBER]
	./ClientTCP.exe client [HOST NAME] [PORT NUMBER] [OPERATION] [MESSAGE]

Example:

	java ServerTCP server 10025
	./ClientTCP.exe client 127.0.0.1 10025 170 ThisIsATest
-----------------------------------------------------------------

Known Bugs:

	1. Parameter checking is not polished. 


