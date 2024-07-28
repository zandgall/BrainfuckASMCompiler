#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* header = 
	".text\n"
	".global main\n"
	"main:\n"
	"	/* Save RBP and use to store RSP */\n"
	"	pushq %rbp\n"
	"	movq %rsp, %rbp\n"
	"	/* Reset RAX */\n"
	"	xor %rax, %rax\n"
	"	/* Untouched registers that are used for output */\n"
	"	movq $0, %rdi\n"
	"	movq $1, %rdx\n"
	"	/* Generate 30,000 cells */\n"
	"	movq $30000, %rax\n"
	"	GEN:\n"
	"		dec %rsp\n"
	"		movb $0, (%rsp)\n"
	"		dec %rax\n"
	"		jnz GEN\n"
	"	/* Move Stack Pointer to center of stack space */\n"
	"	addq $15000, %rsp\n"
	"	/* ~~~~~~ Begin Brain Fuck ~~~~~~ */\n";

const char* footer =
	"	popq %rbp\n"
	"	movl $0, %eax\n"
	"	call exit\n";

int stack[1024];
int stackTop = 0;
int main(int argc, char const *argv[]) {
	if(argc < 2) {
		printf("%s: Syntax: bfc [-g] input.bf\n", argv[0]);
	}	

	int debug = 0;
	const char* filepath = argv[argc-1];
	if(strcmp(argv[1], "-g")==0 || (argc > 2 && strcmp(argv[2], "-g")==0))
		debug = 1;


	int filenameLen;
	for(filenameLen = strlen(filepath)-1; filenameLen >= 0 && filepath[filenameLen]!='.'; filenameLen--);
	char *asmFilePath = malloc(filenameLen+3);
	char *filename = malloc(filenameLen+1);
	strncpy(asmFilePath, filepath, filenameLen);
	strncpy(filename, filepath, filenameLen);
	filename[filenameLen]='\0';
	asmFilePath[filenameLen]='.';
	asmFilePath[filenameLen+1]='s';
	asmFilePath[filenameLen+2]='\0';

	FILE *file = fopen(filepath, "r");
	FILE *asmFile = fopen(asmFilePath, "w+");
	
	fputs(header, asmFile);
	int i = fgetc(file);
	int rbxUpToDate = 0;
	int label = 0;
	int c = 1;
	while(i!=EOF) {
		int number = 1;
		int j = fgetc(file);
		c++;
		while(i==j) {
			number++;
			j = fgetc(file);
			c++;
		}
		switch(i) {
		case '>':
			if(number==1)
				fprintf(asmFile, "decq %%rsp\n");
			else
				fprintf(asmFile, "subq $%d, %%rsp\n", number);
			break;
		case '<':
			if(number==1)
				fprintf(asmFile, "incq %%rsp\n");
			else
				fprintf(asmFile, "addq $%d, %%rsp\n", number);
			break;
		case '-':
			if(number==1)
				fprintf(asmFile, "decb (%%rsp)\n");
			else
				fprintf(asmFile, "subb $%d, (%%rsp)\n", number);
			break;
		case '+':
			if(number==1)
				fprintf(asmFile, "incb (%%rsp)\n");
			else
				fprintf(asmFile, "addb $%d, (%%rsp)\n", number);
			break;
		case '.':
			fprintf(asmFile, "movq %%rsp, %%rsi\n");
			fprintf(asmFile, "movq -8(%%rsp), %%r8\n");
			fprintf(asmFile, "call write\n");
			fprintf(asmFile, "movq %%r8, -8(%%rsp)\n");
			break;
		case ',':
			fprintf(asmFile, "movq %%rsp, %%rsi\n");
			fprintf(asmFile, "movq -8(%%rsp), %%r8\n");
			fprintf(asmFile, "call read\n");
			fprintf(asmFile, "movb (%%rsp), %%r9b\n");
			fprintf(asmFile, "movq %%r8, -8(%%rsp)\n");
			fprintf(asmFile, "movb %%r9b, (%%rsp)\n");
			break;
		case '[':
			for(int n = 0; n < number; n++) {
				if(stackTop==1024)
					printf("%s error: Loop stack limit reached! (1024) Char: #%d\n", argv[0], c);
				else {
					fprintf(asmFile, "L%d:\ncmpb $0, (%%rsp)\nje E%d\n", label, label);
					stack[stackTop] = label;
					stackTop++;
					label++;
				}
			}
			break;
		case ']':
			for(int n = 0; n < number; n++) {
				if(stackTop == 0)
					printf("%s error: ']' present with no corresponding '['! Char: #%d\n", argv[0], c);
				else {
					stackTop--;
					fprintf(asmFile, "jmp L%d\nE%d:\n", stack[stackTop], stack[stackTop]);
				}
			}
			break;
		default:
			break;
		}
		i = j;
	}
	if(stackTop!=0)
		printf("%s error: %d '[' present with no corresponding ']'!", argv[0], stackTop);

	fputs(footer, asmFile);
	
	fclose(file);
	fclose(asmFile);
	char* command = malloc(15 + 3*debug + 2*filenameLen+1);
	sprintf(command, "gcc %s.s %s-m64 -o %s", filename, debug == 1 ? "-g " : "", filename);
	int res = system(command);
	printf("%s\n", command);

	return res;
}
