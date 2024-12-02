/*===============================================================
[Program Name] : sync.c
[Description]  : 
    - 부모와 자식 프로세스 간의 동기화를 구현하여 교대로 메시지를 출력하는 프로그램.
    - 동기화를 위해 사용자 정의 함수(TELL_WAIT, TELL_CHILD, WAIT_CHILD 등)를 사용.
[Input]        : 
    int NLOOPS; // 부모와 자식 간의 동기화를 반복하는 횟수
[Output]       : 
    - 부모와 자식 프로세스의 메시지 출력
    - 동기화 상태를 확인할 수 있는 로그 출력
[Calls]        : 
    - TELL_WAIT(), TELL_CHILD(), TELL_PARENT(), WAIT_CHILD(), WAIT_PARENT()
    - fork(), perror(), exit()
[특기사항]     : 
    - 동기화는 "synclib.h"에 정의된 함수들을 통해 구현.
    - 자식 프로세스 생성은 fork()를 사용.
    - 부모와 자식은 지정된 횟수(NLOOPS)만큼 교대로 동작.
==================================================================*/

#include <stdio.h>
#include "synclib.h"

#define NLOOPS  5

int main() {
    int i;
    pid_t pid;

    TELL_WAIT(); // 동기화를 위한 초기화
    if ((pid = fork()) < 0) { // 자식 프로세스 생성
        perror("fork");
        exit(1);
    } else if (pid > 0) { // 부모 프로세스
        for (i = 0; i < NLOOPS; i++) {
            TELL_CHILD(); // 자식에게 신호 전달
            printf("Parent: Tell to child\n");
            printf("Parent: Wait for child to tell\n");
            WAIT_CHILD(); // 자식의 신호를 대기
        }
    } else { // 자식 프로세스
        for (i = 0; i < NLOOPS; i++) {
            printf("Child: Wait for parent to tell\n");
            WAIT_PARENT(); // 부모의 신호를 대기
            TELL_PARENT(); // 부모에게 신호 전달
            printf("Child: Tell to parent\n");
        }
    }
}

