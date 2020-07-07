package com.example.androidchatapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.neovisionaries.ws.client.WebSocket;
import com.neovisionaries.ws.client.WebSocketAdapter;
import com.neovisionaries.ws.client.WebSocketException;
import com.neovisionaries.ws.client.WebSocketExtension;
import com.neovisionaries.ws.client.WebSocketFactory;

import org.json.JSONException;
import org.json.JSONObject;
import org.w3c.dom.Text;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import static android.provider.Telephony.Carriers.SERVER;

public class Chatroom extends AppCompatActivity {
    public WebSocket socket;
    public static String room;
    public static String user;
    public static String message;

    Handler h= new Handler();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_message);

        // Get the Intent that started this activity and extract the string
        Intent intent = getIntent();
        room = intent.getStringExtra(MainActivity.roomName);
        user = intent.getStringExtra(MainActivity.userName);

        // Capture the layout's TextView and set the string as its text
        TextView textView = findViewById(R.id.room_name);
        textView.setText(room);

        System.out.println(room);
        System.out.println(user);


        Thread t = new Thread( () -> {
            try {
                socket = connect();
            } catch (IOException e) {
                e.printStackTrace();
            } catch (WebSocketException e) {
                e.printStackTrace();
            }


        });
        t.start();

    }


    public WebSocket connect() throws IOException, WebSocketException {
        socket = null;
        try {
            socket = new WebSocketFactory().createSocket("http://ericschat.eastus.azurecontainer.io:8080/");

//            socket.connectAsynchronously();
            System.out.println("socket connected");
            socket.addListener(new WebSocketAdapter() {
                @Override
                public void onConnectError(WebSocket websocket, WebSocketException exception) throws Exception
                {
                    System.out.println("Connection error::::");
                    exception.printStackTrace();
                }
                public void onTextMessage(WebSocket websocket, String message)  {
                    System.out.println("inside onTextMessage: " + message);
                    h.post(new Runnable() {
                            @Override
                            public void run () {
                                try {
                                    appendTextView(message);
                                } catch (JSONException e) {
                                    e.printStackTrace();
                                }
                            }
                        });

                }
                @Override
                public void onConnected(WebSocket webSocket, Map<String, List<String>> headers) throws Exception{
                    super.onConnected(webSocket, headers);
                    System.out.println();
                    System.out.println("on connect: " + room);
                    System.out.println("on connect: " + user);
                    socket.sendText("join " + room + " " + user);
                }
            });
            socket.connect();
        } catch (IOException e){
            e.printStackTrace();
        }
        return socket;
    }

    public void send(View view){

        EditText userMessage = findViewById(R.id.chatEntry);
        message = userMessage.getText().toString();
        System.out.println(message);

        userMessage.setText("");
        socket.sendText(user + " " + message);

        System.out.println("user: " + user);


    }
    private void appendTextView(String message) throws JSONException {
        TextView text = new TextView(this);
        JSONObject jsonMess = new JSONObject(message);
        String user = jsonMess.getString("user");
        String userMessage = jsonMess.getString("message");
        String comboMess = user + " " + userMessage;
        text.setText(comboMess);

        ((LinearLayout)findViewById(R.id.scrolList)).addView(text);
    }

}
