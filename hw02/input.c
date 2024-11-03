#include <stdio.h>

/*===============================================================
[Program Name] : input.c
[Description]  : 띄어쓰기로 입력 구분 한줄에 한번에 입력해서 Output으로 출력하는 프로그램
[Input]        : 
		 char c, s[80];
        	 int i; long l;
        	 float f; double d;

[Output]       : "Output: %c %s %d %ld %.4f %.2lf\n", c, s, i, l, f, d
		 입력받은 문자열
[Calls]        : printf() 
		 scanf() // 다양한 형식의 데이터 입력받는 함수
		 getchar() // 단일 문자를 입력받는 함수, 사용자로부터 한 문자를 읽어 반환
		 putchar() // 단일 문자를 출력하는 함수, 인자로 전달된 문자 하나를 콘솔에 출력
		 gets() // 문자열을 입력받는 함수, 한 줄의 문자열을 입력받아 해당 문자열을 반환
		 puts() // 문자열을 출력하는 함수, 인자로 전달된 문자열을 출력하고 자동으로 줄바꿈(\n)을 추가
[특기사항]     : 띄어쓰기 주의

==================================================================*/
void main()
{
	char c, s[80];
	int i; long l;
	float f; double d;
	
	scanf("%c %s %d %ld %f %lf", &c, s, &i, &l, &f, &d); 
	printf("Output: %c %s %d %ld %.4f %.2lf\n", c, s, i, l, f, d); 

	c = getchar();
	putchar(c);

	gets(s);
	puts(s);
}

