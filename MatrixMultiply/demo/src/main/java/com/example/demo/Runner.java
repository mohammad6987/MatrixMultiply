package com.example.demo;

public class Runner {
    public static void main(String[] args) {
        Convertor.random_fill();
        Convertor.writeMatrixToFile("G:\\untitled5\\input.txt", Convertor.red_matrix, Convertor.blue_matrix, Convertor.green_matrix);
        System.out.println("wrote "+ Convertor.MATRIX_SIZE+"^2 numbers to the selected file");
    }
}
