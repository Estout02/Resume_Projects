package com.example.androidchatapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {
    public static String roomName = "ROOM";
    public static String userName = "USER";



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);



    }

    /** Called when the user taps the Send button */
    public void sendMessage(View view) {
        Intent intent = new Intent(this, Chatroom.class);
        EditText roomText = findViewById(R.id.room_name);
        String message = roomText.getText().toString();
        intent.putExtra(roomName, message);


        EditText userText = findViewById(R.id.editText);
        String user = userText.getText().toString();
        intent.putExtra(userName, user);

        System.out.println("room: " + message);
        System.out.println("username: " + user);


        startActivity(intent);
    }
}
