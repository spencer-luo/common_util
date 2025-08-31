#include "common.hpp"
#include "common_util/print.h"

void PrintArr()
{
    PrintSubTitle("print_arr");

    int8_t arr[] = { 1, -2, 3 };
    cutl::print_arr(arr, 3);
}

void PrintVec()
{
    PrintSubTitle("print_vec");

    std::vector<int> vec1 = { 1, 2, -3, 4 };
    cutl::print_vec(vec1);
    std::vector<float> vec2 = { 3.14, -2.56, 5.122 };
    cutl::print_vec(vec2);
}

void PrintMap()
{
    PrintSubTitle("print_map");

    std::map<std::string, int> map1 = { { "apple", 1 }, { "orange", 3 }, { "banana", 2 } };
    cutl::print_map(map1);
    cutl::print_map(map1, true);
}

void PrintUnorderedMap()
{
    PrintSubTitle("print_unordered_map");

    std::unordered_map<std::string, int> map1 = {
        { "apple", 1 },
        { "orange", 3 },
        { "banana", 2 },
    };
    cutl::print_unordered_map(map1);
    cutl::print_unordered_map(map1, true);
}

void PrintColor()
{
    PrintSubTitle("print_clr");

    // 彩色颜色打印
    cutl::print_clr("red: Hello World", cutl::cli_clr_red);
    cutl::print_clr("bright_red: Hello World", cutl::cli_clr_bright_red);
    cutl::print_clr("green: Hello World", cutl::cli_clr_green);
    cutl::print_clr("bright_green: Hello World", cutl::cli_clr_bright_green);
    cutl::print_clr("blue: Hello World", cutl::cli_clr_blue);
    cutl::print_clr("bright_blue: Hello World", cutl::cli_clr_bright_blue);
    cutl::print_clr("yellow: Hello World", cutl::cli_clr_yellow);
    cutl::print_clr("bright_yellow: Hello World", cutl::cli_clr_bright_yellow);
    cutl::print_clr("purple: Hello World", cutl::cli_clr_purple);
    cutl::print_clr("bright_purple: Hello World", cutl::cli_clr_bright_purple);
    cutl::print_clr("cyan: Hello World", cutl::cli_clr_cyan);
    cutl::print_clr("bright_cyan: Hello World", cutl::cli_clr_bright_cyan);
    cutl::print_clr("white: Hello World", cutl::cli_clr_white);
    cutl::print_clr("gray: Hello World", cutl::cli_clr_gray, cutl::cli_clr_blue);
    cutl::print_clr("deep_gray: Hello World", cutl::cli_clr_deep_gray, cutl::cli_clr_white);
    cutl::print_debug("Debug: Hello World");
    cutl::print_info("Info: Hello World");
    cutl::print_warn("Warn: Hello World");
    cutl::print_error("Error: Hello World");
    cutl::print_success("Success: Hello World");
    // 正常打印
    std::cout << "norman: Hello World" << std::endl;
}

// (用随机数)初始化矩阵
void initializeMatrix(float* matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            matrix[i * cols + j] = (static_cast<float>(rand()) / RAND_MAX) * 10;
        }
    }
}

// (用随机数)初始化矩阵
void initializeMatrix(int8_t* matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            matrix[i * cols + j] = i * cols + j;
        }
    }
}

void PrintIntMatrix()
{
    int M = 10;
    int N = 12;
    int8_t matrix[M * N] = { 0 };
    // 初始化矩阵
    initializeMatrix(matrix, M, N);
    // 打印矩阵
    cutl::print_int_matrix(matrix, M, N, "int matrix");
    cutl::print_int_matrix(matrix, M, N, "int matrix", 5, 5, M - 5, N - 5);
}

void PrintFloatMatrix()
{
    int M = 10;
    int N = 12;
    float matrix[M * N] = { 0 };
    // 初始化矩阵
    initializeMatrix(matrix, M, N);
    // 打印矩阵
    cutl::print_float_matrix(matrix, M, N, "float matrix");
    cutl::print_float_matrix(matrix, M, N, "float matrix", 1, 6, 8, M - 6, N - 8);
}

void TestPrint()
{
    PrintTitle("print");

    PrintColor();
    PrintArr();
    PrintVec();
    PrintMap();
    PrintUnorderedMap();
    PrintIntMatrix();
    PrintFloatMatrix();
}
