#include <iostream>
#include <stdio.h>
#include <regex.h>
#include <string>
#include <chrono>
using namespace std;
float regexHandler() {// checking if the input matches a float regex or not *1
    string input;
    getline(cin, input);

    regex_t regex;
    int val = regcomp(&regex, "^[-+]?[0-9]+\\.?[0-9]+?$", REG_EXTENDED);// possible +/- *2
    // at least 1 number before pricision point
    // unknown numbers after pricision point

    if (val == 0) {
        val = regexec(&regex, input.c_str(), 0, NULL, 0);

        if (val == 0) {
            try {
                return stof(input);    //converting string to float *3
            } catch (const exception& e) {
                printf("استاد به مشکل خوردیم:\nمجبور شدیم بجاش صفر بذاریم\n ");
                printf(e.what());// catching the errors *4
            }
        } else {
            cout <<  " رکب زدی استاد\nبجاش صفر گذاشتم" << endl;
        }

    } else {
        cout << "رکب زدی استاد\nبجاش صفر گذاشتم" << endl;
    }

    return 0.0;
}
void avx_3(){
    float* mat1 = new float[12]();	  // creating 2 float pointers that will be used as a 3*4 matrix and 4*3 matrix
    float* mat2 = new float[12]();
    printf("enter the elements of first matrix in row order:\n");//reading user input for first matrix *6
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            printf("[ %d , %d ] : ",i,j);
            mat1[4*i+j] = regexHandler();
        }

    }
    printf("\nenter the elements of second matrix in col order:\n");//reading user input for the second matrix
    for(int j=0;j<3;j++){
        for(int i=0;i<3;i++){
            printf("[ %d , %d ] : ",i,j);
            mat2[4*j+i] = regexHandler();
        }

    }
    float* mat3 = new float[9]();//initilizing the result matrix (3*3) *7
    auto start = chrono::high_resolution_clock::now();
    asm volatile (
            "mov %[mat1], %%r12\n" // loading address of first element of each matrix in 64-bit registers *8
            "mov %[mat2], %%r14\n"
            "mov %[mat3], %%r15\n"
            "xor %%rcx, %%rcx\n" // changing rcx/ecx to zero , will be used as loop counter *9
            "loop_row_1:\n"      // looping over the elements in a row of resulat matrix *10
            "xor %%edx, %%edx\n" // loadign edx with zero,also will be used as loop counter for the second nested loop *11
            "xor %%rax , %%rax\n" // dummy value for eiseier access to elements of result matrix *12
            "loop_col_1:\n" // looping over the elements in a col of resulat matrix *13
            "movups (%%r12, %%rcx), %%xmm1\n"//loading a row of first matrix in xmm1,or moving 4 single pricision numbers*14
            "movups (%%r14, %%rdx), %%xmm2\n"//loading a row of second  matrix in xmm2 *15
            "vmulps %%xmm1, %%xmm2, %%xmm1\n"//taking sum of the xmm1 and xmm2 and loading the result in xmm2 again *16
            "haddps %%xmm1, %%xmm1\n"//horizantal add to reduce the numbers inside xmm from 4 to 2 *17
            "haddps %%xmm1, %%xmm1\n"//horizantal add to reduce the numbers inside xmm from 2 to 1 *18
            "add %%rcx , %%rax\n"    // rax points to the num of col and rcx points to the num of row*16 *19
            "movss %%xmm1, (%%r15, %%rax)\n"// loading the single value stored in xmm1 to the mat3 [ecx/4 , edx/4] *20
            "sub %%rcx , %%rax\n" // returning rax to normal value *21
            "add $4 , %%rax\n" // using rax as a pointer to the j where j is form mat[i,j] *22
            "add  $16,%%edx\n" // passing to the next col in second matrix or moving to the next 4 32-bit numbers *23
            "cmp $48, %%edx\n" // checking if all cols are multiplyed with a certian row of first matrix *24
            "jl loop_col_1\n"
            "add $16, %%ecx\n" // moving to the next row of first matrix *25
            "cmp $48, %%ecx\n" // checking if all rows are checked or not *26
            "jl loop_row_1\n"
            : [mat3] "=m" (mat3)// output operands
    : [mat1] "m" (mat1), [mat2] "m" (mat2)// input operands
    : "%xmm1", "%xmm2","%r12","%r14", "%r15","%rax","%rcx","%rdx", "%ecx", "%edx","%eax"// registers that are used during the code
    );
    auto stop  = chrono::high_resolution_clock::now();// stopping the timer
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop -start);

    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++)
            printf("%f ", mat3[4*i+j]);
        printf("\n");
    }
    printf("time with avx : %lld ns\n" , duration.count());
    float normal_result[3][3];
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {

            for (int k = 0; k < 3; k++) {
                normal_result[i][j] += mat1[4*i+k] * mat2[4*j+k];
            }
        }
    }
    stop  = chrono::high_resolution_clock::now();// stopping the timer
    duration = chrono::duration_cast<chrono::nanoseconds>(stop -start);
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++)
            printf("%f ", normal_result[i][j]);
        printf("\n");
    }
    printf("time with normal instructions : %lld ns\n" , duration.count());




}
void avx_5(){
    float* mat1 = new float[40]();	  // creating 2 float pointers that will be used as two 5*8 and 8*5 matrixes *27
    float* mat2 = new float[40]();	  // the 5*3 sum matrix is loaded with zero tho nullify the echo of loading *28
    // additional numbers to xmm registers
    printf("enter the elements of first matrix in row order:\n");// reading input from user for the first matrix
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
            printf("[ %d , %d ] : ",i,j);
            mat1[8*i+j] = regexHandler();
        }

    }
    printf("enter the elements of second matrix in row order:\n");//redaing input from user for the second matrix
    for(int j=0;j<5;j++){
        for(int i=0;i<5;i++){
            printf("[ %d , %d ] : ",i,j);
            mat2[8*j+i] = regexHandler();
        }

    }
    float* mat3 = new float[25]();// initilizing the product matrix
    auto start = chrono::high_resolution_clock::now();
    asm volatile (
            "mov %[mat1], %%r12\n"// loading address of matrixes inside register,becuase they are pointers *29
            "mov %[mat2], %%r14\n"// they are address of memory so we should use "mov" instead of "lea"
            "mov %[mat3], %%r15\n"
            "xor %%ecx, %%ecx\n"//using rcx as counter of rows
            "loop_col_2:\n"//looping over the rows of result matrix
            "xor %%edx, %%edx\n"//using edx as col counter that will be reseted after each iteration over a row
            "xor %%rax , %%rax\n"// using rax as a dummy value to
            "loop_row_2:\n"// looping over the cols
            "vmovups (%%r12, %%rcx), %%xmm1\n"// loading the first 4 elements of a row in first matrix *30
            "vmovups (%%r14, %%rdx), %%xmm2\n"// loading the first 4 elements of a col in first matrix *31
            "mov %%rcx , %%rbx\n"// loading address of current row in rbx or address of first element of this row *32
            //"add $16 , %%rbx\n"// increasing rbx with 16 to point the the second 4 elements of the row *33
            "vmovups 16(%%r12, %%rbx), %%xmm0\n"// loading the second 4 elements of a row in first matrix *34
            "mov %%rdx , %%rbx\n"// loading address of current col in rbx or address of first element of this col*35
            //"add $16 , %%rbx\n"// increasing rbx with 16 to point the the second 4 elements of the col *36
            "vmovups 16(%%r14, %%rbx), %%xmm3\n"// loading the second 4 elements of a col in second matrix *37
            // xmm1 and xmm0 have the elements of the row in order
            // xmm2 and xmm3 have the elements of the col in order
            "vmulps %%xmm1, %%xmm2, %%xmm1\n"//multiplying the first 4 elements of row and col and storing in xmm1 *38
            "vmulps %%xmm0, %%xmm3, %%xmm0\n"//multiplying the second 4 elements of row and col and storing in xmm0 *39
            "vaddps %%xmm0 , %%xmm1 , %%xmm1\n"//adding the last line results togethere and saving the sum in xmm1 *40
            "haddps %%xmm1, %%xmm1\n"//doing horizantal add 2 times to get the final sum *41
            "haddps %%xmm1, %%xmm1\n"// and putting it in a cell of result matrix
            "add %%rcx , %%rax\n"//converting rax to a pointer to the cell that we want to store the result *42
            "movss %%xmm1, (%%r15, %%rax)\n"// moving the result sum to the result[ecx/4 , edx/4 ] *43
            "sub %%rcx , %%rax\n"//returing rax to a rdx/4 *44
            "add $4 , %%rax\n"//moving to the next float number in a col *45
            "add  $32,%%edx\n"//moving to the next col or the next 8 4-byte numbers *46
            "cmp $160, %%edx\n"//checking if we have reached the end of cols or not
            "jl loop_row_2\n"
            "add $32, %%ecx\n"//moving to the next row or the next 8 4-byte numbers *47
            "cmp $160, %%ecx\n"//checking if all rows are checked or not
            "jl loop_col_2\n"
            : [mat3] "=m" (mat3)// output operands
    : [mat1] "m" (mat1), [mat2] "m" (mat2)// input operands
    : "%xmm0", "%xmm1", "%xmm2" , "%xmm3","%r12","%r13", "%r14", "%r15", "%ecx", "%edx","%eax","%ebx"// registers that are used
    );
    auto stop  = chrono::high_resolution_clock::now();// stopping the timer
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop -start);
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++)
            printf("%f ", mat3[8*i+j]);
        printf("\n");
    }
    printf("time with avx : %lld ns\n" , duration.count());
    float normal_result[5][5];
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {

            for (int k = 0; k < 5; k++) {
                normal_result[i][j] += mat1[8*i+k] * mat2[8*j+k];
            }
        }
    }
    stop  = chrono::high_resolution_clock::now();// stopping the timer
    duration = chrono::duration_cast<chrono::nanoseconds>(stop -start);
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++)
            printf("%f ", normal_result[i][j]);
        printf("\n");
    }
    printf("time with normal instructions : %lld ns\n" , duration.count());
}

int main() {
    printf("enter the size of the matrix:\n");
    int n;
    scanf("%d" , &n);
    getchar();
    if (n == 5)
        avx_5();
    else if(n == 3)
        avx_3();
    else
        printf("Aziat nakon");
    return 0;
}
