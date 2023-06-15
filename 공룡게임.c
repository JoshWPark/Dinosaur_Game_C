#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdbool.h>

#define _CRT_SECURE_NO_WARNINGS 

//공룡 게임에 필요한 위치를 정함.
//1. 공룡 기본 자세의 Y좌표
#define DINO_BOTTOM_Y 20
//2. 나무의 Y좌표
#define TREE_BOTTOM_Y 23 
//3. 나무의 X좌표 (구름의 X좌표)
#define TREE_BOTTOM_X 45
//4. 구름의 Y좌표
#define CLOUD_Y 15
//5. 무적 아이템, 별의 Y좌표
#define STAR_Y 8
//6. 무적 아이템, 별의 X좌표
#define STAR_X 30

//최고 속도 제한
#define SPEEDLIMIT 10

// 색상 정의
#define BLACK 0
#define BLUE1 1
#define GREEN1 2
#define CYAN1 3
#define RED1 4
#define MAGENTA1 5
#define YELLOW1 6
#define GRAY1 7
#define GRAY2 8
#define BLUE2 9
#define GREEN2 10
#define CYAN2 11
#define RED2 12
#define MAGENTA2 13
#define YELLOW2 14
#define WHITE 15
#define ORANGE 32

//콘솔 넓이와 높이 정의
#define WIDTH 50
#define HEIGHT 30

//더블 버퍼링을 사용하기 위한 변수 선언
//hidden 화면 번호 0 아님 1
int hidden_index;
//화면 버퍼 변수
HANDLE scr_handle[2];

//더블 버퍼링 초기화 함수
void scr_init()
{
    CONSOLE_CURSOR_INFO cci;

    //화면 버퍼 2개를 만든다.
    scr_handle[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    scr_handle[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    //커서 숨기기
    cci.dwSize = 1;
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(scr_handle[0], &cci);
    SetConsoleCursorInfo(scr_handle[1], &cci);

    //0번 화면을 기본 화면으로 정하기
    SetConsoleActiveScreenBuffer(scr_handle[0]);
    //1번 화면을 숨기기
    hidden_index = 1;
}

//더블 버퍼링 화면 전환 함수
void scr_switch()
{
    //hidden 혹은 숨겨진 화면을 active 혹은 보여지는 화면으로 전환 
    SetConsoleActiveScreenBuffer(scr_handle[hidden_index]);
    //위 와는 반대로 보여졌던 화면을 숨기는 화면으로 변경
    //0 <-> 1 토글 
    hidden_index = !hidden_index;
}

//숨겨진 화면에 청소하는 함수
void scr_clear()
{

    COORD Coor = { 0, 0 };
    DWORD dw;
    // nLength 의 값은 설정한 화면의 값과 정확히 같아야하기 때문에 이미 정의해 둔
    // WIDTH 와 HEIGHT을 사용
    DWORD nLength = (int)(WIDTH * 2 * HEIGHT);
    //숨겨진 화면을 청소 혹은 clear 한다. 
    FillConsoleOutputCharacter(scr_handle[hidden_index], ' ', nLength, Coor, &dw);
}

//더블 버퍼링을 해제하는 함수
void scr_release()
{
    //더블 버퍼링 화면을 닫는다.
    CloseHandle(scr_handle[0]);
    CloseHandle(scr_handle[1]);
}

//숨겨진 버퍼링 화면에 그려주는 함수
void printscr(char* str)
{
    DWORD dw;
    //숨겨져있는 화면에 매개변수로 받아온 문자열을 그린다.
    WriteFile(scr_handle[hidden_index], str, strlen(str), &dw, NULL);
}

//숨겨진 버퍼링 화면에 숫자를 그려주는 함수
void printintscr(int num)
{
    //숫자를 문자열로 변경하기 위해 문자열 선언, 
    //이때 가장 크게 들어올 수 있는 매개변수 가 20자리 숫자라고 가정한다  
    char str[20];
    //숫자가 2자리수 이상 일시, 깨지는 것을 방지하기 위해 WCHAR 사용
    WCHAR wstr[20];
    DWORD dw;
    //위에서 선언한 문자열의 크기 
    int len = 0;

    //매개변수가 음수 일 시, '-'를 앞에 추가
    if (num < 0) {
        str[len++] = '-';
        num = -num;
    }

    //매개변수로 받아온 숫자를 뒤집어서 문자열에 넣어준다. 
    do {
        str[len++] = '0' + (num % 10);
        num /= 10;
    } while (num > 0);

    //그 후, 문자열을 뒤집어 준다. 
    for (int i = 0, j = len - 1; i < j; i++, j--) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }

    //문자열은 WCHAR로 변환
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, sizeof(wstr) / sizeof(wstr[0]));

    //WCHAR 로 변환된 문자열을 숨겨진 화면에 그린다.
    WriteConsoleW(scr_handle[hidden_index], wstr, len, &dw, NULL);

}

