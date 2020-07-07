import java.net.ServerSocket;
import java.net.*;
import java.io.*;
import java.nio.file.FileAlreadyExistsException;
import java.security.NoSuchAlgorithmException;
import java.util.Date;
import java.util.HashMap;
import java.util.Scanner;


public class Server
{
    //initialize socket and input stream

    private  static HashMap<String, Room> chatMap;

    // constructor with port
    public Server(int port) throws Exception {
        // starts server and waits for a connection
        chatMap = new HashMap<>();

            ServerSocket server = new ServerSocket(port);
            System.out.println("Server started");
            System.out.println("Waiting for a client ...");

            while(true){
                    Socket socket = server.accept();
                    Thread t;
                t = new Thread(   () -> {
                    try {
                        //create thread
                        System.out.println("Client accepted");

                        httpRequest newRequest = new httpRequest(socket);
                        httpResponse.response(socket, newRequest);
                        if(newRequest.isWebSocket()){

                            String message = MessageHandling.getMessage(socket);
                            String[] messageWords = message.split(" ");
                            String roomName = messageWords[1];
                            Room currentRoom =  addOrJoinRoom(roomName,socket);



                            while(true){
                            currentRoom.roomGetMessage(MessageHandling.getMessage(socket));
                            currentRoom.roomSendMessage();
                            }
                        }

                        socket.close();
                    }
                catch(IOException | InterruptedException | NoSuchAlgorithmException e)
                {
                    e.printStackTrace();
                }
            });
                t.start();
        }

    }

    public static Room addOrJoinRoom(String roomName, Socket newUser) throws IOException {
        if(chatMap.containsKey(roomName)){

            Room roomToAddFrom = chatMap.get(roomName);
            roomToAddFrom.addUserToRoom(newUser);

            System.out.println("created room & joined");
            return roomToAddFrom;
        }else{
            Room newRoom = new Room(roomName);
            System.out.println("created" + roomName );
            chatMap.put(roomName,newRoom);
            newRoom.addUserToRoom(newUser);
            System.out.println("joined" + roomName);
            // create room with constructor
            // use put to place in map roomname , new room constructor
            return newRoom;
        }

    }


    public static void main(String[] args) throws Exception {
        Server server = new Server(8080);
}
}



