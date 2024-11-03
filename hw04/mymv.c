#include <stdio.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : mymv.c
[Description]  : 파일의 이름을 바꾸는 프로그램
[Input]        : ./mymv mymv.o oops.o // mymv.o를 oops.o로 이름을 바꿈
[Output]       : mymv.o -> oops.o
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 exit() //  프로그램 나가기
                 rename() // 지정한 파일을 다른 이름으로 바꾸는 함수
                 perror() // 오류 났을때 메시지를 출력하는 함수
[특기사항]     : <stdlib.h> // exit() 함수 경고 없이 호출
                 <unistd.h> // link() 함수 호출
==================================================================*/
int main(int argc, char *argv[])
{
	if (argc != 3)  {
		fprintf(stderr, "Usage: %s old new\n", argv[0]);
		exit(1);
	}

	if (rename(argv[1], argv[2]))  {
		perror("rename");
		exit(1);
	}
	return 0;
}