//이미 그려져 있는 숨겨진 화면의 커서의 위치를 x, y로 이동하는 함수
void scrBefore_gotoxy(int x, int y)
{
    DWORD dw;
    COORD Pos;
    Pos.X = 2 * x;
    Pos.Y = y;
    //이때, hidden_index의 값을 바꾸어서 넣어준다. 
    SetConsoleCursorPosition(scr_handle[!hidden_index], Pos);
}

//이미 그려져 있는 숨겨진 화면에 그리기 위한 함수
void printscrBefore(char* str)
{
    DWORD dw;
    //이때, hidden_index의 값을 바꾸어서 넣어준다. 
    WriteFile(scr_handle[!hidden_index], str, strlen(str), &dw, NULL);
}

//숨겨진 화면의 커서의 위치를 x, y로 이동하는 함수
void scr_gotoxy(int x, int y)
{
    DWORD dw;
    COORD Pos;
    Pos.X = 2 * x;
    Pos.Y = y;
    //이때, hidden_index의 값은 숨겨진 화면으로 넣어준다. 
    SetConsoleCursorPosition(scr_handle[hidden_index], Pos);
}

//숨겨진 화면에 그리기 위한 함수
void gotoxy(int x, int y)
{
    COORD Pos;
    Pos.X = 2 * x;
    Pos.Y = y;
    //이때, hidden_index의 값은 숨겨진 화면으로 넣어준다. 
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}


// 콘솔 창의 크기와 제목을 지정하는 함수
void SetConsoleView()
{
    system("mode con:cols=100 lines=30");
    system("title Google Dinosaurs. By BlockDMask.");
}

//콘솔의 색상을 바꾸기 위한 함수
void textcolor(int fg_color, int bg_color)
{
    // fg_color = 글자색 , bg_color = 배경 색
    //더블 버퍼링 2개의 색상을 바꿔준다. 
    SetConsoleTextAttribute(scr_handle[0], fg_color | bg_color << 4);
    SetConsoleTextAttribute(scr_handle[1], fg_color | bg_color << 4);

}

// 색상을 선택하여 설정하는 함수
void setConsoleColors(int colorNow) {
    //매개변수의 값에 의하여 색상이 변경한다.
    //매개변수의 값은 공룡의 무적 상태와 게임의 점수 증가에 따라 변경된다.  
    switch (colorNow) {
    case 1:
        textcolor(GREEN1, BLACK);
        break;
    case 2:
        textcolor(GRAY2, BLACK);
        break;
    case 3:
        textcolor(BLUE2, BLACK);
        break;
    case 4:
        textcolor(MAGENTA1, BLACK);
        break;
    case 5:
        textcolor(YELLOW1, BLACK);
        break;
    default:
        break;
    }
}

