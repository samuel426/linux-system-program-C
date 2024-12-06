/*===============================================================
[Program Name] : sipc2.c
[Description]  :
    - 공유 메모리와 POSIX 뮤텍스 및 조건 변수를 사용하여 서버로 요청을 보내고 응답을 수신.
    - 멀티 프로세스 간 통신을 지원하며 요청 및 응답 내용을 출력.
[Input]        :
    - 클라이언트 요청 메시지 작성.
[Output]       :
    - 서버 응답 메시지를 공유 메모리에서 읽어와 출력.
[Calls]        :
    - shm_open(), mmap(), pthread_mutex_lock(), pthread_cond_wait(),
      pthread_cond_signal(), snprintf(), munmap()
[특기사항]     :
    - 공유 메모리는 `shm_open`과 `mmap`을 사용하여 연결.
    - POSIX 뮤텍스와 조건 변수를 사용하여 동기화.
==================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define SHM_NAME "/shared_memory"

typedef struct {
    pthread_mutex_t mutex;       // 뮤텍스
    pthread_cond_t cond;         // 조건 변수
    char message[BUFFER_SIZE];   // 메시지 버퍼
    int hasRequest;              // 요청 상태 플래그
} SharedData;

int main() {
    int shm_fd; // 공유 메모리 파일 디스크립터
    SharedData *sharedData; // 공유 메모리 데이터 구조체 포인터

    // 공유 메모리 연결
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    sharedData = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (sharedData == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 요청 작성
    pthread_mutex_lock(&sharedData->mutex);

    snprintf(sharedData->message, BUFFER_SIZE, "Request from client: %d", getpid());
    sharedData->hasRequest = 1;

    printf("Client: Sent request: %s\n", sharedData->message);

    // 요청 신호
    pthread_cond_signal(&sharedData->cond);

    // 응답 대기
    while (sharedData->hasRequest) {
        pthread_cond_wait(&sharedData->cond, &sharedData->mutex);
    }

    printf("Client: Received reply: %s\n", sharedData->message);

    pthread_mutex_unlock(&sharedData->mutex);

    // 공유 메모리 해제
    munmap(sharedData, sizeof(SharedData));
    return 0;
}

