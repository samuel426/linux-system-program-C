#include <stdio.h>

/*===============================================================
[Function Name] : int strlen_p(char *str)
[Description]   : 
	- 문자열 길이 읽는 함수, 포인터로 접근
	- char 자료형은 1바이트 이므로 1씩 커지면 다음 메모리공간에 접근 가능
[Input]         : char *str
[Output]        : int len
[Call By]       : main()
[Calls]         : Nothing
[Given]         : Nothing
[Returns]       : Nothing
==================================================================*/
int strlen_p(char *str)
{
	int	len = 0;

	while (*(str + len))  {
		len++;
	}
	return len;
}

/*===============================================================
[Function Name] : int strlen_a(char str[])
[Description]   :
        - 문자열 길이 읽는 함수, 배열로 접근
        - char str[i]의 i가 1씩 커지면 다음 메모리공간에 접근 가능
	- for 문으로 '\0'이 안나오는 배열의 끝까지 접근
[Input]         : char str[]
[Output]        : int i
[Call By]       : main()
[Calls]         : Nothing
[Given]         : Nothing
[Returns]       : Nothing
==================================================================*/
int strlen_a(char str[])
{
	int i;

	for (i = 0 ; str[i] != '\0' ; i++)
		;
	return i;
}

/*===============================================================
[Function Name] : void strcpy_p(char *dst, char *src)
[Description]   :
        - 문자를 넣고 싶은 *dst, 넣을 문자 *src를 받아옴
        - while문으로 *src가 아무것도 안 나올 때까지 
        - 포인터를 옮겨주면서 *dst에 복사함
	- 마지막의 '\0'까지 복사하면서 함수 끝
[Input]         : char *dst, char *src
[Output]        : Nothing
[Call By]       : main()
[Calls]         : Nothing
[Given]         : Nothing
[Returns]       : Nothing
==================================================================*/

void strcpy_p(char *dst, char *src)
{
	while (*src)  {
		*dst++ = *src++;
	}
	*dst = *src;
}
/*===============================================================
[Function Name] : void strcpy_a(char dst[], char src[])
[Description]   :
        - 문자를 넣고 싶은 dst[], 넣을 문자 src[]를 받아옴
        - for문으로 src[]가 아무것도 안 나올 때까지
        - i를 증가하면서 dst[]에 복사함
        - 마지막의 '\0'까지 복사하면서 함수 끝
[Input]         : char dst[], char src[]
[Output]        : Nothing
[Call By]       : main()
[Calls]         : Nothing
[Given]         : Nothing
[Returns]       : Nothing
==================================================================*/
void strcpy_a(char dst[], char src[])
{
	int i;

	for (i = 0 ; src[i] != '\0' ; i++)
		dst[i] = src[i];
	dst[i] = src[i];
}

/*===============================================================
[Function Name] : void strcat_p(char *dst, char *src)
[Description]   :
        - 문자열 *dst와 *dst 뒤에 추가할 문자열 *src를 받는다
        - while 문을 이용하여 먼저 *dst가 가르키는 곳을 *dst의 문자열 끝으로 옮긴다.
        - dst-- 하는 이유는 첫번째 while문을 벗어나며 한칸을 더 갔기 때문
        - *src의 크기 만큼 while문을 돌리면서, *src의 내용을 *dst에 복사
[Input]         : char *dst, char *src
[Output]        : Nothing
[Call By]       : main()
[Calls]         : Nothing
[Given]         : Nothing
[Returns]       : Nothing
==================================================================*/

void strcat_p(char *dst, char *src)
{
	while (*dst++)
			;
	dst--;	
	while (*src)  {
			*dst++ = *src++;
	}
	*dst = *src;
}

/*===============================================================
[Function Name] : void strcat_a(char dst[], char src[])
[Description]   :
        - for문과 i를 이용하여 배열 dst[]의 끝까지 간다. 
        - 두번째 for 문을 이용하여 비어있는 dst[i+j] 위치에 
        - src[j]가 있는 내용을 차려로 넣는다.
        - 마지막인 dst[i+j]은 '\0'을 넣어주며 마무리
[Input]         : char dst[], char src[]
[Output]        : Nothing
[Call By]       : main()
[Calls]         : Nothing
[Given]         : Nothing
[Returns]       : Nothing
==================================================================*/

