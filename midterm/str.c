#include <stdio.h>

void strupper(char *str) {
	while (*str != '\\0') {
		*str = toupper(*str);
		*str++;
	}
}

int strcasecmp(char *dst, char *src) {

	while(*dst != '\0' || *src != '\0') {

		if(*dst < *src) {
			return -1;
		}
		else if (*dst > *src) {
			return 1;
		}
		else {
			*dst++;
			*src++;
		}
	}

	
	return 0;
}

int main(int argc, char *argv[]) {
	
	char str1[20], str2[20];
	
	str1 = argv[1];
	str2 = argv[2];
	
	strupper(str1);
	strupper(str2);

	printf("strcasecmp = %d", strcasecmp(str1, str2));
}
