#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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
