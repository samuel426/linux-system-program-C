/*===============================================================
[Program Name] : mycp3.c
[Description]  : 
    - 메모리 매핑을 사용하여 파일의 내용을 복사.
    - 소스 파일을 메모리에 매핑하고, 이를 대상 파일의 메모리에 복사.
[Input]        : 
    - argc, argv: 소스 파일(argv[1])과 대상 파일(argv[2]) 경로.
[Output]       : 
    - 대상 파일에 소스 파일의 내용을 복사.
    - 프로그램 종료 후 복사된 파일 생성.
[Calls]        : 
    - open(), fstat(), lseek(), mmap(), memcpy(), close(), perror(), exit()
[특기사항]     : 
    - 메모리 매핑을 사용하여 효율적으로 파일 복사.
    - 대상 파일이 없는 경우 생성(O_CREAT)하며, 기존 파일은 덮어씀(O_TRUNC).
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int fdin, fdout;           // 소스 및 대상 파일 디스크립터
    char *src, *dst;           // 메모리 매핑 포인터
    struct stat statbuf;       // 파일 상태 정보 저장 구조체

    // 입력 인자 확인
    if (argc != 3) {
        fprintf(stderr, "Usage: %s source destination\n", argv[0]);
        exit(1);
    }

    // 소스 파일 열기
    if ((fdin = open(argv[1], O_RDONLY)) < 0) {
        perror("open"); // 소스 파일 열기 실패 시 에러 출력
        exit(1);
    }

    // 대상 파일 열기(없으면 생성)
    if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
        perror("open"); // 대상 파일 열기 실패 시 에러 출력
        exit(1);
    }

    // 소스 파일 상태 정보 가져오기
    if (fstat(fdin, &statbuf) < 0) {
        perror("fstat"); // 파일 상태 가져오기 실패 시 에러 출력
        exit(1);
    }

    // 대상 파일 크기 설정
    if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) < 0) {
        perror("lseek"); // 파일 크기 설정 실패 시 에러 출력
        exit(1);
    }
    write(fdout, "", 1); // 대상 파일 크기를 소스 파일과 동일하게 만듦

    // 소스 파일을 메모리에 매핑
    if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == (caddr_t)-1) {
        perror("mmap"); // 메모리 매핑 실패 시 에러 출력
        exit(1);
    }

    // 대상 파일을 메모리에 매핑
    if ((dst = mmap(0, statbuf.st_size, PROT_WRITE, MAP_SHARED, fdout, 0)) == (caddr_t)-1) {
        perror("mmap"); // 메모리 매핑 실패 시 에러 출력
        exit(1);
    }

    // 소스 파일 내용을 대상 파일로 복사
    memcpy(dst, src, statbuf.st_size);

    // 파일 디스크립터 닫기
    close(fdin);
    close(fdout);

    return 0;
}

