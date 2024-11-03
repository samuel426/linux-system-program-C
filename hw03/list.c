#include <stdio.h>

#define	MAX_BUF	256
/*===============================================================
[Program Name] : list.c
[Description]  : 입력한 list.c를 output으로 출력하는 프로그햄
[Input]        : ./list list.c
[Output]       : list.c 파일의 전문
[Calls]        : fopen() // 1. 파일 이름, 2. 어떻게 열 것인가 ("rt"는 text 읽기)
                 perror() // 오류 났을때 메시지를 출력하는 함수
                 exit() // 프로그램 나가기
                 fclose() // 파일 닫기, 메모리 누수 방지
[특기사항]     : input을 제대로 안 넣어주면 오류남

==================================================================*/
main(int argc, char *argv[])
{
	FILE	*fp;
	char	buf[MAX_BUF];
	int		line;

	if (argc != 2)  {
		printf("Usage: %s filename\n", argv[0]);
		exit(1);
	}

	if ((fp = fopen(argv[1], "rt")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	line = 1;
	while (fgets(buf, MAX_BUF, fp))  {
		printf("%4d: %s", line++, buf);
	}

	fclose(fp);
}
