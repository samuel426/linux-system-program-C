#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "record.h"
/*===============================================================
[Program Name] : conv.c
[Description]  : 구조체를 이용하여 입력한 text파일을 복사하여,
		 구조체 형태의 바이너리 파일을 생성한다.
[Input]        : ./conv test.in test.out
[Output]       : test.out 바이너리 파일
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
		 strchr() // rec 요소중 \n을 찾아 \0 으로 바꿔준다.
		 fgets() // rec 요소 하나를 받는다. 
[특기사항]     : input을 제대로 안 넣어주면 오류남
                 exit() 함수를 사용하기 위해 <stdlib.h>를 가져왔음
                 <stdlib.h> 파일 입출력, 프로세스 제어, 시스템 정보 등을 제공하는 다양한 함수가 있다.
                 여기선 해당 함수가 따로 쓰이진 않았음
                 <string.h> strchr 때문에 추가된 헤더파일
                 Record 구조체를 담고 있는 record.h 헤더파일 사용
==================================================================*/
main(int argc, char *argv[])
{
	FILE	*src, *dst;
	Record	rec;

	if (argc != 3)  {
		fprintf(stderr, "Usage: %s source dest\n", argv[0]);
		exit(1);
	}

	if ((src = fopen(argv[1], "rt")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	if ((dst = fopen(argv[2], "wb")) == NULL)  {
		perror("fopen");
		exit(1);
	}

	while (fgets(rec.stud, SMAX, src))  {
		*strchr(rec.stud, '\n') = '\0';
		fgets(rec.num, NMAX, src);
		*strchr(rec.num, '\n') = '\0';
		fgets(rec.dept, DMAX, src);
		*strchr(rec.dept, '\n') = '\0';
		fwrite(&rec, sizeof(Record), 1, dst);
	}

	fclose(src);
	fclose(dst);
}
