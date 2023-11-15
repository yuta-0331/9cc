#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// 次のトークンが期待している（引数で与えた）記号の時には、
// トークンを1つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume(char *op) {
    if (token->kind != TK_RESERVED 
        || strlen(op) != token->len 
        || memcmp(token->str, op, token->len)) {
        return false;
    }
    token = token->next;
    return true;
}

// 次のトークンが期待している（引数で与えた）記号の時には、
// トークンを1つ読み進める。それ以外の場合はエラーを報告する。
void expect(char *op) {
    if (token->kind != TK_RESERVED
        || strlen(op) != token->len
        || memcmp(token->str, op, token->len)) {
        error_at(token->str, "expected '%c", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合はエラーを返す。
int expect_number(void) {
    if (token->kind != TK_NUM) {
        error_at(token->str, "expected a number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(void) {
    return token->kind == TK_EOF;
}

// 新しいトークンを作成して、curに繋げる
Token *new_token(Token_kind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool start_with(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (start_with(p, "==") 
            || start_with(p, "!=")
            || start_with(p, "<=")
            || start_with(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            // char *q = p;
            cur->val = strtol(p, &p, 10);
            // cur->len = p - q;
            continue;
        }

        error_at(p, "expected a number");

    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}
 
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // トークナイズする
    user_input = argv[1];
    token = tokenize(user_input);
    Node *node = expr();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // 抽象構文木を下りながらコード生成
    gen(node);

    // スタックトップに式全体の値が残っているはずなので
    // それをRAXにロードしてから関数からの返値とする
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}
