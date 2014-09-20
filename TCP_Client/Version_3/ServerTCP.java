// TCPServer.java
// Chris Barrow, Andrew Marshall, John Skandalakis
// 9/17/14

import java.net.*; 
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.io.*; 

public class ServerTCP { 
	
  private static final int BUFSIZE = 32; //receive buffer
  
  public static void main (String args[]) throws IOException
  { 
	int serverPort = 10025;
	if (args.length > 0) {
		if (args.length == 2 && args[0].toLowerCase().equals("server")) {
			try {
				serverPort = Integer.parseInt(args[1]);
			}
			catch (Exception e) {
				throw new IllegalArgumentException("Parameter(s): \"server\" <Port>");
			}
		}
		else {
			throw new IllegalArgumentException("Parameter(s): \"server\" <Port>");
		}
	}
	else {
		System.out.println("No arguments given. Using default port value 10025.");
	}
		
	ServerSocket serverSocket = new ServerSocket(serverPort); 
	int recieveMessageSize;
	byte[] byteBuffer = new byte[BUFSIZE];
  
	System.out.println("Server listening...");

	while(true) { 
		Socket clientSocket = serverSocket.accept(); 
		
		System.out.println("Handling client at " + clientSocket.getInetAddress().getHostAddress() + " on port " + clientSocket.getPort());
		
		InputStream in = clientSocket.getInputStream();
		OutputStream out = clientSocket.getOutputStream();
		
		while ((recieveMessageSize = in.read(byteBuffer)) != -1){
			
			byte[] lengthIn = Arrays.copyOfRange(byteBuffer,0,2);
			ByteBuffer wrapped = ByteBuffer.wrap(lengthIn);
			short length = wrapped.getShort();
			byteBuffer = Arrays.copyOfRange(byteBuffer, 0, length);
			
			byte[] reqIDIn = Arrays.copyOfRange(byteBuffer,2,4);
			
			short operationIn = (short)byteBuffer[4];
			if (operationIn < 0) {
				operationIn &= 0xff;
			}
			
			String result = new String(Arrays.copyOfRange(byteBuffer, 5, byteBuffer.length));
			
			if (operationIn == 170) {
				result = disemvowel(result);
			}
			else if (operationIn == 85) {
				result = Integer.toString(numVowels(result));
			}
			
			byte[] resultBytes = result.getBytes();
			
			short totalLength = (short) (4 + resultBytes.length);
			
			byteBuffer = new byte[totalLength];
			
			ByteBuffer buffer = ByteBuffer.allocate(2);
			buffer.putShort(totalLength);
			byteBuffer[0] = buffer.get(0);
			byteBuffer[1] = buffer.get(1);
			
			byteBuffer[2] = reqIDIn[0];
			byteBuffer[3] = reqIDIn[1];
			
			for (int i = 0; i < resultBytes.length; i++) {
				byteBuffer[4 + i] = resultBytes[i];
			}
			
			out.write(byteBuffer);
		}
		
		clientSocket.close();
		byteBuffer = new byte[BUFSIZE];
	} 
} 
  
  public static int numVowels(String input) {
	  int result = 0;
	  for (int i = 0; i < input.length(); i++){
		  if (input.charAt(i) == 'A' || input.charAt(i) == 'a'
				  || input.charAt(i) == 'E' || input.charAt(i) == 'e'
				  || input.charAt(i) == 'I' || input.charAt(i) == 'i'
				  || input.charAt(i) == 'O' || input.charAt(i) == 'o'
				  || input.charAt(i) == 'U' || input.charAt(i) == 'u') {
			  
			  result++;
		  }
	  }
	  return result;
  }
  
  public static String disemvowel(String input) {
	  String result = "";
	  for (int i = 0; i < input.length(); i++){
		  if (input.charAt(i) != 'A' && input.charAt(i) != 'a'
				  && input.charAt(i) != 'E' && input.charAt(i) != 'e'
				  && input.charAt(i) != 'I' && input.charAt(i) != 'i'
				  && input.charAt(i) != 'O' && input.charAt(i) != 'o'
				  && input.charAt(i) != 'U' && input.charAt(i) != 'u') {
			  
			 result += input.charAt(i);
		  }
	  }
	  return result;
  }
}
