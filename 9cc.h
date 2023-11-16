#ifndef H_9CC_INCLUDED
    #define H_9CC_INCLUDED

    #include <stdbool.h>
    /********************
    * token: ���͂��ꂽ�v���O������
    *      �@�Ӗ��̒ʂ�ŏ��P�ʂɕ�����������(tokenize)
    *        ��: "120 + 3" -> 120, +, 3�̎O��token�ɕ���
    * node:  �\���؂��\������v�f
    *        token���\����͂��Đ�������
    *        ��: 
    *            +
    *           / \
    *         120  3 ���̗v�f����node
    *********************/

    /********************
    * token�̎�ނ�\���񋓌^
    *********************/
    typedef enum {
        TK_RESERVED, // �L��
	TK_IDENT,    // ���ʎq
	TK_NUM,      // �����g�[�N��
        TK_EOF,      // ���͂̏I���������g�[�N��
    } Token_kind;

    /********************
    * token�̍\���̒�`
    *********************/
    typedef struct _Token {
        Token_kind kind;     // �g�[�N���̌^
        struct _Token *next; // ���̓��̓g�[�N��
        int val;             // kind��TK_NUM�̏ꍇ�A���̐��l
        char *str;           // �g�[�N��������
        int len;             // �g�[�N���̒���
    } Token;

    /********************
    * node�̍\���̒�`
    *********************/
    typedef enum {
        ND_ADD,  // +
        ND_SUB,  // -
        ND_MUL,  // *
        ND_DIV,  // /
        ND_EQ,   // ==
        ND_NE,   // !=
        ND_LT,   // <
        ND_LE,   // <=
	ND_LVAR, // ���[�J���ϐ�
        ND_NUM,  // ����
    } NodeKind;

    /********************
    * ���ۍ\���؂̃m�[�h�^
    *********************/ 
    typedef struct _Node {
        NodeKind kind;     // �m�[�h�̌^
        struct _Node *lhs; // ����
        struct _Node *rhs; // �E��
        int val;           // ND_NUM�̏ꍇ�A���̒l
	int offset;	   // kind��ND_LVAR�̏ꍇ�̂ݎg��
    } Node;

    /********************
    * �G���[��\������B
    * printf�Ɠ������������Avfprintf�ɓn���B
    * ����: 
    *   format, ...: printf�Ɠ������������B
    *********************/
    void error(char *format, ...);
    
    /********************
    * �G���[�̉ӏ���\������
    * ����: 
    *   loc: �G���[�̉ӏ�
    *       user_input�Ɋ܂܂�镶����̂����A
    *       tokenize���ɃG���[��f���������̃A�h���X��n���B
    *   format, ...: printf�Ɠ������������B
    *********************/
    void error_at(char *loc, char *format, ...);
    
    /********************
    * consume: �����
    * �g�[�N�������҂��Ă���i�����ŗ^�����j�L���̎��ɂ́A
    * �g�[�N����1�ǂݐi�߂Đ^��Ԃ��B����ȊO�̏ꍇ�͋U��Ԃ��B
    * ����: 
    *   op: operator�̗��B�����L����n���B
    * �߂�l:
    *   (1)���҂����l�������ꍇ, (0)����ȊO
    *********************/
    bool consume(char *op);

    /********************
    * consume: �����
    * �g�[�N�������[�J���ϐ�(a ~ z�̕���)�̎��ɂ́A
    * �g�[�N����1�ǂݐi�߂ă��[�J���ϐ�token�ւ̃|�C���^�Ԃ��B
    * �߂�l:
    *   ���[�J���ϐ��������ꍇ,����token�ւ̃|�C���^��Ԃ��B 
    *   ����ȊO�̏ꍇ�ANULL
    *********************/
    Token *consume_ident(void);

    /********************
    * expect: ���҂���
    * �g�[�N�������҂��Ă���i�����ŗ^�����j�L���̎��ɂ́A
    * �g�[�N����1�ǂݐi�߂�B����ȊO�̏ꍇ�͋U��Ԃ��B
    * ����: 
    *   op: operator�̗��B�����L����n���B
    *********************/
    void expect(char *op);

    /********************
    * expect: ���҂���
    * �g�[�N�������l�̏ꍇ�ɂ́A
    * �g�[�N����1�ǂݐi�߂Ă��̐��l��Ԃ��B����ȊO�̏ꍇ�̓G���[��񍐂���B
    * �߂�l:
    *   token->val
    *********************/
    int expect_number(void);

    /********************
    * ���̃g�[�N���������ɂ��邩�ǂ����𔻒�
    * �߂�l:
    *   (0)�����ɂ��Ȃ�, (1)�����ɂ���
    *********************/
    bool at_eof(void);

    /********************
    * �V����token�𐶐����āAcur�Ɍq���Atoken��Ԃ�
    * ����:
    *   kind: token�̎��
    *   cur: ���݂̃J�[�\�����w���Ă���token
    *   str: token������
    *   len: token������̒���
    * �߂�l:
    *   ��������token�̃|�C���^��Ԃ�
    *********************/
    Token *new_token(Token_kind kind, Token *cur, char *str, int len);

    /********************
    * ���؂��镶����p��q�Ŏn�܂��Ă��邩���`�F�b�N����
    * ����: 
    *   p: ���؂��镶����
    *   q: ���ؕ�����
    * �߂�l:
    *   (1)q�Ŏn�܂��Ă���, (0)q�Ŏn�܂��Ă��Ȃ�
    *********************/
    bool start_with(char *p, char *q);

    /********************
    * p(user_input)��token�ɕ�������
    * �擪�f�[�^�i�f�[�^�w�b�h�̎���token�j��Ԃ�
    * ����:
    *   p: tokenize���镶����iuser_input�j 
    * �߂�l:
    *   �f�[�^�w�b�h(��f�[�^)�̎���token��Ԃ�
    *********************/
    Token *tokenize(char *p);

    /********************
    * node���쐬���āA���̃|�C���^��Ԃ�
    * ����:
    *   kind: node�̎�� 
    *   lhs: �����Ɍq����node
    *   rhs: �E���Ɍq����node
    * �߂�l:
    *   �쐬����node�̃|�C���^
    *********************/
    Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

    /********************
    * ���lnode���쐬���āA���̃|�C���^��Ԃ�
    * ����:
    *   val: node�̐��l
    * �߂�l:
    *   �쐬����node�̃|�C���^
    *********************/
    Node *new_node_num(int val);

    /********************
    * �\����͈͂ȉ��̒ʂ�ɂȂ�
    * program = stmt*
    * stmt = expr ";"
    * expr = assign
    * assign = equarity ("=" assign)? 
    * equarity = relational ("==" relational | "!=" relational)*
    * relational = add ("<" add | "<=" add | ">" add | ">=" add)*
    * add = mul ("+" mul | "-" mul)*
    * mul = unary ("+" unary | "-" unary)*
    * unary = ("+" | "-")? primary
    * primary = num | ident | "(" expr ")"
    *********************/
    void program(void);
    Node *stmt(void);
    Node *expr(void);
    Node *assign(void);
    Node *equality(void);
    Node *relational(void);
    Node *add(void);
    Node *mul(void);
    Node *unary(void);
    Node *primary(void);

    /********************
    * generator: ������
    * �A�Z���u���̏o�͊֐�
    * ����: 
    *   node: 
    *********************/
    void gen(Node *node);

    /********************
    * generator: ������
    * ���Ӓl�̃A�Z���u�����o�͊֐�
    * ����: 
    *   node: 
    *********************/
    void gen_lval(Node *node);

    // ���̓v���O����
    extern char *user_input;
    // ������node���i�[����
    extern Node *code[100];
    // ���݂�token
    extern Token *token;

#endif /* H_9CC_INCLUDED */
