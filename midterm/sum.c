#include <stdio.h>

int main(int argc, char *argv[]) {
	int a;
	int b;
	if (argc <3) {
		printf("숫자 잘못 입력");
	}
	a = atoi(argv[1]);
	b = atoi(argv[2]);

	printf("%d + %d = %d\n", a, b, a+b);
}
