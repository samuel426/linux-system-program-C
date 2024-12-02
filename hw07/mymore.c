/*===============================================================
[Program Name] : mymore.c
[Description]  : 
    - 이 프로그램은 `pipe`를 사용하여 `ls -l` 명령어 출력 결과를 `more` 명령어로 출력.
    - 부모 프로세스와 자식 프로세스를 사용해 데이터를 파이프로 연결.
[Input]        : 
    없음 (프로그램 실행 시 명령어 실행을 통해 동작)
[Output]       : 
    - `ls -l` 명령의 출력이 `more` 명령으로 전달되어 페이지 단위로 출력.
[Calls]        : 
    - pipe(), fork(), dup(), dup2(), execlp(), perror(), exit()
[특기사항]     : 
    - `pipe`를 통해 자식에서 부모로 데이터 전달.
    - 자식 프로세스는 `ls -l` 명령 실행, 부모는 `more` 명령 실행.
==================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fd[2];       // 파이프 파일 디스크립터 배열
    pid_t child;     // 자식 프로세스 ID

    // 파이프 생성
    if (pipe(fd) == -1) {
        perror("pipe"); // 파이프 생성 실패 시 에러 출력
        exit(1);
    }

    // 자식 프로세스 생성
    child = fork();
    if (child == (pid_t)(-1)) {
        perror("fork"); // fork 실패 시 에러 출력
        exit(1);
    } else if (child == (pid_t)0) { // 자식 프로세스
        close(1);        // 표준 출력 닫기
        close(fd[0]);    // 파이프의 읽기 엔드 닫기

        // 파이프 쓰기 엔드를 표준 출력으로 연결
        if (dup(fd[1]) == -1) {
            perror("dup");
            exit(1);
        }

        // `ls -l` 명령 실행
        if ((execlp("ls", "ls", "-l", NULL)) == -1) {
            perror("execlp"); // 명령 실행 실패 시 에러 출력
            exit(1);
        }
    } else { // 부모 프로세스
        close(0);        // 표준 입력 닫기
        close(fd[1]);    // 파이프의 쓰기 엔드 닫기

        // 파이프 읽기 엔드를 표준 입력으로 연결
        if (dup2(fd[0], 0) == -1) {
            perror("dup2");
            exit(1);
        }

        // `more` 명령 실행
        if ((execlp("more", "more", NULL)) == -1) {
            perror("execlp"); // 명령 실행 실패 시 에러 출력
            exit(1);
        }
    }

    return 0;
}

