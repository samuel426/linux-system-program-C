#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : mymd.c
[Description]  : mkdir 라는 system call을 이용하여 디렉토리 만드는 프로그램
[Input]        : ./mymd test
[Output]       : test의 이름을 가진 디렉토리
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 exit() //  프로그램 나가기
                 mkdir() // 지정한 이름의 디렉토리 생성, 두번째 입력은 권한 설정
                 perror() // 오류 났을때 메시지를 출력하는 함수
[특기사항]     : <stdlib.h> // exit() 함수 경고 없이 호출
		 <sys/types.h> // mkdir 함수에서 사용되는 권한 마스크(mode_t)도 포함되어 있는 헤더파일
		 <sys/stat.h> // 파일과 디렉토리의 상태와 권한을 조작하는 데 필요한 매크로와 상수들을 정의함
==================================================================*/
int main(int argc, char *argv[])
{
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s dirname\n", argv[0]);
		exit(1);
	}

	/* 755 permission */
	if (mkdir(argv[1], 0755) < 0)  {
		perror("mkdir");
		exit(1);
	}
	return 0;
}
