#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

char memory[30000], *pointer = &memory[0], output[30000], *outpointer = &output[0];
char* source;
int memIndex = 0;
int loopStack[1024], loopStackTop = 0;
int char_num = 0, source_len = 0, source_available = 4096, *targets;
char loop_out_flag = 0, print_out_flag = 0;

int int_flag = 0;
void intHandler(int dummy) {
	int_flag = 1;
}

int step() {
	switch(source[char_num]) {
	case '.':
		*outpointer = memory[memIndex];
		outpointer++;
		putchar(memory[memIndex]);
		fflush(stdout);
		print_out_flag = 1;
		break;
	case ',':
		memory[memIndex] = getchar();
		break;
	case '>':
		memIndex++;
		break;
	case '<':
		memIndex--;
		break;
	case '+':
		memory[memIndex]++;
		break;
	case '-':
		memory[memIndex]--;
		break;
	case '[':
		// loopStack[loopStackTop] = char_num;
		// loopStackTop++;
		// if((*pointer)==0)
		// 	while(source[++char_num]!=']');
		// loop_out_flag = 1;
		if(memory[memIndex]==0) {
			char_num = targets[char_num];
			loop_out_flag = 1;
		}
		break;
	case ']':
		// loopStackTop--;
		// if((*pointer)!=0)
		// 	char_num = loopStack[loopStackTop]-1;
		// if((*pointer)==0)
		// 	loop_out_flag = 1;
		if(memory[memIndex]!=0)
			char_num = targets[char_num];
		else
			loop_out_flag = 1;
		break;
	default:
		break;
	}
	char_num++;
	return 0;
}

int print_memory() {
	printf("\n");	
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int begin = memIndex - w.ws_col/6, endOff = 0;
	if(memIndex*3 > w.ws_col/2-3) {
		begin += 1;
		printf("~~~");
	}
	begin = MAX(begin, 0);
	if((30000 - memIndex)*3 > w.ws_col/2 + 3)
		endOff = 1;
	for(int i = begin; i < begin+w.ws_col/3-endOff*2 && i < 30000; i++) {
		if(i == memIndex)
			printf("\033[4;44m%.2x\033[0m ", (uint8_t)memory[i]);
		else
			printf("%.2x ", (uint8_t)memory[i]);
	}
	if(endOff==1)
		printf("~~~");
	printf("\n\n");
	return 0;
}

int print_source() {
	printf("\n");	
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int begin = char_num - w.ws_col/2, endOff = 0;
	if(char_num > w.ws_col/2-3) {
		begin += 3;
		endOff += 3;
		printf("~~~");
	}
	begin = MAX(begin, 0);
	if(source_len - char_num > w.ws_col/2 + 3)
		endOff += 3;
	for(int i = begin; i < begin+w.ws_col-endOff && i < source_len; i++) {
		if(i == char_num)
			printf("\033[4;44m%c\033[0m", source[i]);
		else
			printf("%c", source[i]);
	}
	if(endOff>0)
		printf("~~~");
	printf("\n\n");
	return 0;
}


int main(int argc, char const *argv[]) {
	signal(SIGINT, intHandler);

	FILE *file = fopen(argv[1], "r");
	int fc = fgetc(file);
	source = (char*)malloc(source_available);
	targets = (int*)malloc(source_available*sizeof(int));
	while(fc!=EOF) {
		switch(fc) {
		case ']':
			if(source_len >= source_available) {
				// double source space
			}
			if(loopStackTop == 0) {
				fprintf(stderr, "%s error: Unmatched ']'! Char #%d\n", argv[0], source_len);
				exit(1);
			}
			loopStackTop--;
			targets[source_len] = loopStack[loopStackTop];
			targets[loopStack[loopStackTop]] = source_len;
			source[source_len] = (char)fc;
			source_len++;
			break;
		case '[':
			loopStack[loopStackTop] = source_len;
			loopStackTop++;
			source[source_len] = (char)fc;
			source_len++;
			break;
		case '.':
		case ',':
		case '+':
		case '-':
		case '<':
		case '>':
			source[source_len] = (char)fc;
			source_len++;
		default:
			break;
		}
		fc = fgetc(file);
	}
	if(loopStackTop>0) {
		fprintf(stderr, "%s error: Unmatched '['! Char #%d\n", argv[0], loopStack[--loopStackTop]);
		exit(1);
	}
	char def_choice = '\0';
	int weird_scan = 0;
	while(1) {
		print_memory();
		print_source();
		printf("(s)tep - (e)xit - (r)un - (l)oop out - (p)rint out [%c]\n> ", def_choice);
		char choicestr[1024];
		fgets(choicestr, 1024, stdin);
		char choice = choicestr[0];
		// if(choice == '\n')
		// 	scanf("%c", &choice);
		weird_scan++;
		// if(weird_scan==2)
		// 	scanf("%c", &choice);
		if(choice == '\n')
			choice = def_choice;
		def_choice = choice;
		switch(choice) {
		case 's':
			step();
			break;
		case 'e':
			return 0;
		case 'l':
			while(char_num < source_len && loop_out_flag == 0 && int_flag == 0)
				step();
			int_flag = 0;
			loop_out_flag = 0;
			weird_scan = 1;
			break;
		case 'p':
			while(char_num < source_len && print_out_flag == 0 && int_flag == 0)
				step();
			int_flag = 0;
			print_out_flag = 0;
			weird_scan = 1;
			break;
		case 'r':
			while(char_num < source_len && int_flag == 0)
				step();
			int_flag = 0;
			break;
		}
		if(char_num==source_len) {
			print_memory();
			print_source();
			break;
		}
	}
	free(source);
	printf("Final output:\n");

	for(char* c = output; c!=outpointer; c++)
		printf("%c", *c);
	return 0;
}