// 공룡을 그리는 함수
//공룡의 Y값과 현재 공룡의 상태에 따라 바뀐다.
void DrawDino(int dinoY, bool dinoPosition)
{
    //공룡을 숨겨진 화면에 그린다. 
    scr_gotoxy(0, dinoY);
    //다리를 번갈아 가며 그리기 위한 bool형 변수, 처음에만 true로 선언
    static bool legFlag = true;


    //공룡이 뛰고 있거나 서있는 경우,
    if (dinoPosition == false)
    {
        printscr("            ■         ■\n");
        printscr("            ■    ■   ■\n");
        printscr("           ■■■■■■■ \n");
        printscr("          ■■□■■■■■■■\n");
        printscr("          ■■■■■■■■■■\n");
        printscr("■        ■■■■      \n");
        printscr("■■      ■■■■■■■  \n");
        printscr("■■■   ■■■■■     \n");
        printscr("  ■■  ■■■■■■■■■■■ \n");
        printscr("  ■■■■■■■■■■■    ▼\n");
        printscr("  ■■■■■■■■■■■    \n");
        printscr("    ■■■■■■■■■■    \n");
        printscr("        ■■■■■■■■    \n");
        printscr("          ■■■■■■     \n");
    }
    //공룡이 숙이고 있는 경우
    if (dinoPosition == true)
    {
        printscr("                             ■       ■\n");
        printscr("■                           ■   ■  ■\n");
        printscr("■■     ■■■■■■■     ■■■■■■\n");
        printscr("  ■■ ■■■■■■■■■  ■■□■■■■■\n");
        printscr("    ■■■■■■■■■■■ ■■■■■■■■\n");
        printscr("      ■■■■■■■■■■ ■■■\n");
        printscr("       ■■■■■■■■■  ■■■■■■\n");
        printscr("            ■■■■■  \n");
        //공룡이 숙이고 있는 모습을 두 화면에 그리기 위해 추가
        scrBefore_gotoxy(0, dinoY);
        printscrBefore("                             ■       ■\n");
        printscrBefore("■                           ■   ■  ■\n");
        printscrBefore("■■     ■■■■■■■     ■■■■■■\n");
        printscrBefore("  ■■ ■■■■■■■■■  ■■□■■■■■\n");
        printscrBefore("    ■■■■■■■■■■■ ■■■■■■■■\n");
        printscrBefore("      ■■■■■■■■■■ ■■■\n");
        printscrBefore("       ■■■■■■■■■  ■■■■■■\n");
        printscrBefore("            ■■■■■  \n");
    }
    //지역 변수 legFlag 가 true 일 시,
    if (legFlag)
    {
        printscr("            ■   ■■■    \n");
        printscr("            ■■          ");
        //legFlag 변수를 false 값으로 바꾸기
        legFlag = false;
    }
    //지역 변수 legFlag 가 false 일 시,
    else
    {
        printscr("             ■■■  ■     \n");
        printscr("                     ■■    ");
        //legFlag 변수를 true 값으로 바꾸기
        legFlag = true;
    }

    //출력 버퍼를 비우기 위한 함수
    fflush(stdout);
}

// 나무를 그리는 함수
//나무의 X값과 나무의 종류에 따라 바뀐다.
void DrawTree(int treeX, int random)
{
    //나무를 숨겨진 화면에 그린다. 
    //매개변수로 받아온 나무의 종류에 따라 그려지는 나무의 형태가 달라진다. 
    if (random == 1)
    {
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 1);
        printscr("  ■■■■");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 2);
        printscr("■■■■■■");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 3);
        printscr("■■■■■■");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 4);
        printscr("    ■■ ");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 5);
        printscr("    ■■ ");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 6);
        printscr("    ■■ ");
    }
    else if (random == 2)
    {
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 1);
        printscr("     ■■■   ■■■");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 2);
        printscr("  ■■■■■ ■■■■■ ");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 3);
        printscr("■■■■■■ ■■■■■■ ");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 4);
        printscr("    ■■         ■■");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 5);
        printscr("    ■■         ■■");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 6);
        printscr("    ■■         ■■");
    }
    else
    {
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 1);
        printscr("■■■■ ■■■■ ■■■■ ");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 2);
        printscr("■■■■ ■■■■ ■■■■ ");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 3);
        printscr("■■■■ ■■■■ ■■■■ ");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 4);
        printscr("  ■■     ■■     ■■");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 5);
        printscr("  ■■     ■■     ■■");
        scr_gotoxy(treeX, TREE_BOTTOM_Y + 6);
        printscr("  ■■     ■■     ■■");
    }

    //출력 버퍼를 비우기 위한 함수
    fflush(stdout);
}

//구름을 그리는 함수
//구름의 X값에 따라 바뀐다.
void DrawCloud(int cloudX)
{
    //구름을 숨겨진 화면에 그린다. 
    scr_gotoxy(cloudX, CLOUD_Y);
    printscr("    ■");
    scr_gotoxy(cloudX, CLOUD_Y + 1);
    printscr("  ■■■");
    scr_gotoxy(cloudX, CLOUD_Y + 2);
    printscr("■■■■■");

    //출력 버퍼를 비우기 위한 함수
    fflush(stdout);
}

