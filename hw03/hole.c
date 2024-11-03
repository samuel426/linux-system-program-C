#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/*===============================================================
[Program Name] : hole.c
[Description]  : 1024 byte 짜리 0으로 채워진 파일 생성 프로그램
[Input]        : Nothing
[Output]       : 
		-r-------- 1 lsp34 lsp 1024 Oct 15 13:15 file.hole
		file.hole안에는 0이 채워져 있다.
[Calls]        : creat() // file.hole 이라는 파일 생성, 두번째 입력은 권한
		 perror() // 오류 났을때 메시지를 출력하는 함수
		 exit() // 프로그램 나가기
		 write() // 파일 작성 첫번째 인수는 file.hole을 가르키고, 
		 		두번째는 무엇을 넣을 것인지, 
				세번째는 1023바이트까지 간 거에서 1바이트만 작성하는 것 
		 close() // 파일 닫기, 메모리 누수 방지

==================================================================*/

main()
{
	int fd;
	char ch = '\0';

	if ((fd =creat("file.hole", 0400)) < 0) {
		perror("creat");
		exit(1);
	}

	if(lseek(fd, 1023, SEEK_SET) < 0) {
		perror("lseek");
		exit(1);
	}
	write(fd, &ch, 1);

	close(fd);
}
