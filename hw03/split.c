#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define	MAX_BUF	1024
/*===============================================================
[Program Name] : split.c
[Description]  : 입력한 실행파일(merge)을 a, b로 쪼개는 프로그햄
[Input]        : ./split merge a b
[Output]       : a, b 파일
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
		 ftell() // 파일 포인터의 위치를 반환 
		 		filesize 함수 기준으로 처음은 현재 위치를 가르킴
				두번째는 파일 끝의 오프셋을 가르킴 = 파일의 크기
		 fseek() // 파일 포인터를 특정 위치로 옮김
		 		SEEK_END = 파일 끝으로 이동, 0L은 파일 끝의 오프셋
				SEEK_SET = 파일의 시작점으로 이동, 이후 cur 만큼 이동함 
[특기사항]     : input을 제대로 안 넣어주면 오류남
                 exit() 함수를 사용하기 위해 <stdlib.h>를 가져왔음
		 split할 파일 split될 파일1, 파일2
		 입력 파일src를 받아 크기를 반환하는 filesize 함수가 있음
		 <stdlib.h> 파일 입출력, 프로세스 제어, 시스템 정보 등을 제공하는 다양한 함수가 있다.
		 여기선 해당 함수가 따로 쓰이진 않았음
==================================================================*/
long filesize(FILE *fp)
{
	long	cur, size;

	cur = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, cur, SEEK_SET);
	return(size);
}

main(int argc, char *argv[])
{
	FILE	*src, *dst1, *dst2;
	char	buf[MAX_BUF];
	int		count, size;

	if (argc != 4)  {
		fprintf(stderr, "Usage: %s source dest1 dest2\n", argv[0]);
		exit(1);
	}

	if ((src = fopen(argv[1], "rb")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((dst1 = fopen(argv[2], "wb")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((dst2 = fopen(argv[3], "wb")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	size = filesize(src) / 2;

	while (size > 0)  {
		count = (size > MAX_BUF) ?  MAX_BUF : size;
		fread(buf, 1, count, src); 
		fwrite(buf, 1, count, dst1);
		size -= count;
	}

	while ((count = fread(buf, 1, MAX_BUF, src)) > 0)  {
		fwrite(buf, 1, count, dst2);
	}

	fclose(src);
	fclose(dst1);
	fclose(dst2);
}
