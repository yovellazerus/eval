
#include "parsser.h"

// custom math functions:
static double factorial(double arg){
    if(arg < 0.0 || ceil(arg) != floor(arg)){
        return nan("");
    }
    if( arg == 0.0){
        return 1.0;
    }
    return factorial(arg - 1.0) * arg;
}

int precedence(Token* token){
    switch (token->type){
        case Token_CURET:
        case Token_EXCLAMATION:
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
                // only vars for now no functions!
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
            case Token_EXCLAMATION:
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
                
                if(sp > 0){
                    sp--;
                }
                
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
        case Token_EXCLAMATION:
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
        case Token_EXCLAMATION:
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

double eval_posfix(Token posfixTokens[MAX_TOKENS], Err* error_status){
    double stack[MAX_TOKENS];
    int sp = 0;
    
    int input_index = 0;
    while(posfixTokens[input_index].type != Token_EOF){
        Token curr = posfixTokens[input_index];
        switch (curr.type) {
            case Token_NUMBER:
            case Token_NAME: // no func for now...
                if(sp >= MAX_TOKENS){
                    *error_status = Err_stackOverflow;
                    return 0;
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
                    *error_status = Err_stackUnderflow;
                    return 0;
                }   
                
                double right = stack[sp - 1];
                sp--;
                double left = stack[sp - 1];
                sp--;
                stack[sp] = operation(left, right, curr.type);
                sp++;
                break;
            
            // unary ops:
            case Token_EXCLAMATION:
                if (sp < 1) {
                    *error_status = Err_stackUnderflow;
                    return 0;
                }  
            
                double operand = stack[sp - 1];
                sp--;
                stack[sp] = operation(operand, 0.0, curr.type);
                sp++;
                break;
            
            default:
                *error_status = Err_unknownToken;
                return 0;
                break;
        }
        
        input_index++;
    }
    if(sp != 1){
        *error_status = Err_stackNotEmpty;
        return 0;
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
        case Token_EXCLAMATION: return factorial(left);
        default:
            fprintf(stderr, "ERROR: unknown operation %S in Line: %d file: %s",type_to_str_table[type], __LINE__, __FILE__);
            exit(1);
            break;
    }
}

double eval(char* input){
    Token infixTokens[MAX_TOKENS];
    Token posfixTokens[MAX_TOKENS];
    Lexer lexer = Lexer_init(input);
    double res = 0.0;
    
    int numTokens = lexer_lexAllContent(&lexer, infixTokens);
    Err error_status = shunting_yard(infixTokens, posfixTokens);
    res = eval_posfix(posfixTokens, &error_status);
    switch (error_status) {
        case Err_ok:
            return res;
            break;
        case Err_stackUnderflow:
            fprintf(stderr, "ERROR: Err_stackUnderflow in Line: %d file: %s", __LINE__, __FILE__);
            exit(1);
            break;
        case Err_stackOverflow:
            fprintf(stderr, "ERROR: Err_stackOverflow in Line: %d file: %s", __LINE__, __FILE__);
            exit(1);
            break;
        case Err_MismatchedParentheses:
            fprintf(stderr, "ERROR: Err_MismatchedParentheses in Line: %d file: %s", __LINE__, __FILE__);
            exit(1);
            break;
        case Err_unknownToken:
            fprintf(stderr, "ERROR: Err_unknownToken in Line: %d file: %s", __LINE__, __FILE__);
            exit(1);
            break;
        case Err_stackNotEmpty:
            fprintf(stderr, "ERROR: Err_stackNotEmpty in Line: %d file: %s", __LINE__, __FILE__);
            exit(1);
            break;
        default:
            fprintf(stderr, "ERROR: unknown error in Line: %d file: %s", __LINE__, __FILE__);
            exit(1);
            break;
    }
}

// AST:

int op_to_number_of_values_table[] = {
    [Token_EOF] = -1,
    [Token_ERROR] = -1,
    [Token_NUMBER] = 0,
    [Token_NAME] = 0,
    [Token_PLUS] = 2,
    [Token_MINOS] = 2,
    [Token_CURET] = 2,
    [Token_DIV] = 2,
    [Token_MUL] = 2,
    [Token_MOD] = 2,
    [Token_OPEN]= -1,
    [Token_CLOSE] = -1, 
    [Token_EXCLAMATION] = 1,  
};

typedef struct Node_t
{
    struct Node_t* m_sons[MAX_FUNCTION_VALUES];
    bool m_is_op;
    Token* m_token;
    int m_number_of_sons;
} Node;

Node* Node_create(Token* token){
    if(!token){
        return NULL;
    }
    Node* res = (Node*)malloc(sizeof(*res));
    if(!res){
        return NULL;
    }
    res->m_token = token;
    res->m_is_op = is_operator(token);
    res->m_number_of_sons = op_to_number_of_values_table[token->type];
    return res;
}

void Node_destroy(Node* node){
    free(node);
    node = NULL;
}


struct AST_t{
    Node* m_root;
};

AST* AST_grow(Token infixTokens[MAX_TOKENS]){
    int index = 0;
    while(infixTokens[index].type != Token_EOF){

        index++;
    }
}

void AST_destroy_aux(Node* node){
    if(!node){
        return;
    }
    for(int i = 0; i < node->m_number_of_sons; i++){
        AST_destroy_aux(node->m_sons[i]);
    }
    Node_destroy(node);
}

void AST_destroy(AST* ast){
    AST_destroy_aux(ast->m_root);
}