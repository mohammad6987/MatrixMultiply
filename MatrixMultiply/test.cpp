#include "immintrin.h"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
const int matrixSize = 32;
const int kernelSize = 3;
const int paddedMatrixSize = matrixSize + 2;

void readMatrixFromFile(const string& filename, float matrix_red[matrixSize][matrixSize],float matrix_blue[matrixSize][matrixSize],float matrix_green[matrixSize][matrixSize]) {
    ifstream inputFile(filename);
    if (inputFile.is_open()) {
        for (int i = 0; i < matrixSize; ++i) {//reading the red matrix from the file
            for (int j = 0; j < matrixSize; ++j) {
                inputFile >> matrix_red[i][j];
            }
        }
        for (int i = 0; i < matrixSize; ++i) {//reading the blue matrix from the file
            for (int j = 0; j < matrixSize; ++j) {
                inputFile >> matrix_blue[i][j];
            }
        }
        for (int i = 0; i < matrixSize; ++i) {//reading the green matrix from the file
            for (int j = 0; j < matrixSize; ++j) {
                inputFile >> matrix_green[i][j];
            }
        }
        inputFile.close();
    } else {
        cerr << "Unable to open file for reading: " << filename << endl;
        exit(1);
    }
}

void padMatrix(float* flattenedMatrix, float matrix[matrixSize][matrixSize]) {
    for (int i = 0; i < matrixSize; ++i) {// filling the cells that aren't in the rims with normal values like the main matrix
        for (int j = 0; j < matrixSize; ++j) {
            flattenedMatrix[paddedMatrixSize * (i + 1) + j + 1] = matrix[i][j];
        }
    }


    for (int i = 0; i < matrixSize; ++i) {// filling the left and right rims with the nearest neighborhood from the main matrix
        flattenedMatrix[(i + 1) * paddedMatrixSize] = matrix[i][0];
        flattenedMatrix[(i + 1) * paddedMatrixSize + paddedMatrixSize - 1] = matrix[i][matrixSize - 1];
    }


    for (int j = 0; j < matrixSize; ++j) { // filling the top and bottom rims like the last loop
        flattenedMatrix[j + 1] = matrix[0][j];
        flattenedMatrix[(paddedMatrixSize - 1) * paddedMatrixSize + j + 1] = matrix[matrixSize - 1][j];
    }

    flattenedMatrix[0] = matrix[0][0];// filling the corner values with corner values of main matrix
    flattenedMatrix[paddedMatrixSize - 1] = matrix[0][matrixSize - 1];
    flattenedMatrix[(paddedMatrixSize - 1) * paddedMatrixSize] = matrix[matrixSize - 1][0];
    flattenedMatrix[paddedMatrixSize * paddedMatrixSize - 1] = matrix[matrixSize - 1][matrixSize - 1];
}

void performConvolution(float* flattenedMatrix, float* flattenKernel, float result[matrixSize][matrixSize]) {
    __m128 kernel0 = _mm_loadu_ps(flattenKernel);//loading the first row of kernel in an xmm register
    __m128 kernel1 = _mm_loadu_ps(flattenKernel + 4);//loading the second row of kernel in another xmm register
    __m128 kernel2 = _mm_loadu_ps(flattenKernel + 8);// because the kernel is a float pointer we can add index to it not the size of memory that we want to access
    __m128 row1, row2, row3, set1, set2, set3;// defining 6 new xmm registers that will be used in next instructions

    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {//iterating over the cells of padded matrix
            row1 = _mm_loadu_ps(flattenedMatrix + (i) * paddedMatrixSize + j);//first row of a sub matrix is loaded in row1, the flattenMatrix is a float pointer so I used baseIndexed Addressing
            row2 = _mm_loadu_ps(flattenedMatrix + (i + 1) * paddedMatrixSize + j);//the next row address is the row1 address plus size of a row in paddedMatrix
            row3 = _mm_loadu_ps(flattenedMatrix + (i + 2) * paddedMatrixSize + j);//because the kernel is 3*4 and forth element is zero so the forth number in xmm row registers doesn't effect the result
            set1 = _mm_mul_ps(row1, kernel0);//multiplying sub matrix rows with kernel rows
            set2 = _mm_mul_ps(row2, kernel1);
            set3 = _mm_mul_ps(row3, kernel2);
            set1 = _mm_add_ps(set1, set2);//saving the sum of multiply results and calculating the sum of all of them
            set3 = _mm_add_ps(set3, set1);
            set3 = _mm_hadd_ps(set3, set3);// horizontal add to calculate the sum of every 4 number stored in xmm register
            set3 = _mm_hadd_ps(set3, set3);
            _mm_storeu_ps(&result[i][j], set3); // saving the final result in the result matrix cell
        }
    }
    /* asm volatile (
            "mov %[flattedMatrix] , %%r14\n"
            "lea %[result] , %%r12\n"
            "loop_1:\n"
                "xor %%rax, %%rax\n"
                "mov %[colCounter] , %%rax\n"
                "loop_2:\n"
            //load value from array to xmm
                    "add %[colCounter] , %%r15\n"
                    "movups (%%r14 , %r15) , %[row1]\n"
                    "add %[rowSize] , %%r15\n"
                    "movups (%%r14 , %%r15) , %[row1]\n"
                    "add %[rowSize] , %%r15\n"
                    "movups (%%r14 , %%r15) , %[row1]\n"
                    "sub %[rowSize] , %%r15\n"
                    "sub %[rowSize] , %%r15\n"
                    //mathematical instructions
                    "vmulps %[kernel0], %[row1] ,%[row1]\n"
                    "vmulps %[kernel1], %[row2] ,%[row2]\n"
                    "vmulps %[kernel2], %[row3] ,%[row3]\n"
                    "addps %[row1], %[row2]\n"
                    "addps %[row2], %[row3]\n"
                    "haddps %[row3], %[row3]\n"
                    "haddps %[row3], %[row3]\n"
                    //save value in resualt matrix
                    "movss %[row3] , (%%r12, %%r15)\n"
                    //loop conditions
                    "add $4, %[rowCounter]\n"
                    "cmp %[rowCounter] , %[mainBound]\n"
                    "jnz loop_2\n"
                "add $4, %[colCounter]\n"
                "cmp %[colCounter] , %[mainBound]\n"
                "jnz loop_1\n"


            : [result] "=m" (result)
            : [flattedMatrix] "m" (flattedMatrix), [kernel0] "x" (kernel0), [kernel1] "x" (kernel1), [kernel2] "x" (kernel2),[row1] "x" (row1), [row2] "x" (row2), [row3] "x" (row3),  [set3] "x" (set3),[rowCounter] "r" (0), [colCounter] "r" (0)  , [rowSize] "r" (4*paddedMatrixSize),[mainBound] "r" (4*matrixSize)
            :"%r14" , "%r15", "%r12"
    );*/
}

