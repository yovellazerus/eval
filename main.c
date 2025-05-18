
#include "Lexer.h"
#include "parsser.h"

#define MAX_TEST_LINE 256

int main()
{
    const char* in_path = "input.txt";
    const char* out_path = "output.txt";
    FILE* infile = fopen(in_path, "r");
    if(!infile){
        perror(in_path);
        return 1;
    }
    FILE* outfile = fopen(out_path, "w");
    if(!outfile){
        perror(out_path);
        fclose(infile);
        return 1;
    }

    char line[MAX_TEST_LINE];
    while (fgets(line, sizeof(line), infile)) {
        line[strcspn(line, "\n")] = 0;
        fprintf(outfile, "%f\n", eval(line));
    }

    fclose(infile);
    fclose(outfile);

    // char* input = "3 + 4 * 2 / (1 - 5) ^ 2 ^ 3";
    // double output = eval(input);
    // printf("%.18lf\n", output);

    return 0;
}

