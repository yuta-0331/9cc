#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
    TK_RESERVED, // �L��
    TK_NUM,      // �����g�[�N��
    TK_EOF,      // ���͂̏I���������g�[�N��
} Token_kind;

typedef struct Token Token;

struct Token {
    Token_kind kind; // �g�[�N���̌^
    Token *next;     // ���̓��̓g�[�N��
    int val;         // kind��TK_NUM�̏ꍇ�A���̐��l
    char *str;       // �g�[�N��������
};

Token *token;

typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // ����
} NodeKind;

typedef struct Node Node;

// ���ۍ\���؂̃m�[�h�^
struct Node {
    NodeKind kind; // �m�[�h�̌^
    Node *lhs;     // ����
    Node *rhs;     // �E��
    int val;       // ND_NUM�̏ꍇ�A���̒l
};


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
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        return false;
    }
    token = token->next;
    return true;
}

// ���̃g�[�N�������҂��Ă���i�����ŗ^�����j�L���̎��ɂ́A
// �g�[�N����1�ǂݐi�߂�B����ȊO�̏ꍇ�̓G���[��񍐂���B
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
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
Token *new_token(Token_kind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
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

        if (strchr("+-*/()", *p)) {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "expected a number");

    }
    new_token(TK_EOF, cur, p);
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

Node *mul(void);
Node *primary(void);
Node *expr(void);

// expr = mul ("+" mul | "-" mul)*
Node *expr(void) {
    Node *node = mul();

    for (;;) {
        if (consume('+')) {
            node = new_node(ND_ADD, node, mul());
        } else if (consume('-')) {
            node = new_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

// mul = primary ("*" primary | "/" primary)*
Node *mul(void) {
    Node *node = primary();

    for (;;) {
        if (consume('*')) {
            node = new_node(ND_MUL, node, primary());
        } else if (consume('/')) {
            node = new_node(ND_DIV, node, primary());
        } else {
            return node;
        }
    }
}

// primary = "(" expr ")" | num
Node *primary(void) {
    // ���̃g�[�N����"("�Ȃ�"(" expr ")"�̂͂�
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }
    // �����łȂ���ΐ��l�̂͂�
    return new_node_num(expect_number());
}

void gen(Node *node) {
    if (node->kind == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;

        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rax, rdi\n");
            break;
    }
    printf("    push rax\n");
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