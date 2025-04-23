//////////////////////////////////////////////////////////
// アセンブラサンプル
// x86モードでビルドすること

#include <iostream>
#include <chrono>


#define SWAP_ASM1(PARAM_1, PARAM_2) \
    __asm { \
        mov eax, PARAM_1  \
        mov ebx, PARAM_2  \
        xchg eax, ebx  \
        mov PARAM_1, eax  \
        mov PARAM_2, ebx  \
    }

#define SWAP_ASM2(aaa, bbb)                                \
    __asm {                                           \
        lea  eax, aaa           /* EAX = &a */       \
        lea  ecx, bbb           /* ECX = &b */       \
        mov  edx, DWORD PTR [eax]  /* EDX = a の値 */ \
        mov  ebx, DWORD PTR [ecx]  /* EBX = b の値 */ \
        mov  DWORD PTR [eax], ebx  /* a = b */       \
        mov  DWORD PTR [ecx], edx  /* b = a */       \
    }

#define SWAP_DATA(aaa, bbb) \
    { \
	    int ccc = aaa;  \
	    aaa = bbb;      \
	    bbb = ccc;      \
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
            int a = e[i];
            int b = f[i];
            SWAP_DATA(a, b);
            e[i]=a;
            f[i]=b;
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

		int a, b;
        int* pa = &a;
        int* pb = &b;

        for (int i = 0; i < N; ++i) {
			a = e[i];
			b = f[i];
            __asm {
                mov eax, pa
                mov ebx, pb
                xchg eax, ebx
                mov pa, eax
                mov pb, ebx
            }
            e[i] = a;
            f[i] = b;
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
            int a = e[i];
            int b = f[i];
          SWAP_DATA(a, b);
          e[i] = a;
          f[i] = b;

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

        int a, b;
        int* pa = &a;
        int* pb = &b;
        for (int i = 0; i < N; ++i) {
            a = e[i];
            b = f[i];
            __asm {
                mov eax, pa
                mov ebx, pb
                xchg eax, ebx
                mov pa, eax
                mov pb, ebx
            }
            e[i] = a;
            f[i] = b;
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
