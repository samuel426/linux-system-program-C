#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define MY_ID                   34

#define SERV_TCP_PORT   (7000 + MY_ID)
#define SERV_HOST_ADDR  "127.0.0.1"

#define MSG_REQUEST             1
#define MSG_REPLY               2


