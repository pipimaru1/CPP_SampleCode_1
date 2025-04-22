
//空のプロジェクト作成

// cpp_basics_sample.cpp
// C++の基本文法を学ぶためのサンプルコード（ポインタ除く）
#include <iostream>
#include <string>
#include <vector>

// 関数の宣言
int add(int a, int b);
void printVector(const std::vector<int>& vec);


////////////////////////////////////////////////////
//エントリーポイント 
int main()
{
    // 1. 変数と基本型
    int a = 10;
    double b = 3.14;
    bool flag = true;
    char c = 'X';
    std::string message = "Hello, C++!";

    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "flag = " << flag << std::endl;
    std::cout << "c = " << c << std::endl;
    std::cout << "message = " << message << std::endl;

    // 2. if 文と条件分岐
    if (a > 5)
    {
        std::cout << "a is greater than 5" << std::endl;
    }
    else
    {
        std::cout << "a is 5 or less" << std::endl;
    }

    // 3. for ループ
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "i = " << i << std::endl;
    }

    // 4. while ループ
    int count = 0;
    while (count < 3)
    {
        std::cout << "count = " << count << std::endl;
        ++count;
    }

    // 5. 配列とベクター
    int arr[3] = { 1, 2, 3 };
    std::vector<int> vec = { 4, 5, 6 };

    std::cout << "arr[1] = " << arr[1] << std::endl;
    printVector(vec);

    // 6. 関数の利用
    int sum = add(7, 8);
    std::cout << "7 + 8 = " << sum << std::endl;

    return 0;
}

// 7. 関数の定義
int add(int a, int b)
{
    return a + b;
}

void printVector(const std::vector<int>& vec)
{
    std::cout << "Vector contents: ";
    for (int v : vec)
    {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}
