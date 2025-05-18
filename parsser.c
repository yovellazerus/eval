
#include "parsser.h"

int precedence(Token* token){
    switch (token->type){
        case Token_CURET:
            return 3;
        case Token_DIV:
        case Token_MUL:
        case Token_MOD:
            return 2;
        case Token_PLUS:
        case Token_MINOS:
            return 1;
        default:
            return 0;
    }   
}

Err shunting_yard(Token infixTokens[MAX_TOKENS], Token posfixTokens[MAX_TOKENS]){
    Token stack[MAX_TOKENS];
    int sp = 0;
    int posfix_index = 0;
    int infix_index = 0;
    
    while(infixTokens[infix_index].type != Token_EOF){
        Token currentToken = infixTokens[infix_index];
        switch (currentToken.type) {
            case Token_NUMBER:
                posfixTokens[posfix_index] = currentToken;
                posfix_index++;
                break;
            case Token_NAME:
                // anlly vars for now no functions!
                posfixTokens[posfix_index] = currentToken;
                posfix_index++;
                break;
            case Token_ERROR:
                return Err_unknownToken;
                break;
                
            case Token_CURET:
            case Token_DIV:
            case Token_MUL:
            case Token_MOD:
            case Token_PLUS:
            case Token_MINOS:
                while(sp > 0 &&
                      is_operator(&stack[sp - 1]) &&
                      (
                            (is_left_associative(&currentToken) && precedence(&stack[sp - 1]) >= precedence(&currentToken)) ||
                            (!is_left_associative(&currentToken) && precedence(&stack[sp - 1]) > precedence(&currentToken))
                      )        ){
                          
                   posfixTokens[posfix_index] = stack[sp - 1];
                   sp--;
                   posfix_index++;
                }
                
                if(sp >= MAX_TOKENS) {
                    return Err_stackOverflow;
                }
                
                stack[sp] = currentToken;
                sp++;
                break;
            
            case Token_OPEN:
                if(sp >= MAX_TOKENS) {
                    return Err_stackOverflow;
                }
                stack[sp] = currentToken;
                sp++;
                break;
            case Token_CLOSE:
                while(sp > 0 && stack[sp - 1].type != Token_OPEN){
                    posfixTokens[posfix_index] = stack[sp - 1];
                    sp--;
                    posfix_index++;
                }
                // if loop ended with no stack anderflow
                if(sp > 0){
                    sp--;
                }
                // stack anderflow!
                else{
                    return Err_MismatchedParentheses;
                }
                break;
                
            default:
                return Err_unknownToken;
        }
        
        infix_index++;
    }
    
    while(sp > 0){
        if (stack[sp - 1].type == Token_OPEN || stack[sp - 1].type == Token_CLOSE){
            return Err_MismatchedParentheses;
        }
        posfixTokens[posfix_index] = stack[sp - 1];
        sp--;
        posfix_index++;
    }
    // null termited
    posfixTokens[posfix_index].type = Token_EOF;
    return Err_ok;
}

bool is_operator(Token* token){
    switch (token->type){
        case Token_EOF:
            return false;
        case Token_ERROR:
            return false;
        case Token_NUMBER:
            return false;
        case Token_NAME:
            return false;
        case Token_PLUS:
        case Token_MINOS:
        case Token_CURET:
        case Token_DIV:
        case Token_MUL:
        case Token_MOD:
            return true;
        case Token_OPEN:
            return false;
        case Token_CLOSE:
            return false;
        default:
            return false;
    }
}

bool is_left_associative(Token* token){
    switch (token->type){
        case Token_CURET:
            return false;
        case Token_PLUS:
        case Token_MINOS:
        case Token_MUL:
        case Token_DIV:
        case Token_MOD:
            return true;
        default:
            return true;
    }
}

// TODO: "error handling in a real way..."
double eval_posfix(Token posfixTokens[MAX_TOKENS]){
    double stack[MAX_TOKENS];
    int sp = 0;
    
    int input_index = 0;
    while(posfixTokens[input_index].type != Token_EOF){
        Token curr = posfixTokens[input_index];
        switch (curr.type) {
            case Token_NUMBER:
            case Token_NAME: // no func for now...
                if(sp >= MAX_TOKENS){
                    assert(false && "Stack overflow");
                }
                stack[sp] = curr.value;
                sp++;
                break;
            
            case Token_CURET:
            case Token_DIV:
            case Token_MUL:
            case Token_MOD:
            case Token_PLUS:
            case Token_MINOS:
            
                if (sp < 2) {
                    assert(false && "Stack underflow during evaluation");
                }   
                
                double right = stack[sp - 1];
                sp--;
                double left = stack[sp - 1];
                sp--;
                stack[sp] = operation(left, right, curr.type);
                sp++;
                break;
            
            default:
                assert(false && "unreachable");
                break;
        }
        
        input_index++;
    }
    if(sp != 1){
        assert(false && "stack not empty after eval");
    }
    return stack[0];
}

double operation(double left, double right, TokenType type) {
    switch (type) {
        case Token_PLUS:  return left + right;
        case Token_MINOS: return left - right;
        case Token_MUL:   return left * right;
        case Token_DIV:   return left / right;
        case Token_MOD:   return fmod(left, right);
        case Token_CURET: return pow(left, right);
        default:
            assert(false && "unreachable");
            break;
    }
}

double eval(char* input){
    Token infixTokens[MAX_TOKENS];
    Token posfixTokens[MAX_TOKENS];
    Lexer lexer = Lexer_init(input);
    double res = 0.0;
    
    int numTokens = lexer_lexAllContent(&lexer, infixTokens);
    Err err = shunting_yard(infixTokens, posfixTokens);
    switch (err) {
        case Err_ok:
            res = eval_posfix(posfixTokens);
            break;
        case Err_stackUnderflow:
            assert(false && "ERROR: Err_stackUnderflow error in eval function");
            break;
        case Err_MismatchedParentheses:
            assert(false && "ERROR: Err_MismatchedParentheses error in eval function");
            break;
        case Err_unknownToken:
            assert(false && "ERROR: Err_unknownToken error in eval function");
            break;
        default:
            assert(false && "ERROR: unknon error in eval function");
            break;
    }
    
    return res;
}