
#include "Lexer.h"
#include "parsser.h"

#define MAX_TEST_LINE (256*256)

bool test1();
bool interactive_program(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    if(argc < 2){
        fprintf(stderr, "USEAGE: eval \"<expression>\", example: eval \"8 - (2 ^ 3) * 2\"\nor: eval -test\n");
        return 1;
    }
    if(strcmp(argv[1], "-test") == 0){
        if(!test1()){
            return 1;
        }
        else{
            return 0;
        }
    }
    if(!interactive_program(argc, argv)){
        return 1;
    }
    return 0;
}

bool test1(){
    const char* in_path = "input.txt";
    const char* out_path = "output.txt";
    FILE* infile = fopen(in_path, "r");
    if(!infile){
        perror(in_path);
        return false;
    }
    FILE* outfile = fopen(out_path, "w");
    if(!outfile){
        perror(out_path);
        fclose(infile);
        return false;
    }

    char line[MAX_TEST_LINE];
    while (fgets(line, sizeof(line), infile)) {
        line[strcspn(line, "\n")] = 0;
        fprintf(outfile, "%.8f\n", eval(line));
    }

    fclose(infile);
    fclose(outfile);

    return true;
}

bool interactive_program(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "USEAGE: eval \"expression\", example: eval \"8 - (2 ^ 3) * 2\"\n");
        return false;
    }

    double res = eval(argv[1]);
    printf("%f\n", res);
    return true;
}