#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
/*===============================================================
[Program Name] : stat.c
[Description]  : 각 파일의 속성에 대한 정보를 보여주는 프로그램.
[Input]        : ./stat stat.c
		 프로그램 이름 & 탐색하고자 하는 파일 이름
[Output]       : 
stat.c: regular // 파일 종류를 출력
        st_mode = 33188 // 파일 유형과 권한을 나타내는 값.
        st_ino = 422957 // 파일의 inode 번호.
        st_dev = 64768 // 파일이 위치한 디바이스 ID.
        st_rdev = 0 // 특수 파일의 경우 장치 ID.
        st_nlink = 1 // 하드 링크의 개수.
        st_uid = 1034 // 파일 소유자의 사용자 ID.
        st_gid = 1050 // 파일 소유자의 그룹 ID.
        st_size = 1357 // 파일 크기 (바이트 단위).
        st_atime = 1729758102 // 파일의 마지막 접근 시간 (epoch 기준 초 단위).
        st_mtime = 1729758094 // 파일의 마지막 수정 시간.
        st_ctime = 1729758094 // inode 변경 시간.
        st_blksize = 4096 // 파일 시스템 I/O에서 사용되는 블록 크기.
        st_blocks = 8 // 파일에 할당된 512바이트 블록 수.

[Calls]        : printf() // 출력 함수
		 lstat() // 파일 상태 정보 가져옴
		 perror() // 오류 발생 시 메시지 출력
		 S_ISDIR() // 디렉토리인지 확인
		 S_ISCHR() // 문자 장치 파일인지 확인
		 S_ISBLK() // 블록 장치 파일인지 확인
		 S_ISFIFO() // FIFO 파일인지 확인
		 S_ISLNK() // 심볼릭 링크인지 확인
		 S_ISSOCK() // 소켓 파일인지 확인

[특기사항]     : 
    - input 양식에 맞추기 안맞추면 lstat 나옴 
    - <sys/types.h> // 자료형 정의
    - <sys/stat.h> // 파일 상태 정보 관련 정의

==================================================================*/

int main(int argc, char *argv[]) {
	struct stat statbuf;
	char *mode;
	int i;

	for (i = 1; i < argc; i++) {
		printf("%s: ", argv[i]);
		if(lstat(argv[i], &statbuf) < 0) {
			perror("lstat");
			continue;
		}
		if(S_ISREG(statbuf.st_mode))
			mode = "regular";
		else if (S_ISDIR(statbuf.st_mode))
			mode = "directory";
		else if (S_ISCHR(statbuf.st_mode))
			mode = "character special";
		else if (S_ISBLK(statbuf.st_mode))
			mode = "block special";
		else if (S_ISFIFO(statbuf.st_mode))
			mode = "FIFO";
		else if (S_ISLNK(statbuf.st_mode))
			mode = "symbolic link";
		else if (S_ISSOCK(statbuf.st_mode))
			mode = "socket";
		printf("%s\n", mode);
		printf("\tst_mode = %d\n", statbuf.st_mode);
		printf("\tst_ino = %ld\n", statbuf.st_ino);
		printf("\tst_dev = %ld\n", statbuf.st_dev);
		printf("\tst_rdev = %ld\n", statbuf.st_rdev);
		printf("\tst_nlink = %ld\n", statbuf.st_nlink);
		printf("\tst_uid = %d\n", statbuf.st_uid);
		printf("\tst_gid = %d\n", statbuf.st_gid);
		printf("\tst_size = %ld\n", statbuf.st_size);
		printf("\tst_atime = %ld\n", statbuf.st_atime);
		printf("\tst_mtime = %ld\n", statbuf.st_mtime);
		printf("\tst_ctime = %ld\n", statbuf.st_ctime);
		printf("\tst_blksize = %ld\n", statbuf.st_blksize);
		printf("\tst_blocks = %ld\n", statbuf.st_blocks);
	}
}
