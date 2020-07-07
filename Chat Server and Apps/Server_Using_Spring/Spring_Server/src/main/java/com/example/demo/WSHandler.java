package com.example.demo;

import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;
import org.w3c.dom.Text;

import java.io.IOException;

public class WSHandler extends TextWebSocketHandler {

    @Override
    protected void handleTextMessage(WebSocketSession session, TextMessage message) throws Exception {
        String newMessage = message.getPayload();

//        System.out.println(newMessage);
        String[] parsedRequest = newMessage.split(" ",2);
        String joinRequest = parsedRequest[0];
        String roomRequest = parsedRequest[1];
//        System.out.println(joinRequest);
//        System.out.println(roomRequest);

        Room room = Room.getCurrentRoom(session);


//            parseUserMessage(newMessage);

        if(room == null){

            Room.joinRoom(session, joinRequest);
            //join a room static method
            System.out.println("created and joined room " + roomRequest);




        }else{
            System.out.println("room exists, send messages");
            //send the message to sockets method
            Room currentRoom = Room.getCurrentRoom(session);
            TextMessage textMessage = parseUserMessage(newMessage);


            currentRoom.postMessage(textMessage);
            System.out.println(textMessage);

        }

    }

    synchronized public TextMessage parseUserMessage(String newMessage) throws IOException {
        String[] parsedMessage = newMessage.split(" ",2);
        String userName = parsedMessage[0];
        String userMessage = parsedMessage[1];
        String jsonMess = "{\"user\": \"" + userName + "\", \"message\": \"" + userMessage + "\"}";
        TextMessage textMessage = new TextMessage(jsonMess);
        return textMessage;
    }
}
