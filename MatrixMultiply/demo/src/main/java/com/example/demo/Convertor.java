package com.example.demo;

import javafx.application.Application;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.scene.image.PixelReader;
import javafx.scene.layout.HBox;
import javafx.scene.paint.Color;
import javafx.stage.Stage;
import java.io.*;
import java.util.Random;
import java.util.Scanner;

public class Convertor extends Application {
    static int MATRIX_SIZE = 32;
    static int[][] inputMatrix_red = new int[MATRIX_SIZE][MATRIX_SIZE];
    static int[][] inputMatrix_blue = new int[MATRIX_SIZE][MATRIX_SIZE];
    static int[][] inputMatrix_green = new int[MATRIX_SIZE][MATRIX_SIZE];
    static int[][] red_matrix = new int[MATRIX_SIZE][MATRIX_SIZE];
    static int[][] blue_matrix = new int[MATRIX_SIZE][MATRIX_SIZE];
    static int[][] green_matrix = new int[MATRIX_SIZE][MATRIX_SIZE];

    public Convertor() {
        for(int i=0;i<MATRIX_SIZE;i++){
            for(int j=0;j<MATRIX_SIZE;j++){
                red_matrix[i][j]= 255;
                blue_matrix[i][j] =255;
                green_matrix[i][j] = 255;
            }
        }
    }

