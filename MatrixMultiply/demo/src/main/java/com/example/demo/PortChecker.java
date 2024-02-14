package com.example.demo;

import java.io.IOException;
import java.net.Socket;

public class PortChecker {
    public static void main(String[] args) {
        String serverHost = "127.0.0.1"; // Replace with your server's IP address or hostname
        int serverPort = 125; // Replace with the port you want to check

        try {
            // Attempt to connect to the server's port
            Socket socket = new Socket(serverHost, serverPort);

            // If the connection is successful, the port is open and the server is reachable
            System.out.println("Port " + serverPort + " is open and the server is reachable.");

            // Close the socket
            socket.close();
        } catch (IOException e) {
            // If an IOException occurs, the port is closed or the server is unreachable
            System.out.println("Port " + serverPort + " is closed or the server is unreachable.");
        }
    }
}
