#include <stdio.h>
/*===============================================================
[Program Name] : env.c 환경 변수 및 커맨드라인 인수 출력 프로그램
[Description]  : 커맨드라인에서 전달된 인수와 시스템의 환경 변수를 출력하는 프로그램.
[Input]        : Nothing
[Output]       : 
                 - 커맨드라인 인수 리스트
                 - 환경 변수 리스트 (environ과 envp를 통해)
[Calls]        : Nothing.
[특기사항]     : 
    - 프로그램 실행 시 인수로 전달된 내용은 argc와 argv 배열을 통해 접근 가능.
    - 환경 변수는 environ 및 envp 배열을 통해 접근 가능.
==================================================================*/
int main(int argc, char *argv[], char *envp[])
{
	int			i;
	char		**p;
	extern char	**environ;

	printf("List command-line arguments\n");
	for (i = 0 ; i < argc ; i++)  {
		printf("%s\n", argv[i]);
	}

	printf("\n");
	printf("List environment variables from environ variable\n");
#if 1
	for (i = 0 ; environ[i] != NULL ; i++)  {
		printf("%s\n", environ[i]);
	}
#else
	for (p = environ ; *p != NULL ; p++)  {
		printf("%s\n", *p);
	}
#endif

	printf("\n");
	printf("List environment variables from envp variable\n");
#if 1
	for (i = 0 ; envp[i] != NULL ; i++)  {
		printf("%s\n", envp[i]);
	}
#else
	for (p = envp ; *p != NULL ; p++)  {
		printf("%s\n", *p);
	}
#endif
}
