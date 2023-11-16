#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

// 入力プログラム
char *user_input;
// 現在のtoken
Token *token;

// エラーを報告するための関数
// printfと同じ引数をとる
void error(char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 入力プログラム
char *user_input;
// エラーの箇所を報告する
void error_at(char *loc, char *format, ...) {
    va_list ap;
    va_start(ap, format);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
    fprintf(stderr, "^");
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    exit(1);
}
 
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // トークナイズする
    user_input = argv[1];
    token = tokenize(user_input);
    program();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // プロローグ
    // 変数26個分の領域を確保
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");

    // 先頭の式から順にコード生成
    for (int i = 0; code[i]; i++) {
        gen(code[i]);
    }

    // スタックトップに式全体の値が残っているはずなので
    // スタックが溢れないようにpopしておく
    printf("    pop rax\n");

    // エピローグ
    // 最後の式の結果がraxに残っているので、それが返値になる
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}
