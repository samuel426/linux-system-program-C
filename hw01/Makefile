# 컴파일러를 지정
CC = gcc

# 컴파일 옵션을 지정. -Wall은 모든 경고를 출력하고, -g는 디버깅 정보를 포함
CFLAGS = -Wall -g

# 링크 옵션을 지정. 수학 라이브러리(libm) 링크
LDFLAGS = -lm

# 실행 파일 이름 지정
TARGET = myprogram

# 소스 파일이 위치한 디렉토리 지정
SRC_DIR = src
OBJ_DIR = obj

# 소스 파일 목록 지정. 여러 디렉토리에서 소스 파일 가져옴
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/utils.c $(SRC_DIR)/math_ops.c

# 객체 파일 목록 지정. 소스 파일의 확장자를 .o로 변환
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/math_ops.o

# 헤더 파일이 있는 디렉토리 지정
INCLUDE_DIR = include

# 기본 타겟 지정. 'all'은 기본으로 실행되는 규칙
all: $(TARGET)

# 타겟 파일 생성 규칙. 객체 파일을 링크하여 실행 파일 생성
$(TARGET): $(OBJ)
	@echo "Linking... $(TARGET)" #링크 과정 알림 메시지 출력, '@'는 명령어를 출력하지 않음
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) # 객체 파일을 링크하여 실행 파일 생성
	# $@ : 타겟 파일 이름 (myprogram)
	# $^ : 모든 의존성 파일 (여기선 $(OBJ))

# 객체 파일 생성 규칙. 각 .c 파일을 컴파일하여 .o 파일 생성
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $< ..." # 현재 컴파일 중인 .c 파일 이름 출력. '$<'는 첫 번째 의존성인 .c 파일
	mkdir -p $(OBJ_DIR) # 객체 파일을 저장할 디렉토리 생성. -p는 이미 존재해도 오류가 나지 않게 함
	$(CC) $(CFLAGS) -I $(INCLUDE_DIR) -c -o $@ $< # .c 파일을 컴파일하여 .o 파일 생성
	# -I : 헤더 파일 경로 지정
	# -c : 컴파일만 하고 링크하지 않음
	# -o $@ : 생성될 객체 파일의 이름

# 'clean' 규칙. 이 규칙 호출 시 생성된 객체 파일과 실행 파일 삭제
clean:
	@echo "Cleaning up..." # 정리 과정 알림 메시지 출력
	rm -f $(OBJ_DIR)/*.o $(TARGET) # 객체 파일과 실행 파일 삭제. -f는 존재하지 않는 파일에 대한 오류를 무시

# 'phony' 타겟 지정. all과 clean을 실제 파일 이름과 혼동하지 않도록 함
.PHONY: all clean