// 별을 그리는 함수
//별의 X값에 따라 바뀐다.
void DrawStar(int starX)
{
    //별을 숨겨진 화면에 그린다.
    scr_gotoxy(starX, STAR_Y);
    printscr("  ▲");
    scr_gotoxy(starX, STAR_Y + 1);
    printscr("◀■▶");
    scr_gotoxy(starX, STAR_Y + 2);
    printscr("  ▼");

    //출력 버퍼를 비우기 위한 함수
    fflush(stdout);
}

//점수를 그리는 함수
void DrawScore(int score)
{
    //점수 출력 판의 위치를 정함
    scr_gotoxy(40, 0);

    //점수 출력판을 숨겨진 화면에 그려줌
    printscr("Score : ");

    //점수의 위치를 정함
    scr_gotoxy(44, 0);

    //점수를 숨겨진 화면에 그려줌
    printintscr(score);

    //출력 버퍼를 비우기 위한 함수
    fflush(stdout);
}

//전방 참조
void startmenu();

// 충돌 했을때 게임오버 그려줌
void DrawGameOver(const int score)
{
    system("cls");
    gotoxy(23, 1);
    printf("SCORE : %d", score);

    gotoxy(8, 3);
    printf("▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼\n");
    gotoxy(11, 7);
    printf("■■■■■■         ■        ■■        ■■  ■■■■■■ \n");
    gotoxy(11, 8);
    printf("■              ■        ■   ■ ■      ■ ■  ■      \n");
    gotoxy(11, 9);
    printf("■             ■          ■  ■  ■    ■  ■  ■      \n");
    gotoxy(11, 10);
    printf("■   ■■■■  ■■■■■■■  ■   ■  ■   ■  ■■■■■■ \n");
    gotoxy(11, 11);
    printf("■         ■  ■          ■  ■    ■■    ■  ■      \n");
    gotoxy(11, 12);
    printf("■■■■■ ■  ■          ■  ■     ■     ■  ■      \n");
    gotoxy(11, 13);
    printf("           ■  ■          ■  ■            ■  ■■■■■■ \n");

    gotoxy(8, 15);
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

    gotoxy(12, 17);
    printf("    ■■■■   ■        ■  ■■■■■  ■■■■■  \n");
    gotoxy(12, 18);
    printf("   ■      ■   ■      ■   ■          ■      ■  \n");
    gotoxy(12, 19);
    printf("   ■  ■  ■    ■    ■    ■■■■■  ■■■■■  \n");
    gotoxy(12, 20);
    printf("   ■      ■     ■  ■     ■          ■ ■       \n");
    gotoxy(12, 21);
    printf("    ■■■■        ■       ■■■■■  ■  ■■■  \n");
    gotoxy(8, 25);
    printf("▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲\n");

    gotoxy(1, 28);
    //사용자 입력 값을 받기 위해 추가
    system("pause");
}

// 장애물에 충돌했으면 true, 아니면 false
bool isCollision(const int treeX, bool isBottom, bool treeOrCloud, bool isDown, bool isJumping, bool isGoingDown)
{
    //장애물이 나무 일 때 
    // 트리의 X가 공룡의 몸체쪽에 있고,
    // 공룡의 상태가 서있거나, 숙이고 있으면
    // 충돌
    if (!treeOrCloud)
    {
        if (treeX <= 8 && treeX >= 4 &&
            (isBottom || isDown))
        {
            return true;
        }
        return false;
    }

    //장애물이 구름일 때 
    // 트리의 X 혹은, 구름의 X가 공룡의 몸체쪽에 있고,
    // 공룡의 상태가 뛰고 있거나, 서 있거나, 뛰고 내려오는 중이거나
    // 숙이고 있지 않으면
    // 충돌
    else
    {
        if (treeX <= 8 && treeX >= 4 &&
            ((isBottom || isJumping || isGoingDown) && !isDown))
        {
            return true;
        }
        return false;

    }

}

// 무적 아이템에 충돌했으면 true, 아니면 false
bool isStarCollision(const int starX, const int dinoY)
{
    // 별의 X가 공룡의 몸체쪽에 있을때,
    // 공룡의 Y값이 '8'보다 작으면 혹은 뛰고 있으면
    // 충돌
    if (starX <= 8 && starX >= 4 &&
        dinoY < 8)
    {
        return true;
    }
    return false;
}

