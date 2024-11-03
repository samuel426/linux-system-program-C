#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : myrd.c
[Description]  : rmdir 라는 system call을 이용하여 디렉토리를 지우는 프로그램
[Input]        : ./myrd test
[Output]       : Nothing
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 exit() //  프로그램 나가기
                 rmdir() // 지정한 디렉토리 이름 지우기 
                 perror() // 오류 났을때 메시지를 출력하는 함수
[특기사항]     : <stdlib.h> // exit() 함수 경고 없이 호출
		 <unistd.h> // rmdir() 함수 호출
==================================================================*/
int main(int argc, char *argv[])
{
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s dirname\n", argv[0]);
		exit(1);
	}

	if (rmdir(argv[1]) < 0)  {
		perror("rmdir");
		exit(1);
	}
	return 0;
}