    public static void imageConvertor(String filePath){
        Image image = new Image(filePath);

        PixelReader pixelReader = image.getPixelReader();

        int width = (int) image.getWidth();
        int height = (int) image.getHeight();
        if(width > MATRIX_SIZE || height > MATRIX_SIZE){
            throw new IllegalCallerException();
        }
        Color[][] colorMatrix = new Color[width][height];

        for (int x = 0; x < width; x++) {
            for (int y = height-1; y >-1 ; y--) {
                Color color = pixelReader.getColor(y, x);
                colorMatrix[x][y] = color;
                red_matrix[x][y] = (int) (colorMatrix[x][y].getRed()*255);
                green_matrix[x][y] = (int) (colorMatrix[x][y].getGreen()*255);
                blue_matrix[x][y] = (int) (colorMatrix[x][y].getBlue()*255);
            }
        }

    }

    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("Result");
        readMatrixFromFile("G:\\untitled5\\input.txt",red_matrix,blue_matrix,green_matrix);
        readMatrixFromFile("G:\\untitled5\\output.txt",inputMatrix_red,inputMatrix_blue,inputMatrix_green);
        Canvas canvas = new Canvas(400, 400);
        drawMatrix(canvas.getGraphicsContext2D(), red_matrix,blue_matrix,green_matrix,MATRIX_SIZE);
        Canvas canvas2 = new Canvas(400, 400);
        drawMatrix(canvas2.getGraphicsContext2D(), inputMatrix_red,inputMatrix_blue,inputMatrix_green,MATRIX_SIZE);
        HBox root = new HBox();
        root.setAlignment(Pos.TOP_CENTER);
        root.setSpacing(20);
        root.getChildren().add(canvas2);
        root.getChildren().add(canvas);
        Scene scene = new Scene(root, 820, 400);
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    public static void drawMatrix(GraphicsContext gc, int[][] inputMatrix_red,int[][] inputMatrix_green, int[][] inputMatrix_blue,int size) {
        double cellWidth = gc.getCanvas().getWidth() / size;
        double cellHeight = gc.getCanvas().getHeight() / size;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                double value_red = inputMatrix_red[i][j];
                double value_blue = inputMatrix_blue[i][j];
                double value_green = inputMatrix_green[i][j];
                Color color = getColorForValue(value_red,value_blue,value_green);
                gc.setFill(color);
                gc.fillRect(j * cellWidth, i * cellHeight, cellWidth, cellHeight);
            }
        }
    }

    private static Color getColorForValue(double value_red , double value_blue , double value_green) {

        double normalizedValue_red = value_red / 255.0;
        double normalizedValue_blue = value_blue / 255.0;
        double normalizedValue_green = value_green / 255.0;


        normalizedValue_red = Math.max(0.0, Math.min(1.0, normalizedValue_red));
        normalizedValue_blue = Math.max(0.0, Math.min(1.0, normalizedValue_blue));
        normalizedValue_green = Math.max(0.0, Math.min(1.0, normalizedValue_green));

        return Color.color(normalizedValue_red, normalizedValue_blue, normalizedValue_green);
    }
    public static void readMatrixFromFile(String filePath,int[][] mat1,int[][] mat2 , int[][] mat3) {
        try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) {
            System.out.println("reading red channel matrix with java:");
            for (int i = 0; i < MATRIX_SIZE; i++) {
                String[] values = reader.readLine().trim().split("\\s+");
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    mat1[i][j] = (int) Double.parseDouble(values[j]);
                    //System.out.print(inputMatrix_red[i][j]+" ");
                }
                //System.out.println();
            }
            System.out.println("reading blue channel matrix with java:");
            for (int i = 0; i < MATRIX_SIZE; i++) {
                String[] values = reader.readLine().trim().split("\\s+");
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    mat2[i][j] = (int) Double.parseDouble(values[j]);
                    //System.out.print(inputMatrix_blue[i][j]+" ");
                }
                //System.out.println();
            }
            System.out.println("reading green channel matrix with java:");
            for (int i = 0; i < MATRIX_SIZE; i++) {
                String[] values = reader.readLine().trim().split("\\s+");
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    mat3[i][j] = (int) Double.parseDouble(values[j]);
                    //System.out.print(inputMatrix_green[i][j]+" ");
                }
                //System.out.println();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void writeMatrixToFile(String filePath, int[][] matrix_red,int[][] matrix_blue,int[][] matrix_green) {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filePath))) {
            for (int i = 0; i < MATRIX_SIZE; i++) {
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    writer.write(matrix_red[i][j] + " ");
                }
                writer.newLine(); // Move to the next line for the next row
            }
            for (int i = 0; i < MATRIX_SIZE; i++) {
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    writer.write(matrix_blue[i][j] + " ");
                }
                writer.newLine(); // Move to the next line for the next row
            }
            for (int i = 0; i < MATRIX_SIZE; i++) {
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    writer.write(matrix_green[i][j] + " ");
                }
                writer.newLine(); // Move to the next line for the next row
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void random_fill(){
        Random random = new Random();
        red_matrix = new int[MATRIX_SIZE][MATRIX_SIZE];
        blue_matrix = new int[MATRIX_SIZE][MATRIX_SIZE];
        green_matrix = new int[MATRIX_SIZE][MATRIX_SIZE];
        for (int i = 0; i < MATRIX_SIZE; ++i) {
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                red_matrix[i][j] = random.nextInt(255);
                blue_matrix[i][j] = random.nextInt(255);
                green_matrix[i][j] = random.nextInt(255);// generating a random number between 0 and 254
            }
        }
    }


    public static void main(String[] args) throws IOException, InterruptedException {
        launch();
    }


    public static void compileAndRun() throws IOException, InterruptedException {
        try {

            String command_small = "g++ -o test G://untitled5//test.cpp -mavx -mavx2 && test";
            String command_big = "g++ -o beta G://untitled5//betaTest.cpp -mavx -mavx2 && beta";

            ProcessBuilder proc = new ProcessBuilder("cmd.exe", "/c", (MATRIX_SIZE >200 ) ? command_big:command_small);
            Process process = proc.start();

            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            String info;
            while ((info = reader.readLine()) != null) {
                System.out.println(info); // print the output of the c++ code
            }

            int exitCode = process.waitFor();// wait until the end of the process
            System.out.println("return value of the code : " + exitCode);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    public static void read_input(){
        Scanner scanner =new Scanner(System.in);
        System.out.println("enter the elements of red channel in the next "+MATRIX_SIZE*MATRIX_SIZE+" lines:");
        for(int i=0;i<MATRIX_SIZE;i++){
            for(int j=0;j<MATRIX_SIZE;j++){
                try {
                    String in = scanner.nextLine();
                    int x= Integer.parseInt(in);
                    red_matrix[i][j] = x;
                    if(x > 255) {
                        red_matrix[i][j] = 255;
                        System.out.println("this numbers is too big to be a color value,will be replaced with 255");
                    }
                    else if(x < 0) {
                        red_matrix[i][j] = 0;
                        System.out.println("this numbers is too small to be a color value,will be replaced with 0");
                    }
                }catch (Exception e){
                    System.out.println("error in regex match, this cell will be replaced with 0");
                    red_matrix[i][j] = 0;
                }
            }
        }
        blue_matrix = new int[MATRIX_SIZE][MATRIX_SIZE];
        System.out.println("do you want to enter the elements of blue channel too? [y/n]");
        String in = scanner.nextLine();
        if (in.equals("y")){
            System.out.println("enter the elements of blue channel in the next "+MATRIX_SIZE*MATRIX_SIZE+" lines:");
            for(int i=0;i<MATRIX_SIZE;i++){
                for(int j=0;j<MATRIX_SIZE;j++){
                    try {
                        in = scanner.nextLine();
                        int x= Integer.parseInt(in);
                        blue_matrix[i][j] = x;
                        if(x > 255) {
                            red_matrix[i][j] = 255;
                            System.out.println("this numbers is too big to be a color value,will be replaced with 255");
                        }
                        else if(x < 0) {
                            blue_matrix[i][j] = 0;
                            System.out.println("this numbers is too small to be a color value,will be replaced with 0");
                        }
                    }catch (Exception e){
                        System.out.println("error in regex match, this cell will be replaced with 0");
                        blue_matrix[i][j] = 0;
                    }
                }
            }
        }
        else{
            System.out.println("okay , all elements of this channel will be zero \u263A");
        }

        green_matrix = new int[MATRIX_SIZE][MATRIX_SIZE];

        System.out.println("do you want to enter the elements of green channel too? [y/n]");
        in = scanner.nextLine();
        if (in.equals("y")){
            System.out.println("enter the elements of blue channel in the next "+MATRIX_SIZE*MATRIX_SIZE+" lines:");
            for(int i=0;i<MATRIX_SIZE;i++){
                for(int j=0;j<MATRIX_SIZE;j++){
                    try {
                        in = scanner.nextLine();
                        int x= Integer.parseInt(in);
                        green_matrix[i][j] = x;
                        if(x > 255) {
                            green_matrix[i][j] = 255;
                            System.out.println("this numbers is too big to be a color value,will be replaced with 255");
                        }
                        else if(x < 0) {
                            green_matrix[i][j] = 0;
                            System.out.println("this numbers is too small to be a color value,will be replaced with 0");
                        }
                    }catch (Exception e){
                        System.out.println("error in regex match, this cell will be replaced with 0");
                        green_matrix[i][j] = 0;
                    }
                }
            }
        }
        else{
            System.out.println("okay , all elements of this channel will be zero \u263A");
        }
    }

}
