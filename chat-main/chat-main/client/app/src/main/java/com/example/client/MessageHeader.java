package com.example.client;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public class MessageHeader {
    byte type;
    String code;
    int bodySize;

    public MessageHeader(byte type,String code,int bodySize) {
        this.type=type;
        this.code=code;
        this.bodySize=bodySize;
    }
    public MessageHeader(byte[] data) {
        this.type=data[0];
        this.code=Arrays.copyOfRange(data,1,16).toString();
        this.bodySize=ByteBuffer.wrap(Arrays.copyOfRange(data,17,data.length)).getInt();
    }
    public byte[] ToByte() {
        byte[] result=new byte[21];
//        byte[][] data= {this.code.getBytes(),ByteBuffer.allocate(Integer.BYTES).putInt(this.bodySize).array()};
        byte[][] data= {this.code.getBytes(),ByteBuffer.allocate(Integer.BYTES).order(ByteOrder.LITTLE_ENDIAN).putInt(this.bodySize).array()};
        int [] offset= {1,17};

        result[0]=this.type;

        for(int i=0;i<offset.length;i++){
            for(int j=0;j<data[i].length;j++) {
                result[offset[i]+j]=data[i][j];
            }
        }

//        for(int i=0;i<data[1].length;i++) {
//            result[17+i]=data[1][data[1].length-1-i];
//        }

        return result;
    }
}