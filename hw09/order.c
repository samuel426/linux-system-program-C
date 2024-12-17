/*===============================================================
[Program Name] : order.c
[Description]  : 
    - 정수 변수 a의 주소를 바이트 단위로 읽어 출력하여 시스템의 엔디언(Endianness)을 확인한다.
[Input]        : 
    없음
[Output]       : 
    - 변수 a의 각 바이트 값과 해당 주소를 출력
    - Address <주소>: <값> 형식으로 출력
[Calls]        : 
    없음
[특기사항]     : 
    - 변수 a의 값을 0x12345678로 초기화하여 실험
    - char 포인터 p를 이용해 바이트 단위로 값을 출력
    - 엔디언 방식을 확인하기 위한 코드
==================================================================*/

#include <stdio.h>

int main()
{
    int a = 0x12345678; // 정수 변수 a를 0x12345678로 초기화
    char *p = (char *)&a; // a의 주소를 char 포인터로 캐스팅하여 p에 저장

#if 1
    // 주소와 값을 순차적으로 출력하며 포인터를 증가시킴
    printf("Address %p: %#x\n", p, *p); p++;
    printf("Address %p: %#x\n", p, *p); p++;
    printf("Address %p: %#x\n", p, *p); p++;
    printf("Address %p: %#x\n", p, *p); p++;
#else
    // 포인터를 증가시키면서 주소와 값을 출력
    printf("Address %p: %#x\n", p, *p++);
    printf("Address %p: %#x\n", p, *p++);
    printf("Address %p: %#x\n", p, *p++);
    printf("Address %p: %#x\n", p, *p++);
#endif
}

