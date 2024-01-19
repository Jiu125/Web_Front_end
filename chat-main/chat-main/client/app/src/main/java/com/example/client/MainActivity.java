package com.example.client;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        MyThread mt1 = new MyThread();
        mt1.start();

    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.d("안녕","안녕");
    }
}

class MyThread extends Thread{
    public void run() {

        Log.d("start", "Thread start");
        try {
            Log.d("hi","Hi");
            Socket socket = new Socket("192.168.1.70", 2023);

            byte[] buffer = new byte[1024];
            InputStream input = socket.getInputStream();

            input.read(buffer);

            String Data = new String(buffer);

            Log.d("Data", Data);

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }


}