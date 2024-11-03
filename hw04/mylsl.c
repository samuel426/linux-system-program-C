#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
/*===============================================================
[Function Name] : void print_permissions(mode_t mode)
[Description]   : 파일의 권한 정보를 읽어 사용자에게 보여주는 기능을 수행.
[Input]         : mode_t mode - 파일의 상태 정보를 담고 있는 mode 변수.
[Output]        : 권한 정보 (형식: -rwxr-xr-x) 를 표준 출력으로 출력.
[Call By]       : main() 함수
[Calls]         : printf()
[Given]         :
    - mode는 lstat 함수로부터 받은 파일의 상태 정보로 전달됨.
    - mode의 값은 S_ISDIR()와 같은 매크로를 통해 해석되어야 함.
[Returns]       : 
	Nothing
==================================================================*/

void print_permissions(mode_t mode) {
    printf("%c", S_ISDIR(mode) ? 'd' : '-');
    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c ", (mode & S_IXOTH) ? 'x' : '-');
}

/*===============================================================
[Program Name] : mylsl
[Description]  : 현재 디렉토리의 파일 및 디렉토리 정보를 
                 'ls -l' 명령어와 유사한 형식으로 출력하는 프로그램.
[Input]        : 현재 디렉토리의 파일 정보 (외부 입력 없음).
[Output]       : 각 파일 및 디렉토리의 권한, 링크 수, 소유자, 그룹, 
                 파일 크기, 수정 시간 및 파일 이름을 출력.
[Calls]        : 
                 - opendir(): 지정된 디렉토리를 열고 디렉토리 스트림을 반환.
                 - readdir(): 디렉토리 스트림에서 다음 엔트리를 읽어 반환.
                 - lstat(): 파일의 상태 정보를 가져와 stat 구조체에 채움.
                 - perror(): 오류 메시지를 출력하는 함수.
                 - getpwuid(): 사용자 ID에 해당하는 사용자 정보를 반환.
                 - getgrgid(): 그룹 ID에 해당하는 그룹 정보를 반환.
                 - strftime(): 시간을 지정된 형식으로 문자열로 변환.
                 - printf(): 형식화된 출력 함수.
                 - closedir(): 열린 디렉토리 스트림을 닫음.
[특기사항]     :
    - 프로그램 수행을 위해 현재 디렉토리에 대한 접근 권한이 있어야 함.
    - 해당 프로그램은 POSIX 호환 시스템에서 실행 가능.
    - <dirent.h>: 디렉토리 입출력 함수들 정의 (e.g., opendir, readdir).
    - <sys/stat.h>: 파일 상태 정보를 다루는 구조체와 매크로 정의 (e.g., lstat).
    - <pwd.h>: 사용자 정보 관련 함수와 구조체 정의 (e.g., getpwuid).
    - <grp.h>: 그룹 정보 관련 함수와 구조체 정의 (e.g., getgrgid).
    - <time.h>: 시간 처리 함수 및 구조체 정의 (e.g., strftime).
==================================================================*/
int main() {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    struct passwd *pwd;
    struct group *grp;
    char timebuf[80];

    // 현재 디렉토리 열기
    if ((dir = opendir(".")) == NULL) {
        perror("opendir");
        exit(1);
    }

    // 디렉토리 내 파일/디렉토리 정보 출력
    while ((entry = readdir(dir)) != NULL) {
        if (lstat(entry->d_name, &statbuf) == -1) {
            perror("lstat");
            continue;
        }

        // 파일 권한 및 타입 출력
        print_permissions(statbuf.st_mode);

        // 링크 수, 소유자, 그룹, 파일 크기
        printf("%2ld ", (long)statbuf.st_nlink);
        pwd = getpwuid(statbuf.st_uid);
        grp = getgrgid(statbuf.st_gid);
        printf("%-8s %-8s %8ld ", pwd->pw_name, grp->gr_name, (long)statbuf.st_size);

        // 수정 시간
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&statbuf.st_mtime));
        printf("%s ", timebuf);

        // 파일 이름
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}
