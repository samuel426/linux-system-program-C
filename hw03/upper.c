#include <stdio.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : upper.c
[Description]  : 입력한 testupper txt 파일의 소문자를 대문자로 변환하는 프로그햄
[Input]        : ./upper testupper
[Output]       : tcp2.bak 파일
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 fopen() // 1. 파일 이름, 2. 어떻게 열 것인가 ("rt"는 text 읽기, "wt"는 text 쓰기)
                 perror() // 오류 났을때 메시지를 출력하는 함수
                 exit() // 프로그램 나가기
                 fgets() // 파일의 문자열을 읽어 buf에 저장하는 함수,
                                MAX_BUF은 한번에 얼마까지 적을수 있는지
                                != EOF는 더이상 없는지 판별 여부
                 fclose() // 파일 닫기, 메모리 누수 방지
[특기사항]     : input을 제대로 안 넣어주면 오류남
                 exit() 함수를 사용하기 위해 <stdlib.h>를 가져왔음
		 to_uppercase // 문자열을 받아 ASCII 값 차이를 이용해 소문자를 대문자로 변환하는 함수
==================================================================*/
void to_uppercase(char *str) {
    while (*str) {
        if (*str >= 'a' && *str <= 'z') {
            *str = *str - ('a' - 'A');
        }
        str++;
    }
}

int main(int argc, char *argv[]) {
    FILE *src;
    char ch;
    char buffer[1024];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }

    if ((src = fopen(argv[1], "r")) == NULL) {
        perror("fopen");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), src)) {
        to_uppercase(buffer);
        printf("%s", buffer);
    }

    fclose(src); 
    return 0;
}