void strcat_a(char dst[], char src[])
{
	int i, j;

	for (i = 0 ; dst[i] != '\0' ; i++)
		;

	for (j = 0 ; src[j] != '\0' ; j++)
		dst[i+j] = src[j];	
	
	dst[i+j] = '\0';
}

/*===============================================================
[Function Name] : int strcmp_p(char *dst, char *src) 
[Description]   :
        - 포인터로 접근하여 문자열이 끝날때까지 반복
	- 비교하는 *dst와 *src에서
        - *dst의 아스키코드가 더 크면 return 1, 
        - *src의 아스키코드가 더 크면 return -1	
        - 둘이 같은 경우엔 각각 주소값을 1씩 증가시켜주고
        - 문자열이 끝날때까지 반복
	- while문 끝까지 돌아가면 두 문자열 동일 return 0
[Input]         : char *dst, char *src
[Output]        : Nothing
[Call By]       : main()
[Calls]         : Nothing
[Given]         : Nothing
[Returns]       : Nothing
==================================================================*/

int strcmp_p(char *dst, char *src) {
	
	while(*dst != '\0' || *src != '\0') {

		if(*dst < *src)
			return -1;
		else if (*dst > *src)
			return 1;
		else {
			*dst++;
			*src++;
		}
	}
	return 0;
}
/*===============================================================
[Function Name] : int strcmp_a(char dst[], char src[]) 
[Description]   :
        - for문으로 접근하여 배열이 끝날때까지 반복
        - 비교하는 dst[i]와 src[i]에서
        - dst[i]의 아스키코드가 더 크면 return 1,
        - src[i]의 아스키코드가 더 크면 return -1
        - for문이 끝날때까지 반복
        - for문 끝까지 돌아가면 두 문자열 동일 return 0
[Input]         : char dst[], char src[]
[Output]        : Nothing
[Call By]       : main()
[Calls]         : Nothing
[Given]         : Nothing
[Returns]       : Nothing
==================================================================*/
int strcmp_a(char dst[], char src[]) {
	
	for(int i = 0; dst[i] != '\0'; i++){

		if(dst[i] > src[i])
			return 1;
		else if (dst[i] < src[i])
			return -1;
		
	}
	
	return 0;
}

/*===============================================================
[Program Name] : string.c   void main()
[Description]  : 
    - 1. 문자열을 입력받고 strlen 함수의 point 접근과 array 접근을 통하여 길이를 잰다. 
    - 2. return된 길이(int)는 printf에 출력됨
    - 3. strcpy 함수로 문자열 str1, str2에 "Hello" 문자열 배열과 포인터 접근으로 복사
    - 4. strcat 함수로 생성된 문자열 str1, str2에 ", World!" 문자열을 배열과 포인터 접근으로 추가
    - 5. strcmp 함수로 생선된 문자열 str1, str2의 상태비교
[Input]        :
	Nothing
[Output]       :
	Nothing
[Calls]        :	        
	strlen_p(), strlen_a() // str 길이 return
	strcpy_p(), strcpy_a() // str 문자열 copy
	strcat_p(), strcat_a() // 첫번째 입력 str 뒤에 두번째 입력 str 붙이기 
	strcmp_p(), strcmp_a() // 입력받은 두개의 str 비교 
			       // 같으면 0 return, 아스키 코드 기준으로
			       // 첫번째가 크면 1, 두번째가 크면 -1 return

[특기사항]     : str1, str2 문자열은 20의 크기로 운영된다. 
==================================================================*/


void main()
{
	int		len1, len2;
	char	str1[20], str2[20];

	len1 = strlen_p("Hello");
	len2 = strlen_a("Hello");
	printf("strlen: p=%d, a=%d\n", len1, len2);

	strcpy_p(str1, "Hello");
	strcpy_a(str2, "Hello");
	printf("strcpy: p=%s, a=%s\n", str1, str2);

	strcat_p(str1, ", World!");
	strcat_a(str2, ", World!");
	printf("strcat: p=%s, a=%s\n", str1, str2);

	printf("strcmp: 0 is same p=%d, a=%d\n", strcmp_p(str1, str2), strcmp_a(str1, str2));
	
}
