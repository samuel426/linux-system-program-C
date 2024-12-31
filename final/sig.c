#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>



void sig_handler(int sig) {
	
	if(sig == SIGUSR1) {
		exit(0);
	}
	else if(sig == SIGCHLD) {
		exit(0);
	}
	else {
		exit(0);
	}

}

int main() {
	
	pid_t pid;
	int count = 0;
	
	signal(SIGCHLD, sig_handler);
	signal(SIGUSR1, sig_handler);
	pid = fork();

	if(pid == 0){
		while(1) {
			count++;
		}
	} else {
		while(1) {
			sleep(1);
		}
	}

	return 0;
}

