
#include "Lexer.h"

typedef enum {
    Err_ok,
    Err_stackUnderflow,
    Err_stackOverflow,
    Err_MismatchedParentheses,
    Err_unknownToken,
    Err_stackNotEmpty,
} Err;

Err shunting_yard(Token infixTokens[MAX_TOKENS], Token posfixTokens[MAX_TOKENS]);
int precedence(Token* token);
bool is_operator(Token* token);
bool is_left_associative(Token* token);
double eval_posfix(Token posfixTokens[MAX_TOKENS], Err* error_status);
double operation(double left, double right, TokenType type);

// AST
typedef struct AST_t AST;
AST* AST_grow(Token infixTokens[MAX_TOKENS]);
void AST_destroy(AST* ast);

// user eval function
double eval(char* input);



