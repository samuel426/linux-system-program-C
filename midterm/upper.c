#include <stdio.h>
#include <stdlib.h>

void to_uppercase(char *str) {
    while (*str) {
        if (*str >= 'a' && *str <= 'z') {
            *str = *str - ('a' - 'A');
        }
        str++;
    }
}

int main(int argc, char *argv[]) {
	FILE *src, *dst;
	int ch;

        if (argc != 3)  {
                fprintf(stderr, "Usage: %s source destination\n", argv[0]);
                exit(1);
        }

        if ((src = fopen(argv[1], "rt")) == NULL)  {
                perror("fopen");
                exit(1);
        }

        if ((dst = fopen(argv[2], "wt")) == NULL)  {
                perror("fopen");
                exit(1);
        }

        while ((ch = fgetc(src)) != EOF) {
		to_uppercase(ch);
                fputc(ch, dst);
        }



    fclose(src);
    fclose(dst);
    return 0;
}
