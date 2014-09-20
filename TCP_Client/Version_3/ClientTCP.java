// TCPClient.java
// Chris Barrow, Andrew Marshall, John Skandalakis
// 9/17/14

import java.net.*; // for Socket 
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.io.*; // for IOException and Input/OutputStream 

public class ClientTCP {
	
	private static String server;
	private static int servPort;
	private static String string;
	private static byte[] byteBuffer;
	private static short operation;
	private static short length;
	private static short ReqID;
	
	public static void main(String[] args) throws IOException {
		if (args.length == 0) {
			server = "172.17.94.66";
			servPort = 10025;
			operation = 170; //85 or 170
			string = "This is a test";
			length = 19;
			ReqID = 1;
		}
		else {

			try {
				server = args[1];
			}
			catch (Exception e) {
				throw new IllegalArgumentException("Parameter(s): \"client\", ServerName, PortNumber, Operation, String");
			}
			
			
			try {
				servPort = Integer.parseInt(args[2]);
			}
			catch (Exception e) {
				throw new IllegalArgumentException("Parameter(s): \"client\", ServerName, PortNumber, Operation, String");
			}
			
			
			try {
				operation = Short.parseShort(args[3]);
			}
			catch (Exception e) {
				throw new IllegalArgumentException("Parameter(s): \"client\", ServerName, PortNumber, Operation, String");
			}
			
			// Convert input String to bytes using the default character encoding
			
			try {
				string = new String(args[4].getBytes());
			}
			catch (Exception e) {
				throw new IllegalArgumentException("Parameter(s): \"client\", ServerName, PortNumber, Operation, String");
			}
			
			length = (short)(5 + string.length());
			ReqID = 1;
		}
		
		byteBuffer = new byte[string.length() + 5];
		
		Socket Serversocket = new Socket(server, servPort); 
		System.out.println("Connected to server...");
		
		InputStream in = Serversocket.getInputStream();
		OutputStream out = Serversocket.getOutputStream();
		
		ByteBuffer buffer = ByteBuffer.allocate(2);
		buffer.putShort(length);
		//buffer.flip();
		byteBuffer[0] = buffer.get(0);
		byteBuffer[1] = buffer.get(1);
		
		buffer = ByteBuffer.allocate(2);
		buffer.putShort(ReqID);
		//buffer.flip();
		byteBuffer[2] = buffer.get(0);
		byteBuffer[3] = buffer.get(1);
		
		buffer = ByteBuffer.allocate(1);
		buffer.put((byte)operation);
		//buffer.flip();
		byteBuffer[4] = buffer.get(0);
		
		for (int i = 0; i < string.length(); i++) {
			byteBuffer[5 + i] = (byte) string.charAt(i);
		}
		
		out.write(byteBuffer); // Send the encoded string to the server
		
		byteBuffer = new byte[32];
		int totalBytesRcvd = 0; // Total bytes received so far
		int bytesRcvd; // Bytes received in last read
		//while (totalBytesRcvd < byteBuffer.length) {
			bytesRcvd = in.read(byteBuffer, totalBytesRcvd, byteBuffer.length - totalBytesRcvd);
			totalBytesRcvd += bytesRcvd;
			System.out.println("Received: Request ID(ID doesn't print right): " + new String(Arrays.copyOfRange(byteBuffer, 2, 4)) +  " + Result: " +  new String(Arrays.copyOfRange(byteBuffer, 4, byteBuffer.length)));
			Serversocket.close();
		//}
	}
}