//화면에 그리는 함수들을 모아둔 함수
void DrawScreen(int dinoY, int treeX, int treenum, int treeCount, int starX, bool isInvincible, int score, bool dinoPosition, bool treeOrCloud, int treeInvincibleCount)
{
    //그릴려고 하는 숨겨진 화면을 청소해준다
    scr_clear();

    //공룡의 Y 값과, 공룡의 상태에 근거하여 
    //공룡을 그려준다
    DrawDino(dinoY, dinoPosition);

    //매개변수값에 따라 나무를 그릴지, 구름을 그릴지 결정한다.
    if (treeOrCloud)
    {
        DrawCloud(treeX);
    }
    else
    {
        DrawTree(treeX, treenum);
    }

    //나무 3개 넘을때 마다 무적 아이템 을 그려준다
    if (treeCount >= 3)
    {
        DrawStar(starX);
    }

    //점수를 그린다.
    DrawScore(score);
}

//공룡 게임 함수
void gamestart()
{
    // 게임 시작시 더블 버퍼링 설정 초기화
    scr_init();
    // 랜덤 숫자를 생성하기 위하 추가
    srand(time(NULL));
    // 게임 시작시 초기화 되는 bool 변수들
    //공룡이 뛰고 있는지 아닌지를 구분하기 위한 변수
    //false = 뛰고 있지 않음, true = 뛰고 있음
    bool isJumping = false;
    //공룡이 걷고 있는지를 구분하기 위한 변수
    //false = 걷고 있지 않음, true = 걷고 있음
    bool isBottom = true;
    //공룡이 무적 상태인지를 구분하기 위한 변수
    //false = 무적이 아님, true = 무적임
    bool isInvincible = false;
    //공룡이 숙이고 있는지를 구분하기 위한 변수
    //false = 숙이고 있지 않음, true = 숙이고 있음
    bool isDown = false;
    //공룡이 뛰고 난후 떨어지고 있는지를 구분하기 위한 변수
    //false = 떨어지고 있지 않음, true = 떨어지고 있음
    bool isGoingDown = false;
    //화면의 색상이 바뀌어야 하는지 구분하는 변수
    //false = 바뀌면 안됨, true = 바뀌여야함
    bool isColorChange = false;
    //공룡의 상태를 구분하기 위한 변수
    //false = 걷고있거나, 뛰고 있음, true = 숙이고 있음
    bool dinoPosition = false;
    //장애물의 종류를 구분하기 위한 변수
    //false = 나무, true = 구름
    bool treeOrCloud = false;

    // 게임 시작시 초기화 되는 int 변수들
    //게임속도
    int gamespeed = 100;
    // 현재 화면의 색상
    int colorNow = 1;
    // 중력
    const int gravity = 3;
    // 나무의 종류
    int treenum = 1;
    // 나무를 넘은 개수
    int treeCount = 0;
    // 무적 상태 일 때, 나무를 넘은 개수
    int treeInvincibleCount = 0;
    // 무적 상태 일 때, 점수 
    int invincibleScore = 0;
    // 무적 상태 일 때, 오르는 변수  
    int invincibleCount = 0;
    // 무적 상태의 시간을 정하는 변수
    int invincibleSpeed = 50;
    //점수 변수
    int score = 0;

    // 게임 시작시 초기화 되는 위치 int 변수들, 처음에 정의한 #define 값을 사용
    // 공룡의 Y값
    int dinoY = DINO_BOTTOM_Y;
    // 장애물의 X값
    int treeX = TREE_BOTTOM_X;
    // 무적 아이템의 X값
    int starX = STAR_X;

    // 시작시간 초기화
    clock_t start, curr;
    start = clock();

    //게임  한 판에 대한 루프
    while (true)
    {
        //숨겨진 화면으로 바꿔주는 함수 호출
        scr_switch();

        // 충돌체크 장애물의 x값과 장애물의 종류, 공룡의 상태로 판단
        // 무적 상태가 아닐 시에만 충돌 체크 
        if (isCollision(treeX, isBottom, treeOrCloud, isDown, isJumping, isGoingDown) && !isInvincible)
            break;

        // 사용자 입력 값으로 space키가 눌렸고, 걷고있는 중이면 점프
        if (kbhit() == 1 && getch() == ' ' && isBottom)
        {
            //뛰고 있는 중으로 바꿔준다
            isJumping = true;
            //걷고 있지 않은 상태로 바꿔준다
            isBottom = false;
        }

        // 사용자 입력 값으로 아래 방향키가 눌렸고, 걷고있는 중이면 점프
        if (kbhit() == 1 && getch() == 80 && isBottom)
        {
            //숙이고 있는 중으로 바꿔준다
            isDown = true;
            //공룡의 상태도 숙이고 있는 중으로 바꿔준다
            dinoPosition = true;
        }
        //아닐 시,
        else
        {
            //숙이고 있지 않은 중으로 바꿔준다
            isDown = false;
            //공룡의 상태도 걷고 있는 중으로 바꿔준다
            dinoPosition = false;
        }

        // 점프중이라면 공룡의 Y값을 감소, 점프가 끝났으면 Y를 증가.
        if (isJumping)
        {
            //중력을 사용해 감소
            dinoY -= gravity;
        }
        else
        {
            //중력을 사용해 증가
            dinoY += gravity;
        }

        //  공룡의 Y값이 계속해서 증가하는걸 막기위해 #define에 선언된 공룡의 기본자세 Y값으로 정한다.
        if (dinoY >= DINO_BOTTOM_Y)
        {
            //공룡의 Y값을 기본자세 Y값으로 바꿔준다
            dinoY = DINO_BOTTOM_Y;
            //공룡이 걷고 있는 중으로 바꾼다
            isBottom = true;
            //공룡이 뛰고 난 후 내려오지 않는 중으로 바꾼다
            isGoingDown = false;

        }

        // 장애물이 왼쪽으로 (x음수) 가도록하고
        // 장애물의 위치가 왼쪽 끝으로가면 다시 오른쪽 끝으로 소환.
        treeX -= 2;
        //장애물의 X값이 0보다 작을 시,
        if (treeX <= 0)
        {
            //오른쪽으로 다시 소환하기 위해 장애물의 기본 X값으로 바꿔준다
            treeX = TREE_BOTTOM_X;
            //장애물(나무)를 그릴때 사용할 무작위 숫자를 생성
            treenum = (rand() % 3) + 1;
            // 만약 공룡이 무적 상태가 아닐 시,
            if (!isInvincible)
            {
                //공룡이 지나간 장애물의 개수 증가
                treeCount++;
                //색상이 바뀌여야 한다고 설정
                isColorChange = true;
            }

            // 만약 공룡이 무적 상태 일 시,
            if (isInvincible)
            {
                //무적 상태인 공룡이 지나간 장애물의 개수 증가
                treeInvincibleCount++;
            }

            // 만약 공룡이 무적 상태가 아닐 때 지나간 장애물의 개수가 3 초과 일 시, 
            if (treeCount > 3)
            {
                //공룡이 지나간 장애물의 개수를 0으로 초기화
                treeCount = 0;

            }

            //공룡이 장애물을 넘어가면
            // 스코어 UP
            score++;
        }

        // 점프의 맨위를 찍으면 점프가 끝난 상황.
        if (dinoY <= 3)
        {
            //공룡이 뛰고 있지 않은 상태로 변경
            isJumping = false;
            //공룡이 뛰고 내려오는 중이라고 변경
            isGoingDown = true;
        }


        // 게임 점수가 0보다 크고, 5의 배수이며, 색상을 변경하는지 구분하는 변수가 참 일 시
        // 색상 변경 및 게임 속도 증가
        if (score > 0 && score % 5 == 0 && isColorChange)
        {
            //색상 변수를 증가
            colorNow++;
            //만약 색상 변수가 4 초과 일 시,
            if (colorNow > 4)
            {
                //색상 변수 초기화
                colorNow = 1;
            }

            //만약 공룡이 무적 상태가 아닐시에만 색상 변경
            if (!isInvincible)
            {
                //색상을 변경하는 함수 호출
                setConsoleColors(colorNow);
            }

            //색상을 변경하는지 구분하는 함수를 거짓으로 바꿈
            isColorChange = false;

            //게임 속도를 줄여 줌으로써 게임 속도 증가
            gamespeed -= 15;
            // 무적 상태 시간 증가
            invincibleSpeed += 5;
            //게임 속도가 최고 속도제한 보다 작을 시
            if (gamespeed < SPEEDLIMIT)
            {
                //게임 속도를 최고 속도로 유지
                gamespeed = 10;
                //무적 상태 시간 또한 유지
                invincibleSpeed = invincibleSpeed;
            }

        }

        // 장애물의 종류를 나무라 정한 뒤
        treeOrCloud = false;
        // 나무를 기본 상태 일때 혹은 무적 상태 일 때,
        // 2개를 넘었으면,  
        if (treeCount == 2 || treeInvincibleCount == 2)
        {
            //장애물을 구름으로 변경
            treeOrCloud = true;
        }

        //나무 3개 넘을때 마다 무적 아이템 소환
        if (treeCount >= 3)
        {
            // 충돌체크 별의 x값과 공룡의 y값으로 판단
            if (isStarCollision(starX, dinoY))
            {
                //공룡의 상태를 무적 상태로 변경
                isInvincible = true;
                //색상을 무적 상태 색상으로 변경
                setConsoleColors(5);
                //무적 상태인 공룡이 지나간 장애물의 개수 초기화
                treeInvincibleCount = 0;
                //무적 상태가 아닌 공룡이 지나간 장애물의 개수 초기화
                treeCount = 0;
            }

            // 별이 왼쪽으로 (x음수) 가도록하고
            // 별의 위치가 왼쪽 끝으로가고 충돌 하지 않았다면 못 먹은 것으로 판단
            starX -= 2;
            if (starX <= 0)
            {
                //별 (무적 아이템) 의 X 값을 기본 X값으로 초기화
                starX = STAR_X;
                //무적 상태인 공룡이 지나간 장애물의 개수 초기화
                treeInvincibleCount = 0;
                //무적 상태가 아닌 공룡이 지나간 장애물의 개수 초기화
                treeCount = 0;
            }

        }

        //콘솔창에 그려져야 하는 것들을 모은 함수호출
        DrawScreen(dinoY, treeX, treenum, treeCount, starX, isInvincible, score, dinoPosition, treeOrCloud, treeInvincibleCount);

        // 현재시간 받아오기
        curr = clock();

        //공룡이 무적 상태 일 시,
        if (isInvincible)
        {
            //무적 상태 카운트 증가
            invincibleCount++;
        }
        //무적 상태 카운트가 무적 상태 지속 시간보다 클 시,
        //무적 상태 해제
        if (invincibleCount > invincibleSpeed)
        {
            //공룡의 무적 상태를 해제
            isInvincible = false;
            //별 (무적 아이템) 의 X 값을 기본 X값으로 초기화
            starX = STAR_X;
            //무적 상태 카운트 초기화
            invincibleCount = 0;
            //무적 상태인 공룡이 지나간 장애물의 개수 초기화
            treeInvincibleCount = 0;
            //색상을 재변경
            setConsoleColors(colorNow);
        }

        // 콘솔창 청소
        system("cls");

        //게임속도에 의하여 정지
        Sleep(gamespeed);
    }


    //더블 버퍼링을 해제하는 함수 호출
    scr_release();
    //(v2.0) 게임 오버 메뉴를 그려주는 함수 호출
    DrawGameOver(score);
    //게임 메뉴를 보여주는 함수 호출
    startmenu();
}

