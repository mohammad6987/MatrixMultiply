package com.example.demo;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

public class Client {
    public static void main(String[] args) throws IOException {
        new Client().run();
    }
    public void run() throws IOException {
        String serverHost = "127.0.0.1";
        int serverPort = 125;
        try{
            Socket socket = new Socket(serverHost, serverPort);
            System.out.println(socket.isBound());
            System.out.println(socket.isConnected());
            DataInputStream inputStream = new DataInputStream(socket.getInputStream());

            DataOutputStream outputStream = new DataOutputStream(socket.getOutputStream());
            Convertor.random_fill();
            sendMatrix(outputStream, Convertor.red_matrix);
            sendMatrix(outputStream, Convertor.blue_matrix);
            sendMatrix(outputStream, Convertor.green_matrix);


            int[][] result_red = receiveMatrix(inputStream);
            int[][] result_blue = receiveMatrix(inputStream);
            int[][] result_green = receiveMatrix(inputStream);

            // Do something with the processed matrices (e.g., update UI)
            // For demonstration, let's just print them
            System.out.println("Result Red Matrix:");
            printMatrix(result_red);
            System.out.println("Result Blue Matrix:");
            printMatrix(result_blue);
            System.out.println("Result Green Matrix:");
            printMatrix(result_green);
        } catch (IOException e) {
            e.printStackTrace();
           System.out.println("error");
        }
    }

    private int[][] receiveMatrix(DataInputStream inputStream) throws IOException {
        int[][] matrix = new int[5][5];
        for (int i = 0; i < Convertor.MATRIX_SIZE; i++) {
            for (int j = 0; j < Convertor.MATRIX_SIZE; j++) {
                matrix[i][j] = inputStream.readInt();
            }
        }
        return matrix;
    }

    private  void sendMatrix(DataOutputStream outputStream, int[][] matrix) throws IOException {
        for (int i = 0; i < Convertor.MATRIX_SIZE; i++) {
            for (int j = 0; j < Convertor.MATRIX_SIZE; j++) {
                outputStream.writeInt(matrix[i][j]);
            }
        }
        outputStream.flush();
    }


    private void printMatrix(int[][] matrix) {
        for (int i = 0; i < Convertor.MATRIX_SIZE; i++) {
            for (int j = 0; j < Convertor.MATRIX_SIZE; j++) {
                System.out.print(matrix[i][j] + " ");
            }
            System.out.println();
        }
    }
}

