#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#define	MAX_PATH	256
/*===============================================================
[Function Name] : Nothing, JustDoIt, char *path
[Description]   : 현재 디렉토리에 있는 파일 이름들을 전부 읽어 출력하는 프로그램 
[Input]         : .
[Output]        : Nothing 
[Call By]       : main()
[Calls]         : perror() // 최근에 발생한 시스템 오류 메시지를 출력
		  exit() // 프로그램 종료
		  opendir() // 디렉토리를 열고 DIR 포인터를 반환
		  printf() // 형식화된 문자열을 출력
		  strcmp() // 두 문자열을 비교하여 같으면 0, 다르면 다른 값을 반환
		  readdir() // 열린 디렉토리에서 다음 디렉토리 항목을 읽어옴
		  strcpy() // 문자열을 복사
		  strcat() // 문자열을 이어 붙임
		  closedir() // 열린 디렉토리를 닫음
		  S_ISDIR() // 주어진 파일 모드가 디렉토리인지 확인
		  lstat() // 파일의 상태 정보를 얻어옴 (심볼릭 링크 포함)
[Given]         : 
[Returns]       : Nothing
==================================================================*/
void JustDoIt(char *path)
{
	DIR				*dp;
	struct dirent	*dep;
	struct stat		statbuf;
	char			fullpath[MAX_PATH];

	if ((dp = opendir(path)) == NULL)  {
		perror("opendir");
		exit(0);
	}

	printf("\n%s:\n", path);
	while (dep = readdir(dp))  {
		if (strcmp(".", dep->d_name) == 0 || strcmp("..", dep->d_name) == 0)
			continue;
		printf("%s\n", dep->d_name);
	}

	/* rewind */
	while (dep = readdir(dp))  {
		if (strcmp(".", dep->d_name) == 0 || strcmp("..", dep->d_name) == 0)
			continue;
		strcpy(fullpath, path);
		strcat(fullpath, "/");
		strcat(fullpath, dep->d_name);
		if (lstat(fullpath, &statbuf) < 0)  {
			perror("lstat");
			exit(1);
		}
		if (S_ISDIR(statbuf.st_mode))  {
			JustDoIt(fullpath);
		}
	}
	
	closedir(dp);
}
/*===============================================================
[Program Name] : mylsr.c
[Description]  : JustDoIt을 이용하여 현재 디렉토리에 있는 파일 이름들을 전부 읽어 출력하는 프로그램
[Input]        : Nothing
[Output]       : 해당 디렉토리에 있는 모든 파일 이름들 출력
[Calls]        : JustDoIt()
[특기사항]     : 
- <stdio.h> // 표준 입출력 함수들 사용 (e.g., printf, perror)
- <sys/types.h> // 자료형 정의 (e.g., pid_t, mode_t), 파일 시스템 관련 자료형 제공
- <dirent.h> // 디렉토리 입출력 함수들 사용 (e.g., opendir, readdir, closedir)
- <sys/stat.h> // 파일 상태 및 권한 관련 함수와 매크로 정의 (e.g., lstat, S_ISDIR)
- <stdlib.h> // 일반적인 유틸리티 함수들 사용 (e.g., exit, malloc, free)
- <string.h> // 문자열 처리 함수들 사용 (e.g., strcpy, strcat, strcmp)
==================================================================*/
int main()
{
	JustDoIt(".");
	return 0;
}
