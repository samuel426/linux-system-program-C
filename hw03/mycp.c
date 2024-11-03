#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUF 1024
/*===============================================================
[Program Name] : mycp.c
[Description]  : mycp.c를 mycp.bak으로 복사하는 프로그햄
[Input]        : 복사할 파일, 붙여넣기할 파일
[Output]       : mycp.bak이 생성되고, 안에는 복사할 파일의 내용이 들어가 있다.
[Calls]        : open() // 입력받은 파일을 연다.
				O_RDONLY -> 읽기 전용으로 열기
				O_WRONLY -> 쓰기 전용으로 열기
				O_CREAT -> 입력받은 이름의 파일 없으면 새로 만들기
			        O_TRUNC -> 다 지워버리고 처음부터 시작
			   뒤에는 권한 설정	
                 perror() // 오류 났을때 메시지를 출력하는 함수
                 exit() // 프로그램 나가기
		 read() // 파일 작성 첫번째 인수는 복사할 파일을 가르키고,
		 		두번째는 무엇을 복사할 것인지,
                                세번째는 1024 바이트 만큼 읽는다는 뜻
				read가 반환하는 건 
                 write() // 파일 작성 첫번째 인수는 붙여놓을 파일을 가르키고,
		 		두번째는 무엇을 넣을 것인지,
                                세번째는 count의 바이트 만큼 쓴다는 뜻 
                 close() // 파일 닫기, 메모리 누수 방지
[특기사항]     : input을 제대로 안 넣어주면 오류남 

==================================================================*/
main(int argc, char *argv[])
{
	int fd1, fd2, count;
	char buf[MAX_BUF];

	if(argc != 3) {
		printf("Usage: %s source destination\n", argv[0]);
	}

	if((fd1 = open(argv[1], O_RDONLY)) < 0) {
		perror("open");
		exit(1);
	}

	if((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
		perror("open");
		exit(1);
	}

	while (count = read(fd1, buf, MAX_BUF)) {
		write(fd2, buf, count);
	}

	close(fd1);
	close(fd2);
}
