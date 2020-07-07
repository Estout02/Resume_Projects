package com.example.demo;

import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;


public class Room {
    static private HashMap <WebSocketSession, Room> userSocket = new HashMap<>();
    static private HashMap <String, Room> rooms = new HashMap<>();
    private  ArrayList <TextMessage> messages = new ArrayList<>();

    private ArrayList <WebSocketSession> userSockets = new ArrayList<>();


    synchronized public static Room getCurrentRoom(WebSocketSession socket){
    return userSocket.get(socket);
}

//public static WebSocketSession getSocket(String roomRequest){
//    Room addRoom = new Room();
//    room.put(roomRequest, addRoom);
//    WebSocketSession requestSocket = addRoom.roomName.get(socket);
//
//    return requestSocket;
//}


    synchronized public static  void joinRoom(WebSocketSession socket, String roomRequest){

    if(rooms.containsKey(roomRequest)){
        //get messages
        //write to socket
        Room currentRoom = rooms.get(roomRequest);
        userSocket.put(socket, rooms.get(roomRequest));
        currentRoom.setUserSocket(socket);



    }else {
        Room addRoom = new Room();

        addRoom.rooms.put(roomRequest, addRoom);
//        room.put(roomRequest, addRoom);
        userSocket.put(socket, addRoom);
        addRoom.setUserSocket(socket);

    }
}

//    public void setMessages(TextMessage textMessage){
//    messages.add(textMessage);
//    }
    synchronized public void  setUserSocket(WebSocketSession socket){
    userSockets.add(socket);
}


//void postExistingMessages()

    synchronized void postMessage(TextMessage textMessage) throws IOException {
    messages.add(textMessage);
    for (WebSocketSession soc : userSockets){
        soc.sendMessage(textMessage);
    }

        System.out.println(userSockets.size());

}








}
