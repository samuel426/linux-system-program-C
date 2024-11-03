#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : tab.c
[Description]  : 파일을 읽어 tab을 4칸 띄어쓰기로 바꾸는 프로그램
[Input]        : ./tab *.c
[Output]       : 수정된 *.c파일
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 fopen() // 1. 파일 이름, 2. 어떻게 열 것인가 ("rt"는 text 읽기, "wt"는 text 쓰기)
                 perror() // 오류 났을때 메시지를 출력하는 함수
                 fclose() // 파일 닫기, 메모리 누수 방지
                 strcpy() // 첫번째 입력하는 곳에, 두번째 string에 들어온 것을 복사하는 것
		 fputc() // 주어진 문자를 지정된 파일 스트림에 기록, 반환값은 기록한 문자의 값
		 fgetc() // 지정된 파일 스트림에서 다음 문자를 읽어 반환, 파일의 끝에 도달하면 EOF를 반환
[특기사항]     : input을 제대로 안 넣어주면 오류남
                 exit() 함수를 사용하기 위해 <stdlib.h>를 가져왔음
                 <string.h> strcpy, strcat 때문에 추가된 헤더파일
==================================================================*/

enum { FALSE, TRUE };

void
conv(char *fname)
{
	FILE	*src, *dst;
	char	fdest[40];
	int		ch, first;

	if ((src = fopen(fname, "rt")) == NULL)  {
		perror("fopen");
		return;
	}

	strcpy(fdest, fname);
	strcat(fdest, ".t");
	if ((dst = fopen(fdest, "wt")) == NULL)  {
		perror("fopen");
		return;
	}

	first = TRUE;
	while ((ch = fgetc(src)) != EOF)  {
		if (first && ch == '\t')  {
			fputc(' ', dst);
			fputc(' ', dst);
			fputc(' ', dst);
			fputc(' ', dst);
		}
		else  {
			fputc(ch, dst);
			if (first)
				first = FALSE;
			if (ch == '\n')
				first = TRUE;
		}
	}

	fclose(src);
	fclose(dst);
}

main(int argc, char *argv[])
{
	while (--argc)  {
		conv(argv[argc]);
	}
}
