/*
MIT License

Copyright (c) 2025 huywallz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Macro {
    char name;
    char *value;
    struct Macro *next;
} Macro;

Macro *macro_list = NULL;

void add_macro(char name, char *value) {
    Macro *new_macro = malloc(sizeof(Macro));
    if (!new_macro) {
        fprintf(stderr, "%s: Memory allocation error", progname);
        exit(1);
    }
    new_macro->name = name;
    new_macro->value = strdup(value);
    new_macro->next = macro_list;
    macro_list = new_macro;
}

char* find_macro(char name) {
    for (Macro *macro = macro_list; macro != NULL; macro = macro->next) {
        if (macro->name == name) {
            return macro->value;
        }
    }
    return NULL;
}

void free_macros() {
    while (macro_list) {
        Macro *temp = macro_list;
        macro_list = macro_list->next;
        free(temp->value);
        free(temp);
    }
}

char* progname;

void process_code(char* code, char* outname, FILE* output) {
    int bracket_balance = 0;
    for (char *c = code; *c;) {
        // Handle macros
        if (*c == '#') {
            char name = *(c + 1);
            if (strchr("#+-<>[].,\n\t \f\r\v", name) || !isprint(name)) {
                remove(outname);
                fprintf(stderr, "%s: Invalid macro character '%c' at position %ld\n", progname, name, c - code);
                exit(1);
            }

            c += 2;
            char *start = c;
            while (*c && *c != '\n') c++;
            char *value = strndup(start, c - start);
            add_macro(name, value);
            free(value);
            continue;
        }

        // Include file
        if (*c == '$') {
            c++;
            char* start = c;
            while (*c && *c != '\n') c++;
            char *filename = strndup(start, c - start);
            FILE *file = fopen(filename, "r");
            if (!file) {
                remove(outname);
                fprintf(stderr, "%s: Unable to include file '%s'\n", progname, filename);
                exit(1);
            }

            fseek(file, 0, SEEK_END);
            long size = ftell(file);
            fseek(file, 0, SEEK_SET);

            char* content = malloc(size + 1);
            if (!content) {
                remove(outname);
                fprintf(stderr, "%s: Memory allocation error", progname);
                exit(1);
            }

            fread(content, 1, size, file);
            content[size] = '\0';
            fclose(file);

            process_code(content, filename, output);
            free(content);
            free(filename);
            continue;
        }

        // Skip comments
        if (*c == ';') {
            while (*c && *c != '\n') c++;
            continue;
        }

        // Handle macros
        char* macro_value = find_macro(*c);
        if (macro_value) {
            process_code(macro_value, outname, output);
            c++;
            continue;
        }

        // Code generation
        int repeat_count = 1;
        switch (*c) {
            case '>':
            case '<':
                while (*(c + repeat_count) == *c) repeat_count++;
                fprintf(output, "p%c=%d;f;", 
                        (*c == '>') ? '+' : '-', repeat_count);
                c += repeat_count;
                break;

            case '+':
            case '-':
                while (*(c + repeat_count) == *c) repeat_count++;
                fprintf(output, "*p%c=%d;", (*c == '+') ? '+' : '-', repeat_count);
                c += repeat_count;
                break;

            case '.':
                fprintf(output, "o;");
                c++;
                break;

            case ',':
                fprintf(output, "g;");
                c++;
                break;

            case '[':
                bracket_balance++;
                fprintf(output, "w{");
                c++;
                break;

            case ']':
                bracket_balance--;
                if (bracket_balance < 0) {
                    remove(outname);
                    fprintf(stderr, "%s: Unmatched ']' at position %ld\n", progname, c - code);
                    exit(1);
                }
                fprintf(output, "}");
                c++;
                break;

            default:
                c++;
                break;
        }
    }

    if (bracket_balance != 0) {
        remove(outname);
        fprintf(stderr, "%s: Unmatched '['\n", progname);
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    progname = argv[0];
    if (argc != 3) {
        printf("Usage: %s <input.bfc> <output.c>\n", argv[0]);
        exit(1);
    }

    FILE* input_file = fopen(argv[1], "r");
    if (!input_file) {
        fprintf(stderr, "%s: Unable to open file '%s'", progname, argv[1]);
        exit(1);
    }

    fseek(input_file, 0, SEEK_END);
    long size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    char* code = malloc(size + 1);
    if (!code) {
        fprintf(stderr, "%s: Memory allocation error", progname);
        exit(1);
    }

    fread(code, 1, size, input_file);
    code[size] = '\0';
    fclose(input_file);

    FILE* output_file = fopen(argv[2], "w");
    if (!output_file) {
        fprintf(stderr, "%s: Unable to open file '%s'", progname, argv[2]);
        exit(1);
    }

    fprintf(output_file, "#include <stdio.h>\n#include <stdlib.h>\n#include <ctype.h>\n"
                         "#define w while(*p)\n#define g *p=getchar()\n#define o putchar(*p)\n#define f if(p-t>=s)t=realloc(t,s=p-t+1)\n"
                         "int main(){char*t=malloc(1);size_t s=1;char*p=t;");

    process_code(code, argv[2], output_file);

    free(code);

    fprintf(output_file, "free(t);exit(0);}");

    fclose(output_file);

    free_macros();
    return 0;
}
