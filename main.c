
#include "Lexer.h"
#include "parsser.h"

#define MAX_TEST_LINE 256

typedef enum {
    ProgramSeting_expression = 0,
    ProgramSeting_file,
    ProgramSeting_noArgs,
    ProgramSeting_toMuchArgs,
} ProgramSeting;

bool test1();

ProgramSeting getProgramSeting(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    if(argc != 2){
        fprintf(stderr, "USEAGE: eval \"expression\", example: eval \"8 - (2 ^ 3) * 2\"\n");
        return 1;
    }

    double res = eval(argv[1]);
    printf("%f\n", res);

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
        fprintf(outfile, "%f\n", eval(line));
    }

    fclose(infile);
    fclose(outfile);

    return true;
}

ProgramSeting getProgramSeting(int argc, char* argv[]){

    if(argc < 2){
        return ProgramSeting_noArgs;
    }

    if(argc > 3){
        return ProgramSeting_toMuchArgs;
    }
    
    for(int i = 0; i < argc; i++){
        if (strcmp(argv[i], "-file") == 0){
            return ProgramSeting_file;
        }
        else if (strcmp(argv[i], "-expr") == 0){
            return ProgramSeting_expression;
        }
        else {
            return ProgramSeting_expression;
        }
    }

    return ProgramSeting_expression;
}
