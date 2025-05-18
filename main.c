
#include "Lexer.h"
#include "parsser.h"

int main()
{
    
    char* input = "3 + 4 * 2 / (1 - 5) ^ 2 ^ 3";
    double output = eval(input);
    printf("%.18lf\n", output);

    return 0;
}

