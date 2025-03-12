/*
Copyright (c) 2025 Huywallz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
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
		perror("Memory allocation error");
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
int target;
char* mainname;

enum {
	TARGET_C,
	TARGET_BF,
	TARGET_C0,
};

int get_target(char* name) {
	if (strcmp(name, "c") == 0) return TARGET_C;
	if (strcmp(name, "bf") == 0) return TARGET_BF;
	if (strcmp(name, "c0") == 0) return TARGET_C0;
	return -1;
}

void process_code(char* code, char* outname, FILE* output) {
	int bracket_balance = 0;
	for (char *c = code; *c;) {
		// Handle macros
		if (*c == '#') {
			char name = *(c + 1);
			if (strchr("$#+-<>[].,\n\t \f\r\v", name) || !isprint(name)) {
				if (strcmp(mainname, outname) == 0) remove(outname);
				fprintf(stderr, "%s:%s: Invalid macro character '%c' at position %ld\n", progname, outname, name, c - code);
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
				if (strcmp(mainname, outname) == 0) remove(outname);
				fprintf(stderr, "%s:%s: Unable to include file '%s'\n", progname, outname, filename);
				exit(1);
			}

			fseek(file, 0, SEEK_END);
			long size = ftell(file);
			fseek(file, 0, SEEK_SET);

			char* content = malloc(size + 1);
			if (!content) {
				if (strcmp(mainname, outname) == 0) remove(outname);
				fprintf(stderr, "%s:%s: Memory allocation error", progname, outname);
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
		if (target == TARGET_C) {
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
					fprintf(output, "*p%c=%d;", *c, repeat_count);
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
						if (strcmp(mainname, outname) == 0) remove(outname);
						fprintf(stderr, "%s:%s: Unmatched ']' at position %ld\n", progname, outname, c - code);
						exit(1);
					}
					fprintf(output, "}");
					c++;
					break;
	
				default:
					c++;
					break;
			}
		} else if (target == TARGET_BF) {
			if (strchr("><+-[].,", *c)) fprintf(output, "%c", *c);
			c++;
		} else if (target == TARGET_C0) {
			int repeat_count = 1;
			switch (*c) {
				case '>':
				case '<':
					while (*(c + repeat_count) == *c) repeat_count++;
					fprintf(output, "%c(%d);", 
						(*c == '>') ? 'a' : 'b', repeat_count);
					c += repeat_count;
					break;
	
				case '+':
				case '-':
					while (*(c + repeat_count) == *c) repeat_count++;
					fprintf(output, "%c(%d);", (*c == '+') ? 'c' : 'd', repeat_count);
					c += repeat_count;
					break;
	
				case '.':
					fprintf(output, "e();");
					c++;
					break;
	
				case ',':
					fprintf(output, "f();");
					c++;
					break;
	
				case '[':
					bracket_balance++;
					fprintf(output, "while(*p){");
					c++;
					break;
	
				case ']':
					bracket_balance--;
					if (bracket_balance < 0) {
						if (strcmp(mainname, outname) == 0) remove(outname);
						fprintf(stderr, "%s:%s: Unmatched ']' at position %ld\n", progname, outname, c - code);
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
	}

	if (bracket_balance != 0) {
		if (strcmp(mainname, outname) == 0) remove(outname);
		fprintf(stderr, "%s:%s: Unmatched '['\n", progname, outname);
		exit(1);
	}
}

void help(void) {
	printf("Usage: %s [options] [filepath]\n", progname);
	printf("Available options are:\n");
	printf("  -o name  output to file 'name' (default is \"bc.out\")\n");
	printf("  --output, --out\n");
	printf("  -t	   Compile to the specified language (bf, c, c0)\n");
	printf("  --target\n");
	printf("  -h	   show this list\n");
	printf("  --help\n");
	printf("  -v	   show version information\n");
	printf("  --version, --ver\n");
}

void ver(void) {
	printf("Braincrap v1.1  (C) Copyright 2025 Huywallz");
}

int main(int argc, char *argv[]) {
	progname = strrchr(argv[0], '/');
	#ifdef _WIN32
		if (!progname) progname = strrchr(argv[0], '\\');
	#endif
	progname = progname ? progname + 1 : argv[0];
	if (argc == 1) {
		help();
		exit(0);
	}
	
	char* outname = "bc.out";;
	target = TARGET_C;
	int i = 1;
	while (i < argc && argv[i][0] == '-') {
		if (argv[i][1] == '-') {
			if (strcmp(argv[i], "--help") == 0) {
				help();
			} else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "--out") == 0) {
				if (i + 1 > argc) {
					fprintf(stderr, "%s: Argument to '%s' is missing\n", progname, argv[i]);
					exit(1);
				}
				outname = argv[++i];
			} else if (strcmp(argv[i], "--target") == 0) {
				if (i + 1 > argc) {
					fprintf(stderr, "%s: Argument to '%s' is missing\n", progname, argv[i]);
					exit(1);
				}
				char* targetname = argv[++i];
				target = get_target(targetname);
				if (target == -1) {
					fprintf(stderr, "%s: Unknown target language '%s'\n", progname, targetname);
					exit(1);
				}
			} else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "--ver") == 0) {
				ver();
			} else {
				fprintf(stderr, "%s: Unknown argument '%s'\n", progname, argv[i]);
				exit(1);
			}
		} else if (strlen(argv[i]) == 2) switch (argv[i][1]) {
			case 'o': {
				if (i + 1 > argc) {
					fprintf(stderr, "%s: Argument to '%s' is missing\n", progname, argv[i]);
					exit(1);
				}
				outname = argv[++i];
			} break;
			case 't': {
				if (i + 1 > argc) {
					fprintf(stderr, "%s: Argument to '%s' is missing\n", progname, argv[i]);
					exit(1);
				}
				char* targetname = argv[++i];
				target = get_target(targetname);
				if (target == -1) {
					fprintf(stderr, "%s: Unknown target language '%s'\n", progname, targetname);
					exit(1);
				}
			} break;
			case 'h': {
				help();
			} break;
			case 'v': {
				ver();
			} break;
		} else {
			fprintf(stderr, "%s: Unknown argument '%s'\n", progname, argv[i]);
			exit(1);
		}
		i++;
	}
	if (i + 1 > argc) {
		fprintf(stderr, "%s: No input file\n", progname);
		exit(1);
	}
	char* inname = argv[i];
	mainname = inname;

	FILE* input_file = fopen(inname, "r");
	if (!input_file) {
		fprintf(stderr, "%s: Unable to open file '%s'", progname, inname);
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
	
	FILE* output_file = fopen(outname, "w+");
	if (!output_file) {
		fprintf(stderr, "%s: Unable to open file '%s'", progname, outname);
		exit(1);
	}
	
	if (target == TARGET_C)
		fprintf(output_file, "#include <stdio.h>\n#include <stdlib.h>\n"
							 "#define w while(*p)\n#define g *p=getchar()\n#define o putchar(*p)\n#define f if(p-t>=s)t=realloc(t,s=p-t+1)\n"
							 "int main(){char*t=malloc(1);size_t s=1;char*p=t;");
	else if (target == TARGET_C0)
		fprintf(output_file, "#include <stdio.h>\n#include <stdlib.h>\n"
							 "char*t;size_t s=1;char*p;"
							 "void a(size_t n){p+=n;if(p-t>=s)t=realloc(t,s=p-t+1);}"
							 "void b(size_t n){p-=n;}"
							 "void c(int n){*p+=n;}"
							 "void d(int n){*p-=n;}"
							 "void e(){putchar(*p);}"
							 "void f(){*p=getchar();}"
							 "int main(){t=malloc(1);p=t;");

	process_code(code, argv[2], output_file);

	free(code);

	if (target == TARGET_C || target == TARGET_C0)
		fprintf(output_file, "free(t);exit(0);}");

	fclose(output_file);

	free_macros();
	return 0;
}
