import java.io.*;
import java.net.*;

import javax.xml.bind.DatatypeConverter;

/**
 * i. Accepts as command line of the form: 
 * client 
 * servername 
 * PortNumber 
 * Operation 
 * String 
 * 
 * where: 
 * servername is the server name, 
 * PortNumber  is the port number, 
 * Operation  is the operation requested (85 for length and 170 for Disemvoweling),
 * */
class UDPClient {
	public static void main(String args[]) throws Exception {
		
		String serverName = args[0];
		int portNumber = Integer.parseInt(args[1]);
		int operation = Integer.parseInt(args[2]);
		String message = args[3];
		
		byte[] sendData = new byte[message.length() + 5];
		sendData[0] = (byte) (message.length() & 0xFF);
		sendData[1] = (byte) ((message.length() >> 8) & 0xFF);

		BufferedReader inFromUser = new BufferedReader(new InputStreamReader(
				System.in));
		DatagramSocket clientSocket = new DatagramSocket();
		InetAddress IPAddress = InetAddress.getByName(serverName);

		byte[] receiveData = new byte[1024];

		String sentence = inFromUser.readLine();
		sendData = sentence.getBytes();

		DatagramPacket sendPacket = new DatagramPacket(sendData,
				sendData.length, IPAddress, portNumber);
		clientSocket.send(sendPacket);

		DatagramPacket receivePacket = new DatagramPacket(receiveData,
				receiveData.length);

		clientSocket.receive(receivePacket);

		String modifiedSentence = new String(receivePacket.getData());

		System.out.println("FROM SERVER:" + modifiedSentence);

		clientSocket.close();
	}
}