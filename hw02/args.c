#include <stdio.h>

/*===============================================================
[Program Name] : args.c  void main(int argc, char *argv[]) 
[Description]  : 프로그램의 기능을 간단하게 요약
		 - argc는 인수의 개수를 나타내는 정수 최소값은 1이며, 프로그램 자체의 이름도 인수로 포함
 		 - argv[]는 인수의 배열, argv[0]은 프로그램의 이름, 
		 - 그 뒤에 나오는 값들이 명령줄에서 입력된 인수들을 나타냄.
		 - argv[0]=./args
[Input]        : 
	int argc // 입력 인자의 개수
	char *argc[] // 입력 인자의 문자열 배열	
[Output]       : 처음엔 
		- argv[0]=./args
		 다음엔 입력한 것 순서대로 출력된다.
[Calls]        : printf()
[특기사항]     : 다음 사항을 명기
    - 띄어쓰기 하셔야 입력에 인식이 됩니다.

==================================================================*/

void main(int argc, char *argv[])
{
	int		i;

	for (i = 0 ; i < argc ; i++)
		printf("argv[%d]=%s\n", i, argv[i]);
}

