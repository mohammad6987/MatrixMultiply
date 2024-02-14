#include "immintrin.h"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

const int kernelSize = 3;

void writeMatrixToFile(const string& filename, float** result_red , float** result_blue ,float** result_green , int matrixSize) {
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

void readMatrixFromFile(const string& filename, float** matrix_red,float** matrix_blue,float** matrix_green , int matrixSize) {
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

void readOrWriteMatrixFromFile(const string& filename, float** matrix, int matrixSize, bool read) {
    ifstream inputFile;
    ofstream outputFile;
    if (read) {
        inputFile.open(filename);
        if (!inputFile.is_open()) {
            cerr << "Unable to open file for reading: " << filename << endl;
            exit(1);
        }
    } else {
        outputFile.open(filename);
        if (!outputFile.is_open()) {
            cerr << "Unable to open file for writing: " << filename << endl;
            exit(1);
        }
    }

    if (read) {
        for (int i = 0; i < matrixSize; ++i) {
            for (int j = 0; j < matrixSize; ++j) {
                inputFile >> matrix[i][j];
            }
        }
        inputFile.close();
    } else {
        for (int i = 0; i < matrixSize; ++i) {
            for (int j = 0; j < matrixSize; ++j) {
                outputFile << matrix[i][j] << " ";
            }
            outputFile << endl;
        }
        outputFile.close();
    }
}

void padMatrix(float** flattenedMatrix, float** matrix, int matrixSize) {
    int paddedMatrixSize = matrixSize + 2;
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            flattenedMatrix[i + 1][j + 1] = matrix[i][j];
        }
    }

    for (int i = 0; i < matrixSize; ++i) {
        flattenedMatrix[i + 1][0] = matrix[i][0];
        flattenedMatrix[i + 1][paddedMatrixSize - 1] = matrix[i][matrixSize - 1];
    }

    for (int j = 0; j < matrixSize; ++j) {
        flattenedMatrix[0][j + 1] = matrix[0][j];
        flattenedMatrix[paddedMatrixSize - 1][j + 1] = matrix[matrixSize - 1][j];
    }

    flattenedMatrix[0][0] = matrix[0][0];
    flattenedMatrix[0][paddedMatrixSize - 1] = matrix[0][matrixSize - 1];
    flattenedMatrix[paddedMatrixSize - 1][0] = matrix[matrixSize - 1][0];
    flattenedMatrix[paddedMatrixSize - 1][paddedMatrixSize - 1] = matrix[matrixSize - 1][matrixSize - 1];
}

void performConvolution(float** flattenedMatrix, float* flattenKernel, float** result, int matrixSize) {
    int paddedMatrixSize = matrixSize + 2;
    __m128 kernel0 = _mm_loadu_ps(flattenKernel);
    __m128 kernel1 = _mm_loadu_ps(flattenKernel + 4);
    __m128 kernel2 = _mm_loadu_ps(flattenKernel + 8);
    __m128 row1, row2, row3, set1, set2, set3;

    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            row1 = _mm_loadu_ps(&flattenedMatrix[i][j]);
            row2 = _mm_loadu_ps(&flattenedMatrix[i + 1][j]);
            row3 = _mm_loadu_ps(&flattenedMatrix[i + 2][j]);
            set1 = _mm_mul_ps(row1, kernel0);
            set2 = _mm_mul_ps(row2, kernel1);
            set3 = _mm_mul_ps(row3, kernel2);
            set1 = _mm_add_ps(set1, set2);
            set3 = _mm_add_ps(set3, set1);
            set3 = _mm_hadd_ps(set3, set3);
            set3 = _mm_hadd_ps(set3, set3);
            _mm_storeu_ps(&result[i][j], set3);
        }
    }
}
void readSizeFromFile(const string& filename ,  int matrixSize) {
    ifstream inputFile(filename);
    if (inputFile.is_open()) {
        inputFile >> matrixSize;
        inputFile.close();
    } else {
        cerr << "Unable to open file for reading: " << filename << endl;
        exit(1);
    }
}

int main() {
    int matrixSize = 64 ;
    //readSizeFromFile("G://untitled5//size.txt" , matrixSize);
    float** matrix_red = new float*[matrixSize];
    float** matrix_blue = new float*[matrixSize];
    float** matrix_green = new float*[matrixSize];
    for (int i = 0; i < matrixSize; ++i) {
        matrix_red[i] = new float[matrixSize];
        matrix_blue[i] = new float[matrixSize];
        matrix_green[i] = new float[matrixSize];
    }

    readMatrixFromFile("G://untitled5//input.txt" , matrix_red , matrix_blue , matrix_green , matrixSize);

    float** flattenedMatrix_red = new float*[matrixSize + 2];
    float** flattenedMatrix_blue = new float*[matrixSize + 2];
    float** flattenedMatrix_green = new float*[matrixSize + 2];
    for (int i = 0; i < matrixSize + 2; ++i) {
        flattenedMatrix_red[i] = new float[matrixSize + 2];
        flattenedMatrix_blue[i] = new float[matrixSize + 2];
        flattenedMatrix_green[i] = new float[matrixSize + 2];
    }

    padMatrix(flattenedMatrix_red, matrix_red, matrixSize);
    padMatrix(flattenedMatrix_blue, matrix_blue, matrixSize);
    padMatrix(flattenedMatrix_green, matrix_green, matrixSize);

    float kernel[kernelSize][kernelSize];
    ifstream inputFile("G://untitled5//kernel.txt");
    if (inputFile.is_open()) {
        for (int i = 0; i < kernelSize; i++) {
            for (int j = 0; j < kernelSize; j++) {
                inputFile >> kernel[i][j];
            }
        }
        inputFile.close();
    } else {
        cerr << "Unable to open file for reading: " << "G://untitled5//kernel.txt" << endl;
        exit(1);
    }

    auto* flattenKernel = new float[12];
    for (int i = 0; i < 12; i++) {
        if (i % 4 != 3)
            flattenKernel[i] = kernel[i / 4][i % 4];
        else
            flattenKernel[i] = 0;
    }

    float** result_red = new float*[matrixSize];
    float** result_blue = new float*[matrixSize];
    float** result_green = new float*[matrixSize];
    for (int i = 0; i < matrixSize; ++i) {
        result_red[i] = new float[matrixSize];
        result_blue[i] = new float[matrixSize];
        result_green[i] = new float[matrixSize];
    }

    auto start = chrono::high_resolution_clock::now();
    performConvolution(flattenedMatrix_red, flattenKernel, result_red, matrixSize);
    performConvolution(flattenedMatrix_blue, flattenKernel, result_blue, matrixSize);
    performConvolution(flattenedMatrix_green, flattenKernel, result_green, matrixSize);
    auto stop  = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
    cout << duration.count() << " ns" << endl;

    writeMatrixToFile("G://untitled5//output.txt" , result_red , result_blue , result_green , matrixSize);

    for (int i = 0; i < matrixSize; ++i) {
        delete[] matrix_red[i];
        delete[] matrix_blue[i];
        delete[] matrix_green[i];
        delete[] result_red[i];
        delete[] result_blue[i];
        delete[] result_green[i];
    }
    for (int i = 0; i < matrixSize + 2; ++i) {
        delete[] flattenedMatrix_red[i];
        delete[] flattenedMatrix_blue[i];
        delete[] flattenedMatrix_green[i];
    }
    delete[] matrix_red;
    delete[] matrix_blue;
    delete[] matrix_green;
    delete[] result_red;
    delete[] result_blue;
    delete[] result_green;
    delete[] flattenKernel;

    return 0;
}
