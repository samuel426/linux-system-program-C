/*===============================================================
[Program Name] : sipc1.c
[Description]  :
    - 공유 메모리와 POSIX 뮤텍스 및 조건 변수를 사용하여 클라이언트 요청을 처리하고 응답을 생성.
    - 멀티 프로세스 간 통신을 지원하며, SIGINT 신호를 처리하여 안전한 종료 구현.
[Input]        :
    - 공유 메모리에서 클라이언트 요청 메시지를 수신.
[Output]       :
    - 공유 메모리에 응답 메시지를 저장.
    - 요청 및 응답 내용을 콘솔에 출력.
[Calls]        :
    - shm_open(), mmap(), pthread_mutex_lock(), pthread_cond_wait(), 
      pthread_cond_signal(), signal(), snprintf(), munmap(), shm_unlink()
[특기사항]     :
    - 공유 메모리는 `shm_open`과 `mmap`을 사용하여 생성.
    - POSIX 뮤텍스와 조건 변수를 사용하여 동기화.
==================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
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

int running = 1; // 서버 실행 상태 플래그

// 전역 변수로 sharedData 선언
SharedData *sharedData; 

// SIGINT 신호 핸들러
void handle_sigint(int sig) {
    printf("\nServer: Received SIGINT. Shutting down...\n");
    running = 0;

    // 조건 변수 신호를 보내서 대기 중인 스레드 해제
    pthread_mutex_lock(&sharedData->mutex);
    pthread_cond_signal(&sharedData->cond);
    pthread_mutex_unlock(&sharedData->mutex);
}

// 서버 스레드
void *serverThread(void *arg) {
    while (running) {
        pthread_mutex_lock(&sharedData->mutex);

        // 요청 대기
        while (!sharedData->hasRequest && running) {
            pthread_cond_wait(&sharedData->cond, &sharedData->mutex);
        }
        if (!running) {
            pthread_mutex_unlock(&sharedData->mutex);
            break;
        }

        printf("Server: Received request: %s\n", sharedData->message);

        // 응답 작성
        snprintf(sharedData->message, BUFFER_SIZE, "Reply from server: %d", getpid());
        sharedData->hasRequest = 0;

        pthread_cond_signal(&sharedData->cond);
        pthread_mutex_unlock(&sharedData->mutex);
    }

    return NULL;
}

int main() {
    int shm_fd; // 공유 메모리 파일 디스크립터

    // SIGINT 핸들러 설정
    signal(SIGINT, handle_sigint);

    // 공유 메모리 생성 및 초기화
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    ftruncate(shm_fd, sizeof(SharedData));

    sharedData = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (sharedData == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 공유 메모리 초기화
    pthread_mutexattr_t mutexAttr;
    pthread_condattr_t condAttr;

    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&sharedData->mutex, &mutexAttr);

    pthread_condattr_init(&condAttr);
    pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&sharedData->cond, &condAttr);

    sharedData->hasRequest = 0;

    pthread_t tid;
    pthread_create(&tid, NULL, serverThread, (void *)sharedData);

    // 서버 종료 대기
    pthread_join(tid, NULL);

    // 리소스 정리
    pthread_mutex_destroy(&sharedData->mutex);
    pthread_cond_destroy(&sharedData->cond);
    munmap(sharedData, sizeof(SharedData));
    shm_unlink(SHM_NAME);

    printf("Server: Resources cleaned up.\n");
    return 0;
}

