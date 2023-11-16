#ifndef H_9CC_INCLUDED
    #define H_9CC_INCLUDED

    #include <stdbool.h>
    /********************
    * token: 入力されたプログラムを
    *      　意味の通る最小単位に分割したもの(tokenize)
    *        例: "120 + 3" -> 120, +, 3の三つのtokenに分割
    * node:  構文木を構成する要素
    *        tokenを構文解析して生成する
    *        例: 
    *            +
    *           / \
    *         120  3 この要素一つ一つがnode
    *********************/

    /********************
    * tokenの種類を表す列挙型
    *********************/
    typedef enum {
        TK_RESERVED, // 記号
	TK_IDENT,    // 識別子
	TK_NUM,      // 整数トークン
        TK_EOF,      // 入力の終わりを示すトークン
    } Token_kind;

    /********************
    * tokenの構造体定義
    *********************/
    typedef struct _Token {
        Token_kind kind;     // トークンの型
        struct _Token *next; // 次の入力トークン
        int val;             // kindがTK_NUMの場合、その数値
        char *str;           // トークン文字列
        int len;             // トークンの長さ
    } Token;

    /********************
    * nodeの構造体定義
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
	ND_LVAR, // ローカル変数
        ND_NUM,  // 整数
    } NodeKind;

    /********************
    * 抽象構文木のノード型
    *********************/ 
    typedef struct _Node {
        NodeKind kind;     // ノードの型
        struct _Node *lhs; // 左辺
        struct _Node *rhs; // 右辺
        int val;           // ND_NUMの場合、その値
	int offset;	   // kindがND_LVARの場合のみ使う
    } Node;

    /********************
    * エラーを表示する。
    * printfと同じ引数を取り、vfprintfに渡す。
    * 引数: 
    *   format, ...: printfと同じ引数を取る。
    *********************/
    void error(char *format, ...);
    
    /********************
    * エラーの箇所を表示する
    * 引数: 
    *   loc: エラーの箇所
    *       user_inputに含まれる文字列のうち、
    *       tokenize中にエラーを吐いた文字のアドレスを渡す。
    *   format, ...: printfと同じ引数を取る。
    *********************/
    void error_at(char *loc, char *format, ...);
    
    /********************
    * consume: 消費する
    * トークンが期待している（引数で与えた）記号の時には、
    * トークンを1つ読み進めて真を返す。それ以外の場合は偽を返す。
    * 引数: 
    *   op: operatorの略。消費する記号を渡す。
    * 戻り値:
    *   (1)期待した値だった場合, (0)それ以外
    *********************/
    bool consume(char *op);

    /********************
    * consume: 消費する
    * トークンがローカル変数(a ~ zの文字)の時には、
    * トークンを1つ読み進めてローカル変数tokenへのポインタ返す。
    * 戻り値:
    *   ローカル変数だった場合,そのtokenへのポインタを返す。 
    *   それ以外の場合、NULL
    *********************/
    Token *consume_ident(void);

    /********************
    * expect: 期待する
    * トークンが期待している（引数で与えた）記号の時には、
    * トークンを1つ読み進める。それ以外の場合は偽を返す。
    * 引数: 
    *   op: operatorの略。消費する記号を渡す。
    *********************/
    void expect(char *op);

    /********************
    * expect: 期待する
    * トークンが数値の場合には、
    * トークンを1つ読み進めてその数値を返す。それ以外の場合はエラーを報告する。
    * 戻り値:
    *   token->val
    *********************/
    int expect_number(void);

    /********************
    * 次のトークンが末尾にいるかどうかを判定
    * 戻り値:
    *   (0)末尾にいない, (1)末尾にいる
    *********************/
    bool at_eof(void);

    /********************
    * 新しいtokenを生成して、curに繋げ、tokenを返す
    * 引数:
    *   kind: tokenの種類
    *   cur: 現在のカーソルが指しているtoken
    *   str: token文字列
    *   len: token文字列の長さ
    * 戻り値:
    *   生成したtokenのポインタを返す
    *********************/
    Token *new_token(Token_kind kind, Token *cur, char *str, int len);

    /********************
    * 検証する文字列pがqで始まっているかをチェックする
    * 引数: 
    *   p: 検証する文字列
    *   q: 検証文字列
    * 戻り値:
    *   (1)qで始まっている, (0)qで始まっていない
    *********************/
    bool start_with(char *p, char *q);

    /********************
    * p(user_input)をtokenに分割して
    * 先頭データ（データヘッドの次のtoken）を返す
    * 引数:
    *   p: tokenizeする文字列（user_input） 
    * 戻り値:
    *   データヘッド(空データ)の次のtokenを返す
    *********************/
    Token *tokenize(char *p);

    /********************
    * nodeを作成して、そのポインタを返す
    * 引数:
    *   kind: nodeの種類 
    *   lhs: 左下に繋げるnode
    *   rhs: 右下に繋げるnode
    * 戻り値:
    *   作成したnodeのポインタ
    *********************/
    Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

    /********************
    * 数値nodeを作成して、そのポインタを返す
    * 引数:
    *   val: nodeの数値
    * 戻り値:
    *   作成したnodeのポインタ
    *********************/
    Node *new_node_num(int val);

    /********************
    * 構文解析は以下の通りになる
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
    * generator: 発生器
    * アセンブリの出力関数
    * 引数: 
    *   node: 
    *********************/
    void gen(Node *node);

    /********************
    * generator: 発生器
    * 左辺値のアセンブリを出力関数
    * 引数: 
    *   node: 
    *********************/
    void gen_lval(Node *node);

    // 入力プログラム
    extern char *user_input;
    // 複数のnodeを格納する
    extern Node *code[100];
    // 現在のtoken
    extern Token *token;

#endif /* H_9CC_INCLUDED */