//게임 화면의 테두리를 그리기 위한 함수
void draw_box(int x1, int y1, int x2, int y2, char* ch)
{
    int y;
    int x;
    textcolor(WHITE, BLACK);
    // 세로 테두리를 그리는 for문
    for (y = y1; y <= y2; y += 2)
    {
        gotoxy(x1, y);
        printf("%s", ch);
        gotoxy(x2, y);
        printf("%s", ch);
    }
    // 가로 테두리를 그리는 for문
    for (x = x1; x <= x2; x += 2)
    {
        gotoxy(x, y1);
        printf("%s", ch);
        gotoxy(x, y2);
        printf("%s", ch);
    }

}

//게임 설명을 그려주는 함수
void help()
{
    // 색상을 정해주는 함수 호출
    textcolor(15, 0);
    // 화면을 먼저 지운다
    system("cls");
    gotoxy(20, 3);
    printf("◆공룡게임 도움말◆");
    gotoxy(12, 7);
    printf("(1) SPACE키를 통해서 공룡을 점프 시킬 수 있습니다.");
    gotoxy(12, 9);
    printf("(2) 아래 키[↓]를 통해서 공룡을 숙이게 시킬 수 있습니다.");
    gotoxy(12, 11);
    printf("(3) 하단 나무를 점프하여 통과 시 점수가 증가합니다. ");
    gotoxy(12, 13);
    printf("(4) 상단 구름을 숙여서 통과 시 점수가 증가합니다. ");
    gotoxy(12, 15);
    printf("(5) 장애물 3번 통과 시 무적 아이템이 생성됩니다.");
    gotoxy(12, 17);
    printf("(6) 아이템 획득 시 일정시간 무적상태로 변합니다.");
    gotoxy(12, 19);
    printf("(7) 점수 5배수마다 속도가 빠르게 변하고, 공룡 색이 변합니다.");
    gotoxy(12, 21);
    printf("(8) 장애물과 충돌 시 게임 오버. ");
    //사용자한테서 아무 입력 값을 받은 후 함수 종료
    gotoxy(15, 25);
    system("pause");
}

