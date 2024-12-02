/*===============================================================
[Program Name] : fifos.c
[Description]  : 
    - FIFO(named pipe)를 사용하여 클라이언트와 서버 간의 통신 구현.
    - 서버는 클라이언트로부터 메시지를 받고, 응답 메시지를 FIFO를 통해 전송.
[Input]        : 
    MsgType msg; // 클라이언트로부터 전달된 메시지 구조체
[Output]       : 
    - 클라이언트로 응답 메시지를 전송
    - 수신 및 전송된 메시지를 콘솔에 출력
[Calls]        : 
    - mkfifo(), open(), read(), write(), close(), signal(), remove()
[특기사항]     : 
    - 서버 FIFO 이름은 `SERV_FIFO`(`./.fifo`)로 정의.
    - SIGINT 시그널 핸들러를 등록하여 종료 시 서버 FIFO를 삭제.
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "fifo.h"
#include <stdlib.h>
#include <string.h>

/*===============================================================
[Function Name] : void SigIntHandler(int signo)
[Description]   : 
    - SIGINT 시그널을 처리하여 서버 FIFO 파일을 삭제하고 종료.
[Input]         : 
    int signo; // 시그널 번호
[Output]        : 
    - 서버 FIFO 파일이 삭제되고 프로그램이 종료
[Call By]       : 
    - SIGINT 시 시스템에 의해 호출
[Calls]         : 
    - remove(), perror(), exit()
[Given]         : 
    - SERV_FIFO: FIFO 파일 이름
[Returns]       : 없음
==================================================================*/
void SigIntHandler(int signo) {
    if (remove(SERV_FIFO) < 0) {
        perror("remove");
        exit(1);
    }

    exit(0);
}

int main() {
    int fd, cfd, n;
    MsgType msg;

    // SIGINT 시그널 핸들러 등록
    if (signal(SIGINT, SigIntHandler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    // 서버 FIFO 생성
    if (mkfifo(SERV_FIFO, 0600) < 0) {
        if (errno != EEXIST) {
            perror("mkfifo");
            exit(1);
        }
    }

    // 서버 FIFO 열기
    if ((fd = open(SERV_FIFO, O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    // 클라이언트 요청 처리 루프
    while (1) {
        // 서버 FIFO에서 메시지 읽기
        if ((n = read(fd, (char *)&msg, sizeof(msg))) < 0) {
            if (errno == EINTR) {
                continue; // 시그널로 인한 중단은 무시
            } else {
                perror("read");
                exit(1);
            }
        }
        printf("Received request: %s.....", msg.data);

        // 클라이언트 FIFO 열기
        if ((cfd = open(msg.returnFifo, O_WRONLY)) < 0) {
            perror("open");
            exit(1);
        }

        // 응답 메시지 생성 및 전송
        sprintf(msg.data, "This is a reply from %d.", getpid());
        write(cfd, (char *)&msg, sizeof(msg));
        close(cfd);
        printf("Replied.\n");
    }
}

