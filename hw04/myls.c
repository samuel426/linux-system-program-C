#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : myls.c
[Description]  : ls 라는 system call을 c로 만드는 프로그램
[Input]        : ./myls
[Output]       : 현재 디렉토리에 있는 파일 이름 전부 출력
[Calls]        : 
                 exit() //  프로그램 나가기
                 opendir() // 디렉토리 이름을 받아 해당 디렉토리 스트림을 연다. 
		 		정상적이면 DIR * 포인터가 리턴, 실패하면 NULL 리턴
		 readdir() // opendir()로 얻은 디렉토리 포인터를 사용하여 디렉토리 정보를 하나씩 읽음
		 		디렉토리 스트럼에 끝에 도달하거나, 에러가 발생하면 NULL을 리턴함
		 closedir() // 연관 함수를 사용하는 과정에서 시스템 자원을 사용하므로 DIR * 포인터를 통해 닫아줘야 함
                 perror() // 오류 났을때 메시지를 출력하는 함수
[특기사항]     : <stdlib.h> // exit() 함수 경고 없이 호출
                 <sys/types.h> // mkdir 함수에서 사용되는 권한 마스크(mode_t)도 포함되어 있는 헤더파일
		 <dirent.h> // DIR 구조체를 사용하기 위한 헤더파일 호출
==================================================================*/
int main()
{
	DIR				*dp;
	struct dirent	*dep;

	if ((dp = opendir(".")) == NULL)  {
		perror("opendir");
		exit(0);
	}

	while ((dep = readdir(dp)) != NULL)  {
		printf("%s\n", dep->d_name);
	}
	
	closedir(dp);
	return 0;
}

