#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

// ���̓v���O����
char *user_input;
// ���݂�token
Token *token;

// �G���[��񍐂��邽�߂̊֐�
// printf�Ɠ����������Ƃ�
void error(char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// ���̓v���O����
char *user_input;
// �G���[�̉ӏ���񍐂���
void error_at(char *loc, char *format, ...) {
    va_list ap;
    va_start(ap, format);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos�̋󔒂��o��
    fprintf(stderr, "^");
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// ���̃g�[�N�������҂��Ă���i�����ŗ^�����j�L���̎��ɂ́A
// �g�[�N����1�ǂݐi�߂Đ^��Ԃ��B����ȊO�̏ꍇ�͋U��Ԃ��B
bool consume(char *op) {
    if (token->kind != TK_RESERVED 
        || strlen(op) != token->len 
        || memcmp(token->str, op, token->len)) {
        return false;
    }
    token = token->next;
    return true;
}

// ���̃g�[�N�������҂��Ă���i�����ŗ^�����j�L���̎��ɂ́A
// �g�[�N����1�ǂݐi�߂�B����ȊO�̏ꍇ�̓G���[��񍐂���B
void expect(char *op) {
    if (token->kind != TK_RESERVED
        || strlen(op) != token->len
        || memcmp(token->str, op, token->len)) {
        error_at(token->str, "expected '%c", op);
    }
    token = token->next;
}

// ���̃g�[�N�������l�̏ꍇ�A�g�[�N����1�ǂݐi�߂Ă��̐��l��Ԃ��B
// ����ȊO�̏ꍇ�̓G���[��Ԃ��B
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

// �V�����g�[�N�����쐬���āAcur�Ɍq����
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

// ���͕�����p���g�[�N�i�C�Y���Ă����Ԃ�
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // �󔒕������X�L�b�v
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
        fprintf(stderr, "�����̌�������������܂���\n");
        return 1;
    }

    // �g�[�N�i�C�Y����
    user_input = argv[1];
    token = tokenize(user_input);
    Node *node = expr();

    // �A�Z���u���̑O���������o��
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // ���ۍ\���؂�����Ȃ���R�[�h����
    gen(node);

    // �X�^�b�N�g�b�v�Ɏ��S�̂̒l���c���Ă���͂��Ȃ̂�
    // �����RAX�Ƀ��[�h���Ă���֐�����̕Ԓl�Ƃ���
    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}
