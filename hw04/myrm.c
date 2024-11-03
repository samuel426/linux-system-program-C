#include <stdio.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : myrm.c
[Description]  : remove 라는 system call을 이용하여 파일을 지우는 프로그램
[Input]        : ./myrm myrm.o
[Output]       : Nothing
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 exit() //  프로그램 나가기
                 remove() // 지정한 경로의 파일 삭제 		 
                 perror() // 오류 났을때 메시지를 출력하는 함수
[특기사항]     : <stdlib.h> // exit() 함수 경고 없이 호출
==================================================================*/
int main(int argc, char *argv[])
{
	int		i;

	if (argc == 1)  {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(1);
	}

	for (i = 1 ; i < argc ; i++)  {
		if (remove(argv[i]) < 0)  {
			perror("remove");
			exit(1);
		}
	}
	return 0;
}
