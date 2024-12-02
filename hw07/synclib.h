/*===============================================================
[Program Name] : synclib.h
[Description]  : 
    - 부모와 자식 프로세스 간의 동기화를 위한 함수 선언을 포함.
    - 부모와 자식이 교대로 실행될 수 있도록 제어 신호를 전달하는 메커니즘 제공.
[Input]        : 
    없음 (동기화를 수행하기 위한 함수 선언만 포함)
[Output]       : 
    없음 (동기화를 위한 기능 제공)
[Calls]        : 
    - TELL_WAIT() : 초기화 작업 수행
    - TELL_PARENT() : 부모 프로세스에 신호 전달
    - WAIT_PARENT() : 부모의 신호 대기
    - TELL_CHILD() : 자식 프로세스에 신호 전달
    - WAIT_CHILD() : 자식의 신호 대기
[특기사항]     : 
    - 이 헤더 파일은 `synclib.c`와 함께 사용되며, 함수 정의는 해당 소스 파일에 포함.
    - 동기화 함수는 `unistd.h` 및 `sys/types.h`를 기반으로 구현됨.
==================================================================*/

#include <sys/types.h>
#include <unistd.h>

// 부모와 자식 프로세스 간의 동기화를 위한 함수 선언
void TELL_WAIT(void);       // 초기화 함수
void TELL_PARENT(void);     // 부모 프로세스에 신호 전달
void WAIT_PARENT(void);     // 부모의 신호 대기
void TELL_CHILD(void);      // 자식 프로세스에 신호 전달
void WAIT_CHILD(void);      // 자식의 신호 대기

