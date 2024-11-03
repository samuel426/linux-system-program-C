#include <stdio.h>
#include <stdlib.h>

void myexit1()
{
	printf("first exit handler\n");
}

void myexit2()
{
	printf("second exit handler\n");
}
/*===============================================================
[Program Name] : exit.c 종료 핸들러 등록 예제
[Description]  : atexit() 함수를 사용해 프로그램 종료 시 실행할 함수를 등록하고, 
                 종료 시점에 등록한 함수가 호출되는 순서를 보여줌.
[Input]        : 없음. 사용자 입력을 받지 않음.
[Output]       : "main is done" 이후 등록된 종료 핸들러 순서대로 
                 "first exit handler", "second exit handler" 출력.
[Calls]        : 
    - atexit() : 프로그램 종료 시 호출할 함수를 등록
    - perror() : atexit() 실패 시 오류 메시지를 표준 오류로 출력
    - exit()   : 프로그램을 특정 상태 코드로 종료
[특기사항]     : 
    - atexit()로 등록된 종료 핸들러는 프로그램 종료 시 
      등록된 순서의 역순으로 호출됨.
    - <stdlib.h>: 종료 함수 관련 라이브러리 제공
==================================================================*/
int main()
{
	/* Add myexit2 to exit functions */
	if (atexit(myexit2) != 0)  {
		perror("atexit");
		exit(1);
	}

	if (atexit(myexit1) != 0)  {
		perror("atexit");
		exit(1);
	}

	if (atexit(myexit1) != 0)  {
		perror("atexit");
		exit(1);
	}

	printf("main is done\n");
}
