
#include "Lexer.h"

typedef enum {
    Err_ok,
    Err_stackUnderflow,
    Err_stackOverflow,
    Err_MismatchedParentheses,
    Err_unknownToken,
} Err;

Err shunting_yard(Token infixTokens[MAX_TOKENS], Token posfixTokens[MAX_TOKENS]);
int precedence(Token* token);
bool is_operator(Token* token);
bool is_left_associative(Token* token);
double eval_posfix(Token posfixTokens[MAX_TOKENS]);
double operation(double left, double right, TokenType type);

double eval(char* input);