//게임의 시작화면을 그리는 함수
void startPage()
{
    //게임 화면의 테두리를 그리는 함수를 호출
    draw_box(0, 0, 49, 29, "◆");
    //기본 색을 설정 하고 그려주는 함수 호출
    textcolor(10, 0);
    //시작화면을 그려준다,
    gotoxy(11, 3);
    printf("  ■■■■■■■■■■            ■■■■■■■■■  \n");
    gotoxy(11, 4);
    printf("                    ■                            ■  \n");
    gotoxy(11, 5);
    printf("                    ■            ■■■■■■■■■  \n");
    gotoxy(11, 6);
    printf("                    ■            ■           \n");
    gotoxy(11, 7);
    printf("            ■      ■            ■■■■■■■■■   \n");
    gotoxy(11, 8);
    printf("            ■                          ■  ■    \n");
    gotoxy(11, 9);
    printf("■■■■■■■■■■■■■    ■■■■■■■■■■■■■\n");
    gotoxy(11, 11);
    printf("          ■■■                        ■■■    \n");
    gotoxy(11, 12);
    printf("        ■■  ■■                    ■■  ■■    \n");
    gotoxy(11, 13);
    printf("          ■■■                        ■■■   \n");
    gotoxy(10, 16);
    printf("■■■■■■         ■        ■■        ■■  ■■■■■■ \n");
    gotoxy(10, 17);
    printf("■              ■        ■   ■ ■      ■ ■  ■      \n");
    gotoxy(10, 18);
    printf("■             ■          ■  ■  ■    ■  ■  ■      \n");
    gotoxy(10, 19);
    printf("■   ■■■■  ■■■■■■■  ■   ■  ■   ■  ■■■■■■ \n");
    gotoxy(10, 20);
    printf("■         ■  ■          ■  ■    ■■    ■  ■      \n");
    gotoxy(10, 21);
    printf("■■■■■ ■  ■          ■  ■     ■     ■  ■      \n");
    gotoxy(10, 22);
    printf("           ■  ■          ■  ■            ■  ■■■■■■ \n");
    gotoxy(10, 25);
    //사용자한테서 아무 입력 값을 받은 후 함수 종료
    getch();
}

