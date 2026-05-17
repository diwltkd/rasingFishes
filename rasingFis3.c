#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#define NUM 6
#define MAX_WATER 100
#define WATER_AMOUNT 10
#define MAX_LEVEL 5
#define MAX_SIZE 5

// 물고기 상태 구조체
typedef struct {

    int waterLevel;    // 물 높이
    int isAlive;       // 생존 여부
    int size;          // 물고기 크기 (1~5)

} FishTank;

// 전역 변수
FishTank fishTanks[NUM];

int level;

long prevElapsedTime;
long totalElapsedTime;
long startTime = 0;

FishTank* cursor;

int position = 0;

// 함수 선언
void initData();
void update();
void render();

void decreaseWater(long elapsedTime);

int checkFishAlive();
int checkWin();

void gotoxy(int x, int y);
int nonBlockingGetch();

void processInput();

void moveCursorLeft();
void moveCursorRight();

void giveWater();

void levelUpFish();

char* getFishState(FishTank tank);

int main(void) {

    int gameOver = 0;

    initData();

    startTime = clock();

    cursor = fishTanks;

    system("cls");

    while (!gameOver) {

        position = cursor - fishTanks;

        processInput();

        update();

        render();

        // 레벨업
        if (level < MAX_LEVEL &&
            totalElapsedTime / 20 > level - 1) {

            level++;

            levelUpFish();

            gotoxy(2, 15);
            printf("레벨업! 현재 레벨 : %d", level);
        }

        // 승리
        if (checkWin()) {
            gameOver = 1;
        }

        // 패배
        if (checkFishAlive() == 0) {

            gotoxy(2, 16);

            printf("모든 물고기가 죽었습니다.");

            gameOver = 1;
        }

        prevElapsedTime = totalElapsedTime;

        Sleep(100);
    }

    getchar();

    return 0;
}

// 초기화
void initData() {

    level = 1;

    prevElapsedTime = 0;
    totalElapsedTime = 0;

    for (int i = 0; i < NUM; i++) {

        fishTanks[i].waterLevel = 100;

        fishTanks[i].isAlive = 1;

        fishTanks[i].size = 1;
    }
}

// 입력 처리
void processInput() {

    int input = nonBlockingGetch();

    if (input == -1) {
        return;
    }

    switch (input) {

    case 'j':
        moveCursorLeft();
        break;

    case 'l':
        moveCursorRight();
        break;

    case 'k':
        giveWater();
        break;

    default:
        gotoxy(2, 18);
        printf("잘못된 입력입니다.      ");
        break;
    }
}

// 왼쪽 이동
void moveCursorLeft() {

    if (cursor > fishTanks) {
        cursor--;
    }
}

// 오른쪽 이동
void moveCursorRight() {

    if (cursor < fishTanks + NUM - 1) {
        cursor++;
    }
}

// 물 주기
void giveWater() {

    // 죽은 물고기면 물 못줌
    if (cursor->isAlive == 0) {

        gotoxy(2, 17);

        printf("죽은 물고기에게는 물을 줄 수 없습니다.");

        return;
    }

    cursor->waterLevel += WATER_AMOUNT;

    if (cursor->waterLevel > MAX_WATER) {

        cursor->waterLevel = MAX_WATER;
    }
}

// 레벨업 시 물고기 크기 증가
void levelUpFish() {

    for (int i = 0; i < NUM; i++) {

        if (fishTanks[i].isAlive &&
            fishTanks[i].size < MAX_SIZE) {

            fishTanks[i].size++;
        }
    }
}

// 업데이트
void update() {

    totalElapsedTime =
        (clock() - startTime) / CLOCKS_PER_SEC;

    prevElapsedTime =
        totalElapsedTime - prevElapsedTime;

    decreaseWater(prevElapsedTime);
}

// 물 감소
void decreaseWater(long elapsedTime) {

    for (int i = 0; i < NUM; i++) {

        // 죽은 물고기는 계산 안함
        if (fishTanks[i].isAlive == 0) {
            continue;
        }

        // 크기가 커질수록 물 감소량 증가
        int decreaseAmount =
            level * fishTanks[i].size * 2;

        fishTanks[i].waterLevel -=
            decreaseAmount * elapsedTime;

        // 물 부족하면 사망
        if (fishTanks[i].waterLevel <= 0) {

            fishTanks[i].waterLevel = 0;

            fishTanks[i].isAlive = 0;
        }
    }
}

// 물고기 상태 반환
char* getFishState(FishTank tank) {

    if (tank.isAlive == 0) {
        return "죽음";
    }

    if (tank.waterLevel >= 80) {
        return "매우 건강";
    }

    if (tank.waterLevel >= 50) {
        return "건강";
    }

    if (tank.waterLevel >= 20) {
        return "위험";
    }

    return "매우 위험";
}

// 화면 출력
void render() {

    system("cls");

    gotoxy(2, 2);
    printf("@ 물고기 키우기 게임 @");

    gotoxy(2, 3);
    printf("현재 레벨 : %d / %d", level, MAX_LEVEL);

    gotoxy(2, 4);
    printf("총 경과 시간 : %ld초", totalElapsedTime);

    // 번호 출력
    gotoxy(2, 6);

    for (int i = 0; i < NUM; i++) {

        printf("    [%d]    ", i + 1);
    }

    // 물 출력
    gotoxy(2, 7);

    for (int i = 0; i < NUM; i++) {

        printf("    %3d     ", fishTanks[i].waterLevel);
    }

    // 크기 출력
    gotoxy(2, 8);

    for (int i = 0; i < NUM; i++) {

        printf("    %d단계   ", fishTanks[i].size);
    }

    // 생존 여부 출력
    gotoxy(2, 9);

    for (int i = 0; i < NUM; i++) {

        if (fishTanks[i].isAlive) {
            printf("      O     ");
        }
        else {
            printf("      X     ");
        }
    }

    // 상태 출력
    gotoxy(2, 10);

    for (int i = 0; i < NUM; i++) {

        printf(" %-10s ", getFishState(fishTanks[i]));
    }

   
    // 커서 표시
    gotoxy(2, 14);

    for (int i = 0; i < position; i++) {
            
        printf("            ");
    }

    printf("       ^");

    // 조작법
    gotoxy(2, 20);

    printf("왼쪽(j)  물주기(k)  오른쪽(l)");
}

// 승리 체크
int checkWin() {

    if (level >= MAX_LEVEL) {

        gotoxy(2, 16);

        printf("최고 레벨 달성!");

        return 1;
    }

    return 0;
}

// 생존 체크
int checkFishAlive() {

    for (int i = 0; i < NUM; i++) {

        if (fishTanks[i].isAlive) {
            return 1;
        }
    }

    return 0;
}

// 커서 이동
void gotoxy(int x, int y) {

    COORD CursorPosition = { x, y };

    SetConsoleCursorPosition(
        GetStdHandle(STD_OUTPUT_HANDLE),
        CursorPosition
    );
}

// 논블로킹 입력
int nonBlockingGetch() {

    if (_kbhit()) {

        return _getch();
    }

    return -1;
}