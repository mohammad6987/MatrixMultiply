#include "immintrin.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <winsock2.h>

using namespace std;

const int matrixSize = 16;
const int kernelSize = 3;
const int paddedMatrixSize = matrixSize + 2;

void initializeWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed." << endl;
        exit(1);
    }
}

SOCKET createListenSocket() {
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        cerr << "Error creating listen socket: " << WSAGetLastError() << endl;
        WSACleanup();
        exit(1);
    }
    return listenSocket;
}

void bindSocket(SOCKET listenSocket) {
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(125); // Listening port
    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed with error: " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        exit(1);
    }
}

void listenForConnections(SOCKET listenSocket) {
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed with error: " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        exit(1);
    }
}

SOCKET acceptClientSocket(SOCKET listenSocket) {
    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Accept failed with error: " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        exit(1);
    }
    return clientSocket;
}

void receiveMatrixFromClient(SOCKET clientSocket, float matrix[matrixSize][matrixSize]) {
    int totalBytesReceived = 0;
    int bytesReceived;
    while (totalBytesReceived < matrixSize*matrixSize * sizeof(float)) {
        bytesReceived = recv(clientSocket, reinterpret_cast<char*>(matrix) + totalBytesReceived, matrixSize*matrixSize * sizeof(float) - totalBytesReceived, 0);
        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Error receiving matrix: " << WSAGetLastError() << endl;
            closesocket(clientSocket);
            WSACleanup();
            exit(1);
        }
        totalBytesReceived += bytesReceived;
    }
}

void performConvolutionOnMatrix(float* flattedMatrix, float* flattenKernel, float result[matrixSize][matrixSize]) {
    __m128 kernel0 = _mm_loadu_ps(flattenKernel);
    __m128 kernel1 = _mm_loadu_ps(flattenKernel + 4);
    __m128 kernel2 = _mm_loadu_ps(flattenKernel + 8);
    __m128 row1, row2, row3, set1, set2, set3;
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            row1 = _mm_loadu_ps(flattedMatrix + (i) * paddedMatrixSize + j);
            row2 = _mm_loadu_ps(flattedMatrix + (i + 1) * paddedMatrixSize + j);
            row3 = _mm_loadu_ps(flattedMatrix + (i + 2) * paddedMatrixSize + j);
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

void sendMatrixToClient(SOCKET clientSocket, float matrix[matrixSize][matrixSize]) {
    int totalBytesSent = 0;
    int bytesSent;
    while (totalBytesSent < matrixSize*matrixSize * sizeof(float)) {
        bytesSent = send(clientSocket, reinterpret_cast<char*>(matrix) + totalBytesSent, matrixSize*matrixSize * sizeof(float) - totalBytesSent, 0);
        if (bytesSent == SOCKET_ERROR) {
            cerr << "Error sending matrix: " << WSAGetLastError() << endl;
            closesocket(clientSocket);
            WSACleanup();
            exit(1);
        }
        totalBytesSent += bytesSent;
    }
}

int main() {
    initializeWinsock();
    SOCKET listenSocket = createListenSocket();
    bindSocket(listenSocket);
    listenForConnections(listenSocket);

    cout << "Waiting for incoming connections..." << endl;

    while (true) {
        SOCKET clientSocket = acceptClientSocket(listenSocket);

        float matrix_red[matrixSize][matrixSize];
        float matrix_blue[matrixSize][matrixSize];
        float matrix_green[matrixSize][matrixSize];
        receiveMatrixFromClient(clientSocket, matrix_red);
        receiveMatrixFromClient(clientSocket, matrix_blue);
        receiveMatrixFromClient(clientSocket, matrix_green);

        float result_red[matrixSize][matrixSize];
        float result_blue[matrixSize][matrixSize];
        float result_green[matrixSize][matrixSize];
        auto* flattenKernel = (float*)malloc(12 * sizeof(float));
        const float kernel[kernelSize][kernelSize] = {
                {0.11, 0.11, 0.11},
                {0.11, 0.11, 0.11},
                {0.11, 0.11, 0.11}
        };

        for (int i = 0; i < 12; i++) {
            if (i % 4 != 3)
                flattenKernel[i] = kernel[i / 4][i % 4];
            else
                flattenKernel[i] = 0;
        }

        performConvolutionOnMatrix((float*)matrix_red, flattenKernel, result_red);
        performConvolutionOnMatrix((float*)matrix_blue, flattenKernel, result_blue);
        performConvolutionOnMatrix((float*)matrix_green, flattenKernel, result_green);

        sendMatrixToClient(clientSocket, result_red);
        sendMatrixToClient(clientSocket, result_blue);
        sendMatrixToClient(clientSocket, result_green);

        closesocket(clientSocket);
        free(flattenKernel);
    }

    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
