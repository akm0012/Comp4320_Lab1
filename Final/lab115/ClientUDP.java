import java.io.*; 
import java.net.*; 
import java.util.Random; 
import javax.xml.bind.DatatypeConverter; 
import java.nio.ByteBuffer;
import java.util.Timer; 
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
 * Operation  is the operation requested (85 for length and 170 for  
 *    Disemvoweling), 
 * */ 
class ClientUDP { 
    public static void main(String args[]) throws Exception { 
        long startTime = System.nanoTime();
 
	String client_check = args[0];
        String serverName = args[1]; 
        int portNumber = Integer.parseInt(args[2]); 
        int operation = Integer.parseInt(args[3]); 
         
        Random random = new Random(); 
        int rid = random.nextInt(255); 
        String message = args[4]; 
 
        //a 5 byte header that goes befor the message. 
        byte[] header = new byte[5]; 
         
        //TML - TotalMessageLength 
        header[0] = (byte) (message.length() & 0xFF); 
        header[1] = (byte) ((message.length() >> 8) & 0xFF); 
         
        //Request ID 
        header[2] = (byte) (rid & 0xFF); 
        header[3] = (byte) ((rid >> 8) & 0xFF); 
         
        //operation 
        header[4] = (byte) (operation & 0xFF); 
         
        byte[] messageData = message.getBytes(); 
        byte[] sendData = new byte[header.length + messageData.length]; 
         
        //attach the two arrays. 
        for (int i = 0; i < sendData.length; ++i) 
        { 
                sendData[i] = i < header.length ? header[i] :  
                    messageData[i - header.length]; 
                     
                //this is to test what the datastream looks like 
                /*if (i < 5) System.out.println((int) sendData[i]); 
                else System.out.print((char) sendData[i]);*/ 
        } 
         
        BufferedReader inFromUser = new BufferedReader( 
            new InputStreamReader(System.in)); 
             
        //creates a client socket 
        DatagramSocket clientSocket = new DatagramSocket(); 
         
        InetAddress IPAddress = InetAddress.getByName(serverName); 
 
        byte[] receiveData = new byte[1024]; 
 
        //create a send packet 
        DatagramPacket sendPacket = new DatagramPacket(sendData, 
                sendData.length, IPAddress, portNumber); 
                 
        //set the client to send the packet to the server 
        clientSocket.send(sendPacket); 
 
        //create the packet to be received 
        DatagramPacket receivePacket = new DatagramPacket(receiveData, 
                receiveData.length); 
 
        //set the client to receive the data 
        clientSocket.receive(receivePacket); 
        
        //format the message total length
        
         
        //format the server's request ID 
        byte[] requestedRIDBytes = new byte[2]; 
        requestedRIDBytes[0] = receiveData[2]; 
        requestedRIDBytes[1] = receiveData[3]; 
        short requestedRID = ByteBuffer.wrap(requestedRIDBytes).getShort(); 
        int intVal = requestedRID >= 0 ? requestedRID : 0x10000 + requestedRID;
        
        //format the server's response
        String response = "";
        if (operation > 100) 
            for (int i = 4; i < receiveData.length; i++) { 
                response += (char)receiveData[i]; 
            }
        else {
        
            short val = ByteBuffer.wrap(
                new byte[]{receiveData[4],receiveData[5]})
                .getShort(); 
            int inte = val >= 0 ? val : 0x10000 + val;
            response = "" + inte;
        }
         
        //Print the results
        System.out.println("------------------"); 
        System.out.println("request id: " + intVal); 
        System.out.println("server response: " + response); 
         
         
        //close this client socket. 
        clientSocket.close();
        
        long endTime = System.nanoTime();

        long duration = (endTime - startTime)/1000000;
        
        System.out.println("duration(ms): " + duration);
        System.out.println("------------------"); 
    } 
}
