//////////////////////////////////////////////////////////
// アセンブラサンプル
// x86モードでビルドすること

#include <iostream>
#include <chrono>


#define SWAP_VARS(PARAM_1, PARAM_2) \
    __asm { mov eax, PARAM_1 } \
    __asm { mov ebx, PARAM_2 } \
    __asm { xchg eax, ebx } \
    __asm { mov PARAM_1, eax } \
    __asm { mov PARAM_2, ebx }

#define SWAP(aaa, bbb)                                \
    __asm {                                           \
        lea  eax, aaa           /* EAX = &a */       \
        lea  ecx, bbb           /* ECX = &b */       \
        mov  edx, DWORD PTR [eax]  /* EDX = a の値 */ \
        mov  ebx, DWORD PTR [ecx]  /* EBX = b の値 */ \
        mov  DWORD PTR [eax], ebx  /* a = b */       \
        mov  DWORD PTR [ecx], edx  /* b = a */       \
    }

//#define SWAP_VARS(PARAM_1, PARAM_2) \
//    __asm { lea esi, PARAM_1 }         /* アドレスを esi に */ \
//    __asm { lea edi, PARAM_2 }         /* アドレスを edi に */ \
//    __asm { mov eax, [esi] }     \
//    __asm { mov ebx, [edi] }     \
//    __asm { xchg eax, ebx }      \
//    __asm { mov [esi], eax }     \
//    __asm { mov [edi], ebx }


// 32ビット x86, MSVC 用
inline void SwapAsm(int& a, int& b)
{
    __asm {
        // 引数 a（実体は int*）を EAX にロード
        mov  eax, a
        // 引数 b（実体は int*）を ECX にロード
        mov  ecx, b
        // [EAX]（= a の値）を EDX にロード
        mov  edx, [eax]
        // EDX（= a の値）と [ECX]（= b の値）を交換
        xchg edx, [ecx]
        // EAX が指す場所に、交換後の EDX（= 元の b の値）を書き戻し
        mov[eax], edx
    }
}

inline void SwapAsm2(int& PARAM_1, int& PARAM_2)
{
    __asm { mov eax, PARAM_1 }
    __asm { mov ebx, PARAM_2 }
    __asm { xchg eax, ebx }
    __asm { mov PARAM_1, eax }
    __asm { mov PARAM_2, ebx }
}

inline void SwapData(int& a, int& b)
{
	int c = a;
	b = a;
	a = c;
}

///////////////////////////////////////////////////////////////////////////
// clflush命令を使用してキャッシュラインを無効する関数
void flush_cache_line(void* pt) 
{
    __asm {
        mov eax, pt
        clflush[eax]
    }
}
///////////////////////////////////////////////////////////////////////////
//エントリーポイント
///////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <chrono>

// flush_cache_line／SWAP_VARS は適宜定義済みとします。

int main()
{
    const int N = 1000000;  // 1パスあたりのスワップ回数
    const int M = 100;      // 繰り返し計測回数

    int* e = new int[N];
    int* f = new int[N];
    long long sum_enabled_alg = 0, sum_enabled_asm = 0;
    long long sum_disabled_alg = 0, sum_disabled_asm = 0;

    // === キャッシュ有効（flushなし） ===

    // 1) 代数スワップ
    for (int m = 0; m < M; ++m) {
        // キャッシュをそのまま有効利用
        for (int i = 0; i < N; ++i) {
            e[i] = i;
            f[i] = N - i;
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            //int buf = e[i];
            //e[i] = f[i];
            //f[i] = buf;
			SwapData(e[i], f[i]);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        sum_enabled_alg += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }

    // 2) アセンブラスワップ
    for (int m = 0; m < M; ++m) {
        // キャッシュをそのまま有効利用
        for (int i = 0; i < N; ++i) {
            e[i] = i;
            f[i] = N - i;
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
			int a = e[i];
			int b = f[i];
            SWAP_VARS(a, b);
            //SWAP_VARS(e[i], f[i]);
            //SWAP(e[i], f[i]);
            //SwapAsm2(e[i], f[i]);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        sum_enabled_asm += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }

    // === キャッシュ無効（毎回flush） ===

    // 3) 代数スワップ
    for (int m = 0; m < M; ++m) {
        flush_cache_line(e);
        flush_cache_line(f);
        for (int i = 0; i < N; ++i) {
            e[i] = i;
            f[i] = N - i;
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            //int buf = e[i];
            //e[i] = f[i];
            //f[i] = buf;
			SwapData(e[i], f[i]);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        sum_disabled_alg += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }

    // 4) アセンブラスワップ
    for (int m = 0; m < M; ++m) {
        flush_cache_line(e);
        flush_cache_line(f);
        for (int i = 0; i < N; ++i) {
            e[i] = i;
            f[i] = N - i;
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            int a = e[i];
            int b = f[i];
            SWAP_VARS(a, b); //直接突っ込むとマクロで文法エラー
            //SWAP_VARS(e[i], f[i]);
            //SwapAsm2(e[i], f[i]);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        sum_disabled_asm += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    }

    // 平均を計算
    double avg_en_alg = static_cast<double>(sum_enabled_alg) / M;
    double avg_en_asm = static_cast<double>(sum_enabled_asm) / M;
    double avg_dis_alg = static_cast<double>(sum_disabled_alg) / M;
    double avg_dis_asm = static_cast<double>(sum_disabled_asm) / M;

    // 結果表示
    std::cout << "N = " << N << " 回スワップ, M = " << M << " 回の平均時間（micro ssec）\n\n";

    std::cout << "キャッシュ有効:\n";
    std::cout << "  代数処理     : " << avg_en_alg << " micro sec\n";
    std::cout << "  アセンブラ処理: " << avg_en_asm << " micro sec\n\n";

    std::cout << "キャッシュ無効:\n";
    std::cout << "  代数処理     : " << avg_dis_alg << " micro sec\n";
    std::cout << "  アセンブラ処理: " << avg_dis_asm << " micro sec\n\n";

    std::cout << "1要素あたり（ns）:\n";
    std::cout << "キャッシュ有効:\n";
    std::cout << "  代数処理     : " << (avg_en_alg / N) * 1000 << " ns\n";
    std::cout << "  アセンブラ処理: " << (avg_en_asm / N) * 1000 << " ns\n\n";

    std::cout << "キャッシュ無効:\n";
    std::cout << "  代数処理     : " << (avg_dis_alg / N) * 1000 << " ns\n";
    std::cout << "  アセンブラ処理: " << (avg_dis_asm / N) * 1000 << " ns\n";

    delete[] e;
    delete[] f;
    return 0;
}
