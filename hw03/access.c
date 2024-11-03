#include <stdio.h>
#include <unistd.h>
#include "record.h"
#include <stdlib.h>
#include <string.h>
/*===============================================================
[Program Name] : access.c
[Description]  : 구조체를 이용하여 입력한 text파일을 출력하고, 
		 구조의 순서를 섞고 
		 Record list를 추가하는 프로그램
[Input]        : ./access test.out
[Output]       : 수정된 test.out text파일
[Calls]        : fprintf() // stderr 시 터미널에 출력하는 것
                 fopen() // 1. 파일 이름, 2. 어떻게 열 것인가 ("rb"는 binary 읽기, "wb"는 binary 쓰기)
                 perror() // 오류 났을때 메시지를 출력하는 함수
                 exit() // 프로그램 나가기
                 fread() // 파일의 문자열을 읽어 buf에 저장하는 함수,
                                읽을 데이터의 크기 char는 1byte이기 때문에 1
                                MAX_BUF은 한번에 얼마까지 적을수 있는지
                                src는 읽고자 하는 파일
                 fwrite() // 받은 문자열 buf를 파일 dst에 넣는 함수
                                count가 0 또는 1이기 때문에 하나씩 쓰게 된다.
                 fclose() // 파일 닫기, 메모리 누수 방지
                 fseek() // 파일 포인터를 특정 위치로 옮김
                                SEEK_END = 파일 끝으로 이동, 0L은 파일 끝의 오프셋
                                SEEK_SET = 파일의 시작점으로 이동, 이후 cur 만큼 이동함
		 puts() // 입력받은 구조체 rp를 요소별로 접근해서 출력하는 함수
		 strcpy() // 첫번째 입력하는 곳에, 두번째 string에 들어온 것을 복사하는 것
		 rewind() // 읽을 위치를 가르키는 포인터를 처음으로 돌려놓음
		 getchar() // 문자, 문자열을 받는 함수
[특기사항]     : input을 제대로 안 넣어주면 오류남
                 exit() 함수를 사용하기 위해 <stdlib.h>를 가져왔음
                 <stdlib.h> 파일 입출력, 프로세스 제어, 시스템 정보 등을 제공하는 다양한 함수가 있다.
                 여기선 해당 함수가 따로 쓰이진 않았음
		 <string.h> strcpy 때문에 추가된 헤더파일
		 Record 구조체를 담고 있는 record.h 헤더파일 사용 
==================================================================*/
void print_record(Record *rp)
{
	puts(rp->stud);
	puts(rp->num);
	puts(rp->dept);
}

main(int argc, char *argv[])
{
	FILE *fp;
	Record rec;

	if(argc != 2) {
		fprintf(stderr, "Usage: %s file\n", argv[0]);
		exit(1);
	}

	if ((fp = fopen(argv[1], "r+b")) == NULL) {
		perror("fopen");
		exit(1);
	}

	printf("-----Ordered Record List-----\n");

	while(fread(&rec, sizeof(rec), 1, fp)) {
		print_record(&rec);
	}
	rewind(fp);
	getchar();

	printf("-----Shuffled Record List (3, 6, 2, 4, 1, 5)-----\n");

	fseek(fp, sizeof(rec)*2L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);

	fseek(fp, sizeof(rec)*5L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);

	fseek(fp, sizeof(rec)*1L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);

	fseek(fp, sizeof(rec)*3L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);

	fseek(fp, sizeof(rec)*4L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	print_record(&rec);
	getchar();

	printf("-----Update Record List-----\n");

	fseek(fp, sizeof(rec)*2L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	strcpy(rec.dept, "Movie");
	fseek(fp, sizeof(rec)*5L, SEEK_SET);
	fwrite(&rec, sizeof(rec), 1, fp);

	fseek(fp, sizeof(rec)*5L, SEEK_SET);
	fread(&rec, sizeof(rec), 1, fp);
	strcpy(rec.dept, "Baseball");
	fseek(fp, sizeof(rec)*5L, SEEK_SET);
	fwrite(&rec, sizeof(rec), 1, fp);

	rewind(fp);
	while (fread(&rec, sizeof(rec), 1, fp)) {
		print_record(&rec);
	}

	fclose(fp);
}
