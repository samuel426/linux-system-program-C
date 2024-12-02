/*===============================================================
[Program Name] : synclib.c
[Description]  : 
    - 부모와 자식 프로세스 간의 동기화를 위한 함수들을 구현.
    - 두 개의 파이프를 사용하여 부모와 자식 간 신호를 전달.
    - 부모와 자식이 교대로 실행되도록 제어.
[Input]        : 
    없음 (함수 호출로 동작 수행)
[Output]       : 
    부모와 자식 간 동기화를 제어하여 실행 순서를 보장.
[Calls]        : 
    - perror() : 에러 처리를 위한 메시지 출력
    - pipe(), read(), write() : 파이프를 통한 동기화 구현
[특기사항]     : 
    - 두 개의 정적 변수 Pfd1, Pfd2를 통해 부모-자식 간 신호 전달.
    - 각 함수는 호출 시 실패할 경우 프로그램을 종료하도록 설계됨.
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

// 정적 파이프 파일 디스크립터
static int Pfd1[2], Pfd2[2];

/*===============================================================
[Function Name] : void TELL_WAIT(void)
[Description]   : 부모와 자식 간 동기화를 위해 두 개의 파이프를 초기화.
[Input]         : 없음
[Output]        : 
    - Pfd1, Pfd2 파이프 초기화
[Call By]       : 
    - Main 함수에서 호출
[Calls]         : 
    - pipe() : 파이프 생성
    - perror() : 파이프 생성 실패 시 에러 메시지 출력
[Given]         : 없음
[Returns]       : 없음
==================================================================*/
void TELL_WAIT(void) {
    if (pipe(Pfd1) < 0 || pipe(Pfd2) < 0) {
        perror("pipe");
        exit(1);
    }
}

/*===============================================================
[Function Name] : void TELL_PARENT(void)
[Description]   : 자식 프로세스가 부모 프로세스에 신호를 보냄.
[Input]         : 없음
[Output]        : 
    - 부모 프로세스가 WAIT_PARENT() 호출 시 신호를 받을 수 있음
[Call By]       : 
    - 자식 프로세스의 코드에서 호출
[Calls]         : 
    - write() : 부모로 신호 전송
    - perror() : 실패 시 에러 메시지 출력
[Given]         : 
    - 정적 변수 Pfd2 사용
[Returns]       : 없음
==================================================================*/
void TELL_PARENT(void) {
    if (write(Pfd2[1], "c", 1) != 1) {
        perror("write");
        exit(1);
    }
}

/*===============================================================
[Function Name] : void WAIT_PARENT(void)
[Description]   : 자식 프로세스가 부모로부터 신호를 대기.
[Input]         : 없음
[Output]        : 
    - 부모로부터의 신호가 올 때까지 대기
[Call By]       : 
    - 자식 프로세스의 코드에서 호출
[Calls]         : 
    - read() : 부모로부터 신호 수신
    - perror() : 실패 시 에러 메시지 출력
[Given]         : 
    - 정적 변수 Pfd1 사용
[Returns]       : 없음
==================================================================*/
void WAIT_PARENT(void) {
    char c;

    if (read(Pfd1[0], &c, 1) != 1) {
        perror("read");
        exit(1);
    }
    if (c != 'p') {
        fprintf(stderr, "WAIT_PARENT: incorrect data");
        exit(1);
    }
}

/*===============================================================
[Function Name] : void TELL_CHILD(void)
[Description]   : 부모 프로세스가 자식 프로세스에 신호를 보냄.
[Input]         : 없음
[Output]        : 
    - 자식 프로세스가 WAIT_CHILD() 호출 시 신호를 받을 수 있음
[Call By]       : 
    - 부모 프로세스의 코드에서 호출
[Calls]         : 
    - write() : 자식으로 신호 전송
    - perror() : 실패 시 에러 메시지 출력
[Given]         : 
    - 정적 변수 Pfd1 사용
[Returns]       : 없음
==================================================================*/
void TELL_CHILD(void) {
    if (write(Pfd1[1], "p", 1) != 1) {
        perror("write");
        exit(1);
    }
}

/*===============================================================
[Function Name] : void WAIT_CHILD(void)
[Description]   : 부모 프로세스가 자식으로부터 신호를 대기.
[Input]         : 없음
[Output]        : 
    - 자식으로부터의 신호가 올 때까지 대기
[Call By]       : 
    - 부모 프로세스의 코드에서 호출
[Calls]         : 
    - read() : 자식으로부터 신호 수신
    - perror() : 실패 시 에러 메시지 출력
[Given]         : 
    - 정적 변수 Pfd2 사용
[Returns]       : 없음
==================================================================*/
void WAIT_CHILD(void) {
    char c;

    if (read(Pfd2[0], &c, 1) != 1) {
        perror("read");
        exit(1);
    }
    if (c != 'c') {
        fprintf(stderr, "WAIT_CHILD: incorrect data");
        exit(1);
    }
}

