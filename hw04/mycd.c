#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : mycd.c
[Description]  : chdir 라는 system call을 함수를 이용하여 입력한 디렉토리로 이동하는 프로그램
		 하지만 ...!
		 독립된 프로세스 환경으로
		 프로세스의 독립성: 
		 	터미널에서 실행한 프로그램은 자식 프로세스로 실행되어
		 	자식 프로세스에서 이루어진 디렉토리 변경은 부모 프로세스(터미널)에는 영향을 미치지 않는다. 
		 	이는 각 프로세스가 독립적인 환경을 유지하는 유닉스 철학 때문
		 
		 자식은 부모의 환경을 변경할 수 없음: 
		 	자식 프로세스에서 작업 디렉토리를 변경해도 해당 변경은 자식 프로세스에서만 유효. 
			프로세스 간에는 독립된 메모리 공간과 환경이 유지되므로, 
			자식 프로세스에서의 변경 사항은 부모 프로세스에 전파되지 않는다.
		결론 = 프로그램이 종료되면서 작업 디렉토리는 원래 상태로 돌아가
	       		프로그램을 실행해도 바뀌는건 없다.

[Input]        : ./mycd ..
[Output]       : Nothing
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 exit() //  프로그램 나가기
                 chdir() // 지정한 이름의 디렉토리 생성, 두번째 입력은 권한 설정
                 perror() // 오류 났을때 메시지를 출력하는 함수
[특기사항]     : <stdlib.h> // exit() 함수 경고 없이 호출
		 <unistd.h> // POSIX 시스템 호출 사용을 위한 헤더 파일 (e.g., chdir, fork, exec) 
==================================================================*/
int main(int argc, char *argv[])
{
	if (argc != 2)  {
		fprintf(stderr, "Usage: %s dirname\n", argv[0]);
		exit(1);
	}

	if (chdir(argv[1]) < 0)  {
		perror("chdir");
		exit(1);
	}
	return 0;
}
