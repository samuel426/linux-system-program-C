#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : umask.c
[Description]  : 모든 사용자에게 읽기, 쓰기 권한을 가진 bar라는 파일과
                 그룹 및 다른 사용자에 대한 읽기, 쓰기 권한을 제한하는 foo 파일을 생성하는 프로그램
[Input]        : Nothing
[Output]       : 모든 사용자에게 읽기, 쓰기 권한을 가진 bar라는 파일을 생성.
		 그룹 및 다른 사용자에 대한 읽기, 쓰기 권한을 제한하는 foo 파일 생성
[Calls]        : 
		 umask() // 새로 생성되는 파일에게 권한을 부여
		 creat() // 파일 생성 함수
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
	umask(0);
	if (creat("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | 
				S_IROTH | S_IWOTH) < 0)  {
		perror("creat");
		exit(1);
	}

	/* Set file mode creation mask: S_IRGRP, S_IWGRP, S_IROTH, S_IWOTH */
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (creat("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | 
				S_IROTH | S_IWOTH) < 0)  {
		perror("creat");
		exit(1);
	}

	return 0;
}
