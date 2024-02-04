#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* header = "section .data\n	memory dq 0\nsection .text\noutput:\n	mov rax, 1\n	mov rdx, 1\n	mov rsi, rbx\n	mov rdi, 1\n	syscall\n	ret\ninput:\n	mov rax, 0\n	mov rdx, 1\n	mov rsi, rbx\n	mov rdi, 0\n	syscall\n	ret\nglobal _start\n_start:\n	mov rax, 9\nmov rsi, 30000\nmov rdi, 0\nmov rdx, 3\nmov r8, -1\nmov r9, 0\nmov r10, 0x22\nsyscall\nmov qword[memory], rax\nmov rbx, qword[memory]\n";
const char* footer = "	mov rax, 60\n	mov rdi, 0\n	syscall\n";

int stack[1024];
int stackTop = 0;
int main(int argc, char const *argv[]) {
	if(argc < 2) {
		printf("%s: Syntax: bfc [-g] input.bf\n", argv[0]);
	}

	int debug = 0;
	const char* filepath = argv[1];
	if(strcmp(argv[1], "-g")==0) {
		debug = 1;
		filepath = argv[2];
	}

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
				fprintf(asmFile, "inc rbx\n");
			else
				fprintf(asmFile, "add rbx, %d\n", number);
			break;
		case '<':
			if(number==1)
				fprintf(asmFile, "dec rbx\n");
			else
				fprintf(asmFile, "sub rbx, %d\n", number);
			break;
		case '-':
			if(number==1)
				fprintf(asmFile, "dec byte[rbx]\n");
			else
				fprintf(asmFile, "sub byte[rbx], %d\n", number);
			break;
		case '+':
			if(number==1)
				fprintf(asmFile, "inc byte[rbx]\n");
			else
				fprintf(asmFile, "add byte[rbx], %d\n", number);
			break;
		case '.':
			fprintf(asmFile, "call output\n");
			break;
		case ',':
			fprintf(asmFile, "call input\n");
			break;
		case '[':
			for(int n = 0; n < number; n++) {
				if(stackTop==1024)
					printf("%s error: Loop stack limit reached! (1024) Char: #%d\n", argv[0], c);
				else {
					fprintf(asmFile, "l%d:\ncmp byte[rbx], 0\nje e%d\n",label, label);
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
					fprintf(asmFile, "cmp byte[rbx], 0\njne l%d\ne%d:\n", stack[stackTop], stack[stackTop]);
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
	char *command = malloc(49 + 10*debug + 4*filenameLen+1);
	sprintf(command, "yasm -f elf -m amd64%s '%s.s' -o '%s.o' && ld '%s.o' -o '%s'", debug==1 ? " -g dwarf2" : "", filename, filename, filename, filename);
	system(command);
	printf("%s\n", command);

	return 0;
}