/*===============================================================
[Program Name] : pipe.c
[Description]  : 
    - 부모와 자식 프로세스 간의 양방향 통신을 두 개의 파이프를 사용하여 구현
    - 부모는 자식에게 메시지를 보내고, 자식은 응답 메시지를 부모에게 다시 전송
[Input]        : 
    char buf[MAX_BUF]; // 프로세스 간 데이터를 저장하기 위한 버퍼
[Output]       : 
    프로세스 간 통신 내용을 출력하여 결과를 콘솔에 보여줌
[Calls]        : 
    - perror() : 에러 처리를 위한 출력
    - fork() : 자식 프로세스 생성
    - read(), write() : 파이프를 통한 데이터 전송 및 수신
[특기사항]     : 
    - 부모에서 자식으로의 통신을 위한 pipe1과 자식에서 부모로의 통신을 위한 pipe2 사용
    - 파이프의 사용하지 않는 엔드를 닫아야 deadlock 및 에러를 방지할 수 있음
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 128

int main() {
    int pipe1[2], pipe2[2]; // pipe1: 부모 → 자식 통신, pipe2: 자식 → 부모 통신
    pid_t pid;
    char buf[MAX_BUF];

    // 부모 → 자식 통신을 위한 첫 번째 파이프 생성
    if (pipe(pipe1) < 0) {
        perror("pipe1");
        exit(1);
    }

    // 자식 → 부모 통신을 위한 두 번째 파이프 생성
    if (pipe(pipe2) < 0) {
        perror("pipe2");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // 자식 프로세스
        close(pipe1[1]); // pipe1의 쓰기 엔드 닫기
        close(pipe2[0]); // pipe2의 읽기 엔드 닫기

        // 부모로부터 메시지 읽기
        if (read(pipe1[0], buf, MAX_BUF) < 0) {
            perror("read");
            exit(1);
        }
        printf("Child: Received data from parent: %s", buf);

        // 부모에게 응답 메시지 전송
        strcpy(buf, "Message received by child!\n");
        if (write(pipe2[1], buf, strlen(buf) + 1) < 0) {
            perror("write");
            exit(1);
        }
        printf("Child: Sent response to parent.\n");

        close(pipe1[0]);
        close(pipe2[1]);
    } else { // 부모 프로세스
        close(pipe1[0]); // pipe1의 읽기 엔드 닫기
        close(pipe2[1]); // pipe2의 쓰기 엔드 닫기

        // 자식에게 메시지 전송
        strcpy(buf, "Hello, Child!\n");
        printf("Parent: Sending data to child.\n");
        if (write(pipe1[1], buf, strlen(buf) + 1) < 0) {
            perror("write");
            exit(1);
        }

        // 자식으로부터 응답 읽기
        if (read(pipe2[0], buf, MAX_BUF) < 0) {
            perror("read");
            exit(1);
        }
        printf("Parent: Received response from child: %s", buf);

        close(pipe1[1]);
        close(pipe2[0]);
    }

    return 0;
}