void writeMatrixToFile(const string& filename, float result_red[matrixSize][matrixSize],float result_blue[matrixSize][matrixSize],float result_green[matrixSize][matrixSize]) {
    ofstream outputFile(filename);
    if (outputFile.is_open()) {
        for (int i = 0; i < matrixSize; ++i) {//writing the red matrix to the file
            for (int j = 0; j < matrixSize; ++j) {
                outputFile << result_red[i][j] << " ";
            }
            outputFile << endl;
        }
        for (int i = 0; i < matrixSize; ++i) {//writing the blue matrix to the file
            for (int j = 0; j < matrixSize; ++j) {
                outputFile << result_blue[i][j] << " ";
            }
            outputFile << endl;
        }
        for (int i = 0; i < matrixSize; ++i) {//writing the green matrix to the file
            for (int j = 0; j < matrixSize; ++j) {
                outputFile << result_green[i][j] << " ";
            }
            outputFile << endl;
        }
        outputFile.close();
    } else {
        cout << "Unable to open file for writing: " << filename << endl;
    }
}

int main() {
    float matrix_red[matrixSize][matrixSize];// defining the color matrixes
    float matrix_blue[matrixSize][matrixSize];
    float matrix_green[matrixSize][matrixSize];

    // Read input matrices from files
    readMatrixFromFile("G://untitled5//input.txt", matrix_red , matrix_blue , matrix_green);// reading values from file and storing in color matrixes


    // Pad matrices
    float flattenedMatrix_red[paddedMatrixSize * paddedMatrixSize];//padded matrixes to handle the image processing correctly
    float flattenedMatrix_blue[paddedMatrixSize * paddedMatrixSize];
    float flattenedMatrix_green[paddedMatrixSize * paddedMatrixSize];

    padMatrix(flattenedMatrix_red, matrix_red);
    padMatrix(flattenedMatrix_blue, matrix_blue);
    padMatrix(flattenedMatrix_green, matrix_green);

    // Define kernel
    float kernel[kernelSize][kernelSize] ;
    ifstream inputFile("G://untitled5//kernel.txt");
    if (inputFile.is_open()) {
        for (int i = 0; i < kernelSize; ++i) {//reading the red matrix from the file
            for (int j = 0; j < kernelSize; ++j) {
                inputFile >> kernel[i][j];
            }
        }

        inputFile.close();
    } else {
        cerr << "Unable to open file for reading: " << "G://untitled5//kernel.txt" << endl;
        exit(1);
    }

    auto* flattenKernel = (float*)malloc(12 * sizeof(float));//converting the kernel to a flat array that is filled at some points with zero
    for (int i = 0; i < 12; i++) {
        if (i % 4 != 3)
            flattenKernel[i] = kernel[i / 4][i % 4];
        else
            flattenKernel[i] = 0;
    }


    float result_red[matrixSize][matrixSize];//the result of each color channel will be saved in this matrixes
    float result_blue[matrixSize][matrixSize];
    float result_green[matrixSize][matrixSize];
    auto start = chrono::high_resolution_clock::now();//starting rhe timer
    //for(int i=0;i<100000;i++)
    performConvolution(flattenedMatrix_red, flattenKernel, result_red);//performing the convolution on red channel matrix
    performConvolution(flattenedMatrix_blue, flattenKernel, result_blue);
    performConvolution(flattenedMatrix_green, flattenKernel, result_green);
    auto stop  = chrono::high_resolution_clock::now();// stopping the timer
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop -start);
    printf("%lld ns \n" , duration.count());
    // Write result matrices to files
    writeMatrixToFile("G://untitled5//output.txt", result_red , result_blue , result_green);// writing the results in another file that will be read java file

    return 0;
}
