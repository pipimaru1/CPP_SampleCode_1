//ポインタと参照の参照のサンプル
#include <conio.h>
#include <iostream>
#include <vector>

void printVector(const std::vector<int>& vec);
void AddDataVector_Addr(std::vector<int>& vec, int _num);
void AddDataVector_Val(std::vector<int> vec, int _num);

int main()
{
	std::vector<int> vec = { 0, 1, 2 };
	
	std::cout << "\n///////////////////////////////////////////\n";
	printVector(vec);

	_getch(); // ユーザーがキーを押すまで待機
	std::cout << "\n///////////////////////////////////////////\n";
	std::cout << "関数に値渡しを指定した場合\n";
	AddDataVector_Val(vec, 5);
	printVector(vec);

	_getch(); // ユーザーがキーを押すまで待機
	std::cout << "\n///////////////////////////////////////////\n";
	std::cout << "関数に参照渡しを指定した場合\n";
	AddDataVector_Addr(vec, 5);
	printVector(vec);

}


// 引数にconst std::vector<int>&を指定することで、ベクターの参照を渡すことができる
// &は参照を示す ポインタと参照の回で詳しく説明する
void printVector(const std::vector<int>& vec)
{
	std::cout << "Vector contents: ";
	// このfor分は範囲for文と言う
	// これはC++11以降の機能で、C++の標準ライブラリに追加されたもの
	// vecの要素を1つずつ取り出してvに代入する
	// 取り出した要素はconst int&で参照渡しされる 値渡しも可能 
	for (const int& v : vec)
	{
		// 参照渡しされた要素を出力する 
		std::cout << v << " ";
	}

	std::cout << std::endl; //std::endlは改行を出力する。処理系に左右されない。"\n"はwindows系の改行
}

void AddDataVector_Addr(std::vector<int>& vec, int _num)
{
	int _init = vec.size(); // ベクターのサイズを取得
	for (int i = 0; i < _num ; ++i)
	{
		vec.push_back(i + _init);
	}
	return;
}

void AddDataVector_Val(std::vector<int> vec, int _num)
{
	int _init = vec.size(); // ベクターのサイズを取得
	for (int i = 0; i < _num; ++i)
	{
		vec.push_back(i + _init);
	}
	return;
}

