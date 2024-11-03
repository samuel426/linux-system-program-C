#include <stdio.h>

/*===============================================================
[Program Name] : output.c
[Description]  : 미리 정해진 변수들을 받아 Output으로 출력하는 프로그램
[Input]        : Nothing
[Output]       : "Output: %c %s %d %#X %ld %.4f %.2lf %p\n", c, s, i, i, l, f, d, p
[Calls]        : printf()
		 putchar() // 단일 문자를 출력하는 함수, 인자로 전달된 문자 하나를 콘솔에 출력 
		 puts() // 문자열을 출력하는 함수, 인자로 전달된 문자열을 출력하고 자동으로 줄바꿈(\n)을 추가
[특기사항]     : Nothing
==================================================================*/
void main()
{
	char c = 'a', s[] = "hello";
	int i = 100; long l = 99999;
	float f = 3.14; double d = 99.999;
	int *p = &i;
	
	printf("Output: %c %s %d %#X %ld %.4f %.2lf %p\n", c, s, i, i, l, f, d, p); 
	putchar(c);
	puts(s);
}

