#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : mysln.c
[Description]  : 파일의 심볼릭 링크를 만드는 프로그램
[Input]        : ./mysln mysln.c mysln.c.ln
[Output]       : mysln.c을 링크하는 mysln.c.ln 생성됨
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 exit() //  프로그램 나가기
                 symlink() // 지정한 파일의 심볼릭 링크를 만들어주는 함수
                 perror() // 오류 났을때 메시지를 출력하는 함수
[특기사항]     : <stdlib.h> // exit() 함수 경고 없이 호출
                 <unistd.h> // link() 함수 호출
==================================================================*/
int main(int argc, char *argv[])
{
	if (argc != 3)  {
		fprintf(stderr, "Usage: %s source destinaion\n", argv[0]);
		exit(1);
	}

	if (symlink(argv[1], argv[2]) < 0)  {
		perror("link");
		exit(1);
	}
	return 0;
}
