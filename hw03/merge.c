#include <stdio.h>
#include <stdlib.h>

#define	MAX_BUF	1024
/*===============================================================
[Program Name] : merge.c
[Description]  : 입력한 a, b를 merge.new으로 합치는 프로그햄
[Input]        : ./merge a b merge.new 
[Output]       : mycp2.bak 파일
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 fopen() // 1. 파일 이름, 2. 어떻게 열 것인가 ("rb"는 binary 읽기, "wb"는 binary 쓰기)
                 perror() // 오류 났을때 메시지를 출력하는 함수
                 exit() // 프로그램 나가기
                 fread() // 파일의 문자열을 읽어 buf에 저장하는 함수,
                                읽을 데이터의 크기 char는 1byte이기 때문에 1
                                MAX_BUF은 한번에 얼마까지 적을수 있는지
                                src는 읽고자 하는 파일
                 fwrite() // 받은 문자열 buf를 파일 dst에 넣는 함수
                                count가 0 또는 1이기 때문에 하나씩 쓰게 된다.
                 fclose() // 파일 닫기, 메모리 누수 방지
[특기사항]     : input을 제대로 안 넣어주면 오류남
                 exit() 함수를 사용하기 위해 <stdlib.h>를 가져왔음
		 먼저 split한 두 파일을 순서 제대로 해서 입력을 해야함 아니면 merge되어도 실행 안됨
==================================================================*/
main(int argc, char *argv[])
{
	FILE	*src1, *src2, *dst;
	char	buf[MAX_BUF];
	int		count;

	if (argc != 4)  {
		fprintf(stderr, "Usage: %s source1 source2 dest\n", argv[0]);
		exit(1);
	}

	if ((src1 = fopen(argv[1], "rb")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((src2 = fopen(argv[2], "rb")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((dst = fopen(argv[3], "wb")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	while ((count = fread(buf, 1, MAX_BUF, src1)))  {
		fwrite(buf, 1, count, dst);
	}

	while ((count = fread(buf, 1, MAX_BUF, src2)))  {
		fwrite(buf, 1, count, dst);
	}

	fclose(src1);
	fclose(src2);
	fclose(dst);
}
