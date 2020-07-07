import java.io.IOException;
import java.net.Socket;
import java.net.http.WebSocket;
import java.util.ArrayList;

public class Room {
    String roomName;
    ArrayList<Socket> userSockets;
    ArrayList<String> userMessages;

    Room(String roomName){
        this.roomName = roomName;
        userSockets = new ArrayList<>();
        userMessages = new ArrayList<>();
    }
    //join room method

    public synchronized void addUserToRoom(Socket newUser) throws IOException {
        userSockets.add(newUser);
        for(String s: userMessages){
            MessageHandling.sendMessage(s,newUser);
        }

    }

    public synchronized void roomSendMessage() throws IOException {
    for(Socket sock: userSockets){
        MessageHandling.sendMessage(userMessages.get(userMessages.size() -1),sock);
    }
    }

    public synchronized void roomGetMessage(String message) {
        String[] parsedMessage = message.split(" ", 2);
        String userName = parsedMessage[0];
        String userMessage = parsedMessage[1];
        String jsonMess = "{\"user\": \"" + userName + "\", \"message\": \"" + userMessage + "\"}";
        userMessages.add(jsonMess);
//        System.out.println(jsonMess);
    }

}
