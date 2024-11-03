#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : chmod.c
[Description]  : 모든 사용자에게 읽기, 쓰기 권한을 가진 bar라는 파일과
                 그룹 및 다른 사용자에 대한 읽기, 쓰기 권한을 제한하는 foo 파일의 권한을 바꾸는 프로그램
[Input]        : Nothing
[Output]       : bar라는 파일은 그룹 쓰기 권한을 없애고, 소유자 실행 권한을 없앰  
                 foo 파일은 그룹 권한과 다른 사용자 권한으로 읽기가 가능하게 변경 
[Calls]        :
                 stat() // 파일의 정보를 조회할 때 사용, 오류가 발생하면 -1 return
                 chmod() // 권한 변경 함수
                 perror() // 실행 실패시 나오는 출력값 설정
                 exit() // 프로그램 종료
[특기사항]     :
    - <sys/types.h> // 자료형 정의
    - <stdlib.h> // exit() 함수 호출할 때 쓰는 것
    - <sys/stat.h> // 파일 상태 정보 관련 정의
S_ISUID: Set User ID (SUID) 비트. 파일을 실행할 때 실행하는 사용자의 권한이 아닌, 파일 소유자의 권한으로 실행되게 함.
S_ISGID: Set Group ID (SGID) 비트. 파일을 실행할 때 실행하는 사용자가 속한 그룹이 아닌, 파일 소유 그룹의 권한으로 실행되게 함. 디렉토리의 경우 새로 생성된 파일이 부모 디렉토리의 그룹을 상속받음
.S_IRUSR: 소유자(Read) 읽기 권한.
S_IWUSR: 소유자(Write) 쓰기 권한.
S_IXUSR: 소유자(Execute) 실행 권한.
S_IRGRP: 그룹(Read) 읽기 권한.
S_IWGRP: 그룹(Write) 쓰기 권한.
S_IXGRP: 그룹(Execute) 실행 권한.
S_IROTH: 다른 사용자(Read) 읽기 권한.
S_IWOTH: 다른 사용자(Write) 쓰기 권한.
S_IXOTH: 다른 사용자(Execute) 실행 권한.
==================================================================*/
int main()
{
	struct stat	statbuf;

	if (stat("bar", &statbuf) < 0)  {
		perror("stat");
		exit(1);
	}

	if (chmod("bar", (statbuf.st_mode & ~S_IWGRP) | S_ISUID) < 0)  {
		perror("chmod");
		exit(1);
	}

	/* S_IRUSR, S_IWUSR, S_IRGRP, S_IROTH : ON */
	if (chmod("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)  {
		perror("chmod");
		exit(1);
	}
	return 0;
}