//게임 메뉴를 보여주는 함수
void startmenu()
{
    //사용자의 입력 값
    int a;
    //while 반복문을 사용해 사용자가 정해진 메뉴에서만 고를 수 있게 한다. 
    while (1)
    {
        // 화면 지우기
        system("cls");
        // 화면 테두리 그리기
        draw_box(0, 0, 49, 29, "◆");
        // 화면 색상 정하고 그리기
        textcolor(10, 0);
        gotoxy(21, 8);
        printf(" ◆공룡 게임◆");
        gotoxy(21, 11);
        printf("게임시작 : (1)");
        gotoxy(21, 14);
        printf(" 도움말  : (2)");
        gotoxy(22, 17);
        printf("종료   : (3)");

        gotoxy(19, 23);
        printf("번호를 입력하시오: ");
        scanf_s("%d", &a);

        //입력 값이 '1'일 시,
        if (a == 1)
        {
            //게임 시작
            gamestart();
            break;

        }

        //입력 값이 '2'일 시,
        else if (a == 2)
        {
            //게임 도움말 함수 호출
            help();

        }

        //입력 값이 '3'일 시,
        else if (a == 3)
        {
            //게임 종료
            exit(0);
        }
    }
}


int main()
{
    //콘솔의 크기를 정해주는 함수를 먼저 호출한다.
    SetConsoleView();
    //시작 페이지를 보여주는 함수 호출
    startPage();
    //게임 메뉴를 보여주는 함수 호출
    startmenu();
}