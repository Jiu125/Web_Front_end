package com.example.client;

import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class CommunicationThread extends Thread{
    Socket socket;
    String ip;
    int port;
    InputStream input;
    OutputStream output;

    public CommunicationThread(String ip,int port){
        //메인 액티비티에 static 멤버변수로 선언해놓고 쓰면 될듯?
        this.ip=ip;
        this.port=port;
    }
    @Override
    public void run(){
        try {
            socket=new Socket(this.ip,this.port);
            input=socket.getInputStream();
            output=socket.getOutputStream();
            byte[] byteHeader=new byte[21];
            byte[] body;
            while(true){
                input.read(byteHeader);
                MessageHeader header=new MessageHeader(byteHeader);
                Log.d("header",""+header.type+"|"+header.code+"|"+header.bodySize);
                if(header.bodySize>0){
                    body=new byte[header.bodySize];
                    input.read(body);
                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    public void Send(byte[] header,byte[] body){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    output.write(header);
                    output.flush();
                    output.write(body);
                    output.flush();
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        }).start();
    }
    public void Send(byte[] header){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    output.write(header);
                    output.flush();
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        }).start();
    }
}
