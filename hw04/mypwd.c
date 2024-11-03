#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define	MAX_BUF	256

/*===============================================================
[Program Name] : mypwd.c
[Description]  : getcwd 라는 system call을 함수를 이용하여 현재 디렉토리 경로를 출력하는 함수
[Input]        : ./mypwd
[Output]       : /home/lsp34/hw04 
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 exit() //  프로그램 나가기
                 getcwd() // 현재 디렉토리의 절대 경로를 저장할 버퍼의 주소와 버퍼의 크기를 인자로 받음
			 만약 버퍼의 주소가 NULL 이면, getcwd는 직접 malloc으로 메모리를 할당하고 주소를 리턴한다. 
			 getcwd는 수행 도중 오류가 발생하면 널을 리턴한다.
                 perror() // 오류 났을때 메시지를 출력하는 함수
[특기사항]     : <stdlib.h> // exit() 함수 경고 없이 호출
                 <unistd.h> // POSIX 시스템 호출 사용을 위한 헤더 파일 (e.g., chdir, fork, exec)
==================================================================*/

int main(int argc, char *argv[])
{
	char	buf[MAX_BUF];

	if (getcwd(buf, MAX_BUF) == NULL)  {
		perror("getcwd");
		exit(1);
	}

	printf("%s\n", buf);
	return 0;
}
