package com.example.demo;
import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import java.io.*;

import static com.example.demo.Convertor.*;

public class GUI extends Application{
    private final Label copyrightL = new Label("please try not to copy this program code \u263A");
    public static boolean alreadyFilled = false;
    public static boolean checkErrors = false;
    float[][] sameKernel = {//defining the kernel that will be used during convolution
            {0f, 0f, 0f},
            {0f, 1f, 0f},
            {0f, 0f, 0f}
    };
    float[][] sharpKernel = {//defining the kernel that will be used during convolution
            {0f, -1f, 0f},
            {-1f, 5f, -1f},
            {0f, -1f, 0f}
    };
    float[][] sharperKernel = {//defining the kernel that will be used during convolution
            {0f, -1f, 0f},
            {-1f, 8f, -1f},
            {0f, -1f, 0f}
    };
    float[][] fadeKernel = {//defining the kernel that will be used during convolution
            {0.1111f, 0.1111f, 0.1111f},
            {0.1111f, 0.1111f, 0.1111f},
            {0.1111f, 0.1111f, 0.1111f}
    };
    @Override
    public void start(Stage stage) throws Exception {
        String buttonStyle = "-fx-background-color: transparent; -fx-border-color: white;-fx-font-weight: bold; -fx-border-radius: 25px; -fx-text-fill: white;";
        writeKernel(sameKernel , "G://untitled5/kernel.txt");
        HBox root  = new HBox();
        root.setSpacing(25);
        root.setStyle("-fx-background-color: black;");
        VBox buttonsRoot = new VBox();
        buttonsRoot.setAlignment(Pos.TOP_CENTER);
        root.getChildren().add(buttonsRoot);
        root.setPadding(new Insets(5,15,15,15));
        buttonsRoot.setPadding(new Insets(25,25,25,25));
        buttonsRoot.setSpacing(35);
        buttonsRoot.setBorder( new Border(new BorderStroke(Color.WHITE , BorderStrokeStyle.SOLID , new CornerRadii(25) , new BorderWidths(5))));
        MenuButton menuButton =new MenuButton();
        menuButton.setAlignment(Pos.TOP_CENTER);
        menuButton.setStyle("-fx-background-color: transparent; -fx-border-color: white;-fx-font-weight: bold; -fx-border-radius: 25px; -fx-text-fill: white;");
        menuButton.setMinWidth(180);
        menuButton.setText("normal kernel");
        MenuItem fade = new MenuItem("fade kernel");
        MenuItem same = new MenuItem("same kernel");
        MenuItem sharp = new MenuItem("sharp kernel");
        MenuItem sharper = new MenuItem("sharper kernel");
        menuButton.getItems().addAll(same , fade , sharp ,sharper);
        Button button = new Button("Choose the Image");
        button.setStyle(buttonStyle);
        button.setMinWidth(180);
        Button randomFill = new Button("Filling the pixels randomly");
        randomFill.setMinWidth(180);
        randomFill.setStyle(buttonStyle);
        Button compileAndRun = new Button("Compile and Run");
        compileAndRun.setMinWidth(180);
        compileAndRun.setStyle(buttonStyle);
        Button manualAdd = new Button("Writing values manually");
        manualAdd.setMinWidth(180);
        manualAdd.setStyle(buttonStyle);
        buttonsRoot.getChildren().add(menuButton);
        buttonsRoot.getChildren().add(button);
        buttonsRoot.getChildren().add(randomFill);
        buttonsRoot.getChildren().add(manualAdd);
        buttonsRoot.getChildren().add(compileAndRun);
        Canvas selectedImage = new Canvas(400, 400);
        ImageView imageView = new ImageView();
        fade.setOnAction(e -> {
            menuButton.setText("fade kernel");
            writeKernel(fadeKernel , "G://untitled5/kernel.txt");
        });
        sharp.setOnAction(e -> {
            menuButton.setText("sharp kernel");
            writeKernel(sharpKernel , "G://untitled5/kernel.txt");
        });
        sharper.setOnAction( e-> {
            menuButton.setText("sharper kernel");
            writeKernel(sharperKernel , "G://untitled5/kernel.txt");
        });
        same.setOnAction(e -> {
            menuButton.setText("same kernel");
            writeKernel(sameKernel , "G://untitled5/kernel.txt");
        });
        button.setOnAction(e -> {
            FileChooser fileChooser = new FileChooser();
            fileChooser.setInitialDirectory(new File(System.getProperty("user.home")));
            fileChooser.getExtensionFilters().add(
                    new FileChooser.ExtensionFilter("Image Files", "*.png", "*.jpg")
            );

            File selectedFile = fileChooser.showOpenDialog(stage);

            if (selectedFile != null) {
                if(alreadyFilled){
                    Alert alert = new Alert(Alert.AlertType.ERROR);
                    alert.setContentText("you have already selected something!");
                    alert.show();
                }
                else {
                    System.out.println("Selected image file: " + selectedFile.getAbsolutePath());
                    Image image = new Image(selectedFile.toURI().toString());
                    try {
                        Convertor.imageConvertor(image.getUrl());
                    } catch (Exception Ex) {
                        Alert alert = new Alert(Alert.AlertType.ERROR);
                        alert.setContentText("this image can not be processed wit current matrix size!");
                        alert.show();
                        alreadyFilled = false;
                        checkErrors = true;
                    }
                    if (!alreadyFilled && !checkErrors) {
                        root.getChildren().add(selectedImage);
                        alreadyFilled = true;
                        Convertor.writeMatrixToFile("G:\\untitled5\\input.txt", red_matrix, blue_matrix, green_matrix);
                        imageView.setImage(image);
                        Convertor.drawMatrix(selectedImage.getGraphicsContext2D(), red_matrix, blue_matrix, green_matrix, MATRIX_SIZE);
                    }
                }
            } else {
                System.out.println("No image file selected.");
                Alert alert = new Alert(Alert.AlertType.ERROR);
                alert.setContentText("You didn't choose anything!");
                alert.show();
            }
        });
        randomFill.setOnMouseClicked(e -> {
            Convertor.random_fill();
            Convertor.writeMatrixToFile("G:\\untitled5\\input.txt", red_matrix, blue_matrix, green_matrix);
            Convertor.drawMatrix(selectedImage.getGraphicsContext2D(), red_matrix,blue_matrix,green_matrix,MATRIX_SIZE);
            if(!alreadyFilled) {
                root.getChildren().add(selectedImage);
                alreadyFilled = true;
            }
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText("the pixels are now filled randomly!");
            alert.show();
        });
        manualAdd.setOnAction( e -> {
            Convertor.read_input();
            Convertor.writeMatrixToFile("G:\\untitled5\\input.txt", red_matrix, blue_matrix, green_matrix);
            Convertor.drawMatrix(selectedImage.getGraphicsContext2D(), red_matrix,blue_matrix,green_matrix,MATRIX_SIZE);
            if(!alreadyFilled) {
                root.getChildren().add(selectedImage);
                alreadyFilled = true;
            }
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText("you entered all elements!\nnew lines doesn't do any thing,stop typing");
            alert.show();
        });
        compileAndRun.setOnMouseClicked(e -> {
            if(!alreadyFilled){
                Alert alert = new Alert(Alert.AlertType.ERROR);
                alert.setContentText("nothing has been selected!");
                alert.show();
            }
            else {
                stage.close();
                try {
                    Convertor.compileAndRun();
                } catch (IOException | InterruptedException ex) {
                    throw new RuntimeException(ex);
                }
                new Convertor().start(stage); // Call HelloApplication.main() with an empty string array as argument
            }
        });
        buttonsRoot.getChildren().add(copyrightL);
        copyrightL.setTextFill(Color.WHITE);
        stage.setScene(new Scene(root, 700, 460));
        stage.show();
    }


    public static void main(String[] args) throws IOException, InterruptedException {

        launch();
        Convertor.compileAndRun();
    }
    public static void writeKernel(float[][] kernel , String filePath){
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filePath))) {
            for (float[] floats : kernel) {
                for (float aFloat : floats) {
                    writer.write(aFloat + " ");
                }
                writer.newLine(); // Move to the next line for the next row
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void writeSize(String filePath){
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filePath))) {
            writer.write(MATRIX_SIZE);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
