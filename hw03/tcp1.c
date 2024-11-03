#include <stdio.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : tcp1.c
[Description]  : 입력한 tcp1.c를 tcp1.bak으로 복사하는 프로그햄
[Input]        : ./tcp1 tcp1.c tcp1.bak
[Output]       : tcp1.bak 파일
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
		 fopen() // 1. 파일 이름, 2. 어떻게 열 것인가 ("rt"는 text 읽기, "wt"는 text 쓰기)
                 perror() // 오류 났을때 메시지를 출력하는 함수
                 exit() // 프로그램 나가기
		 fgetc() // 문자열 하나씩 파일을 읽는 함수 != EOF는 더이상 없는지 판별 여부
                 fputc() // 문자열 하나씩 읽고, 받은 문자 ch를 해당 파일에 넣음
		 fclose() // 파일 닫기, 메모리 누수 방지
[특기사항]     : input을 제대로 안 넣어주면 오류남
		 exit() 함수를 사용하기 위해 <stdlib.h>를 가져왔음
==================================================================*/
main(int argc, char *argv[])
{
	FILE	*src, *dst;
	int		ch;

	if (argc != 3)  {
		fprintf(stderr, "Usage: %s source destination\n", argv[0]);
		exit(1);
	}

	if ((src = fopen(argv[1], "rt")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	if ((dst = fopen(argv[2], "wt")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	while ((ch = fgetc(src)) != EOF) {	
		fputc(ch, dst);
	}

	fclose(src);
	fclose(dst);
}
