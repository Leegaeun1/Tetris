// WinAPI.cpp : 애플리케이션에 대한 진입점을 정의합니다.

#include "framework.h"
#include "WinAPI.h"
#include<vector>
#include <cstdlib> // rand(), srand()
#include <ctime>   // time()
#include <windows.h>

#define TIMER_ID 1
int timer_interval = 1000; // 1초 (1000ms)
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HDC memDC;
HBITMAP hBitmap;
HBITMAP oldBitmap;
bool gameOver = false; // 게임오버인지
bool ishold = false; // 홀드하고있는게 있는지
int holdCnt = 0; //


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINAPI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPI));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}
//  함수: MyRegisterClass()
//  용도: 창 클래스를 등록합니다.
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPI));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINAPI);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
//   함수: InitInstance(HINSTANCE, int)
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//   주석:
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, L"Tetris", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}
struct Block {
    int x, y;              // 현재 위치
    int shape[4][4];       // 블록 모양 (4x4 배열)
    int color;
    int idx;
    bool isFin;
};
const int BOARD_WIDTH = 10;   // 게임 보드 가로 크기
const int BOARD_HEIGHT = 20;  // 게임 보드 세로 크기
int gameBoard[BOARD_HEIGHT][BOARD_WIDTH] = { 0 }; // 게임 보드 상태 (0: 빈 칸, 1: 블록)
COLORREF blockColors[BOARD_HEIGHT][BOARD_WIDTH] = { 0 };
int newBlockFlags[BOARD_HEIGHT][BOARD_WIDTH] = { 0 }; // 새로 추가된 블록 추적
int space = 0; // 스페이스 누른 횟수
int score = 0; // 점수
void DrawBoard(HDC hdc); // 보드 그림
void isCollision(HDC hdc, HWND hWnd); // 충돌했는가
void DrawUI(HDC hdc); // 점수와 다음블록, 홀드UI
void ClearFullRows(); // 줄 지우기
bool isRowFull(int row); // 행이 가득 찼는가
Block nextNewBlock();

std::vector<Block> blockTemplates = {
    // T 블록
    {
        3, 0,
        {

            { 0, 1, 0, 0 },
            { 1, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        RGB(128, 0, 128), // 보라색
        0,0
    },
    // I 블록
    {
        3, 0,
        {
            { 0, 0, 0, 0 },
            { 1, 1, 1, 1 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        RGB(0, 255, 255), // 하늘색
        1,0
    },
    // O 블록
    {
        3, 0,
        {
            { 1, 1, 0, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        RGB(255, 255, 0), // 노란색
        2,0
    },
    // L 블록
    {
        3, 0,
        {
            { 0, 0, 1, 0 },
            { 1, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        RGB(255, 165, 0), // 주황색
        3,0
    },
    // J 블록
    {
        3, 0,
        {
            { 1, 0, 0, 0 },
            { 1, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        RGB(0, 0, 255), // 파란색
        4,0
    },
    // Z 블록
    {
        3, 0,
        {
            { 1, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        RGB(255, 0, 0), // 빨간색
        5,0
    },
    // S 블록
    {
        3, 0,
        {
            { 0, 1, 1, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        RGB(0, 255, 0), // 초록색
        6,0
    }
};


int GenerateRandomBlock() {
    srand(static_cast<unsigned int>(time(0))); // 랜덤 시드 설정
    int randomIndex = rand() % blockTemplates.size(); // 0 ~ blockTemplates.size()-1 중 랜덤
    return randomIndex;
}

Block currentBlock = { 3, 0,{{ 0, 1, 0, 0 },{ 1, 1, 1, 0 },{ 0, 0, 0, 0 },{ 0, 0, 0, 0 }},RGB(128, 0, 128),0,0 }; //보라색
Block nextBlock = nextNewBlock();
Block holdBlock = { 0 }; // 홀드 초기화
Block nextNewBlock() { // 다음 블럭 랜덤으로 생성
    int idx = GenerateRandomBlock();
    nextBlock.idx = idx;
    nextBlock = blockTemplates[idx];
    return nextBlock;
}

Block block = currentBlock; // 기본은 현재 블럭
void SpawnNewBlock() { // 블럭 생성
    currentBlock.idx = nextBlock.idx; // 다음 블럭의 정보 저장 후 다음블럭은 새로 할당
    currentBlock = blockTemplates[nextBlock.idx];
    currentBlock.x = 3; // 초기 X 위치 (보드 중앙)
    currentBlock.y = 0; // 초기 Y 위치 (상단)
    nextBlock = nextNewBlock();
}

//void InitializeGame() {
//    SpawnNewBlock(); // 첫 번째 블록 생성
//}
//
// 게임 보드를 초기화하는 함수
void InitializeBoard() {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            gameBoard[y][x] = 0;
        }
    }
}
void DrawBlock(HDC hdc) {
    const int CELL_SIZE = 30;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentBlock.shape[y][x] == 1) {
                RECT cell = {
                    (currentBlock.x + x) * CELL_SIZE,
                    (currentBlock.y + y) * CELL_SIZE,
                    (currentBlock.x + x + 1) * CELL_SIZE,
                    (currentBlock.y + y + 1) * CELL_SIZE
                };
                HBRUSH brush = CreateSolidBrush(currentBlock.color); // 블록 색상
                FillRect(hdc, &cell, brush);
                DeleteObject(brush);
                // 테두리
                FrameRect(hdc, &cell, (HBRUSH)GetStockObject(GRAY_BRUSH));
            }
        }
    }
}
void DrawBoard(HDC hdc) { // 보드 그림
    const int CELL_SIZE = 30;
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            RECT cell = {
                x * CELL_SIZE,
                y * CELL_SIZE,
                (x + 1) * CELL_SIZE,
                (y + 1) * CELL_SIZE
            };

            HBRUSH brush;
            if (gameBoard[y][x] == 0) {
                // 빈 칸 색상
                brush = CreateSolidBrush(RGB(240, 240, 240));
            }
            else {
                // 저장된 블록 색상
                brush = CreateSolidBrush(blockColors[y][x]);
            }

            FillRect(hdc, &cell, brush);
            DeleteObject(brush);
            FrameRect(hdc, &cell, (HBRUSH)GetStockObject(GRAY_BRUSH));
        }
    }
    DrawUI(hdc);
}
bool CheckCollision(int dx, int dy) { // 충돌 검사
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (currentBlock.shape[y][x] == 1) {
                int newX = currentBlock.x + x + dx;
                int newY = currentBlock.y + y + dy;
                // 경계 충돌 체크
                if (newX < 0 || newX >= BOARD_WIDTH || newY >= BOARD_HEIGHT) {
                    return true;
                }
                // 기존 블록과 충돌 체크
                if (gameBoard[newY][newX] == 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

void UIBox(RECT Rect, HDC hdc) {
    const int CELL_SIZE = 30; // 셀 크기
    // 다음 블록 그리기
    const int previewStartX = (Rect.left + Rect.right) / 2 - (2 * CELL_SIZE);
    const int previewStartY = (Rect.top + Rect.bottom) / 2 - (2 * CELL_SIZE);
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (nextBlock.shape[y][x] == 1) {
                RECT cell = {
                    previewStartX + x * CELL_SIZE,
                    previewStartY + y * CELL_SIZE,

                    previewStartX + (x + 1) * CELL_SIZE,
                    previewStartY + (y + 1) * CELL_SIZE
                };

                HBRUSH blockBrush = CreateSolidBrush(nextBlock.color);
                FillRect(hdc, &cell, blockBrush);
                DeleteObject(blockBrush);

                FrameRect(hdc, &cell, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
        }
    }
}void UIBox_Hold(RECT Rect, HDC hdc, bool ishold) {
    const int CELL_SIZE = 30; // 셀 크기
    // 홀드 블록 그리기
    const int previewStartX = (Rect.left + Rect.right) / 2 - (2 * CELL_SIZE);
    const int previewStartY = (Rect.top + Rect.bottom) / 2 - (2 * CELL_SIZE);

    if (ishold) { // 홀드 중이면 block 바꾸기
        block = holdBlock;
        ishold = false;
    }
    else {
        return; //홀드된게 없으면
    }
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (block.shape[y][x] == 1) {
                RECT cell = {
                    previewStartX + x * CELL_SIZE,
                    previewStartY + y * CELL_SIZE,

                    previewStartX + (x + 1) * CELL_SIZE,
                    previewStartY + (y + 1) * CELL_SIZE
                };

                HBRUSH blockBrush = CreateSolidBrush(block.color);
                FillRect(hdc, &cell, blockBrush);
                DeleteObject(blockBrush);

                FrameRect(hdc, &cell, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
        }
    }
}

void DrawUI(HDC hdc) {
    const int BOARD_WIDTH_PX = BOARD_WIDTH * 50; // 게임 보드 가로 크기 (픽셀)
    const int CELL_SIZE = 30; // 셀 크기

    // 점수판 출력
    SetTextColor(hdc, RGB(0, 0, 0));  // 텍스트 색상
    SetBkMode(hdc, TRANSPARENT);      // 배경 투명 처리
    RECT scoreRect = { 350, 10, BOARD_WIDTH_PX, 40 }; // 점수판 위치

    // 점수판 배경을 지우기 (흰색으로 덮기)
    HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255)); // 흰색 배경
    FillRect(hdc, &scoreRect, bgBrush);
    DeleteObject(bgBrush); // 브러시 삭제

    // 점수판 텍스트
    char scoreText[50]; // 점수를 저장할 문자열 버퍼
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score); // 안전한 문자열 포맷팅
    DrawTextA(hdc, scoreText, -1, &scoreRect, DT_LEFT); // ASCII 문자열 출력

    // 다음 블록 표시 영역
    RECT nextBlockRect = { BOARD_WIDTH_PX - 150, 100, BOARD_WIDTH_PX - 20, 220 };
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255)); // 흰색 배경
    FillRect(hdc, &nextBlockRect, brush);
    DeleteObject(brush);

    // "Next Block" 텍스트 출력
    RECT textRect = { BOARD_WIDTH_PX - 150 , 70, BOARD_WIDTH_PX - 20, 100 };
    DrawText(hdc, L"Next Block", -1, &textRect, DT_LEFT);

    UIBox(nextBlockRect, hdc);

    // 홀드할 블록
    RECT holdBlockRect = { BOARD_WIDTH_PX - 150, 200, BOARD_WIDTH_PX - 20, 420 };
    HBRUSH brush1 = CreateSolidBrush(RGB(255, 255, 255)); // 흰색 배경
    FillRect(hdc, &holdBlockRect, brush1);
    DeleteObject(brush1);

    // "Hold" 텍스트 출력
    RECT hlodRect = { BOARD_WIDTH_PX - 150 , 180, BOARD_WIDTH_PX - 20, 200 };
    DrawText(hdc, L"Hold", -1, &hlodRect, DT_LEFT);

    UIBox_Hold(holdBlockRect, hdc, ishold);
}

void ClearFullRows() {
    for (int row = BOARD_HEIGHT - 1; row >= 0; row--) {
        if (isRowFull(row)) {
            // 해당 행을 삭제하고 위의 행들을 아래로 내림
            for (int r = row; r > 0; r--) {
                for (int col = 0; col < BOARD_WIDTH; col++) {
                    gameBoard[r][col] = gameBoard[r - 1][col];  // 위 행 복사
                    blockColors[r][col] = blockColors[r - 1][col]; // 색상도 복사
                }
            }
            // 맨 위 행은 초기화 (비워줌)
            for (int col = 0; col < BOARD_WIDTH; col++) {
                gameBoard[0][col] = 0;
                blockColors[0][col] = RGB(240, 240, 240); // 기본 색상
            }
            score += 100; // 한 행 지워지면 점수 증가
            // 지운 행 아래의 행들도 체크하기 위해 현재 row를 다시 검사
            row++;
        }
    }

}
//행이 가득찼으면 true, 아니면 false
bool isRowFull(int row) {
    for (int col = 0; col < BOARD_WIDTH; col++) {
        if (gameBoard[row][col] == 0) {
            return false; // 비어있는 칸이 있다면 가득 찬 게 아님
        }
    }
    return true; // 모두 차 있으면 true 반환
}
void DoubleBuffering(HWND hwnd) { // 더블버퍼링
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);

    memDC = CreateCompatibleDC(hdc);
    hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

    ReleaseDC(hwnd, hdc);
}
void DrawDoubleBuffering(HDC hdc, RECT rect) { // 더블버퍼링 그리기
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);
}
void CleanupDoubleBuffering() {
    SelectObject(memDC, oldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(memDC);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool isFin = 0;

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rect;
        GetClientRect(hWnd, &rect);

        // 백 버퍼에 그림 그리기
        DrawDoubleBuffering(memDC, rect);

        // 백 버퍼 -> 화면 복사
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);
        if (gameOver) { // 게임 오버 시
            // 배경을 흰색으로 채움
            RECT rect = { 0, 0, 1000, 1000 };
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);

            // "Game Over" 텍스트 출력
            SetTextColor(hdc, RGB(255, 0, 0));  // 빨간색
            SetBkMode(hdc, TRANSPARENT);        // 배경 투명
            HFONT hFont = CreateFont(50, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

            DrawText(hdc, L"Game Over", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
        }
        DrawBoard(hdc);
        isCollision(hdc, hWnd);
        DrawBlock(hdc);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        CleanupDoubleBuffering();
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        InitializeBoard(); // 게임 보드 초기화
        SetTimer(hWnd, TIMER_ID, timer_interval, NULL); // 타이머 시작
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_LEFT:
            if (!CheckCollision(-1, 0)) currentBlock.x--;
            else { // 더이상 왼쪽으로 못갈때
                currentBlock.isFin = 1;
                holdCnt = 0;
            }
            break;
        case VK_RIGHT:
            if (!CheckCollision(1, 0)) currentBlock.x++;
            else { // 더이상 오른쪽으로 못갈때
                currentBlock.isFin = 1;
                holdCnt = 0;
            }
            break;
        case VK_DOWN:
            if (!CheckCollision(0, 1)) currentBlock.y++;
            else { // 더이상 아래로 못갈때
                currentBlock.isFin = 1;
                holdCnt = 0;
            }
            break;
        case VK_SHIFT: // 홀드
            ishold = true;

            if (holdCnt >= 1) { // 홀드카운트가 1이상이면 홀드 못함
                return 0;
            }
            if (holdBlock.x == 0) { // 홀드가 비어있으면 현재 블럭 받아오고 새로 생성해주기
                Block tmp;
                tmp = holdBlock;
                holdBlock = currentBlock;
                currentBlock = tmp;
                block = holdBlock;
                SpawnNewBlock();
            }
            else { // 홀드 채워져있으면 서로 바꿔주기
                Block tmp;
                tmp = holdBlock;
                holdBlock = currentBlock;
                currentBlock = tmp;
                block = holdBlock;

            }
            currentBlock.x = 3; //홀드 누르면 위치 초기화
            currentBlock.y = 0;
            holdCnt += 1;
            break;
        case VK_SPACE: // 회전
            int index = currentBlock.idx;
            switch (index) {
            case 0:// T블럭
            {
                if (space == 0 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(1, 0)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 1 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 2 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(-1, 0)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 3 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, -1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space = 0;
                }
                break;
            }
            case 1: //I블럭
            {
                if (space == 0 && currentBlock.x < 7 && currentBlock.x >= 0 && !CheckCollision(1, 0)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 1 && currentBlock.x < 7 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space = 0;
                }
                break;
            }
            case 2:// 2x2 블럭
                break;
            case 3: // L 블럭
            {
                if (space == 0 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 1 && currentBlock.x < 7 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 2 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 3 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space = 0;
                }
                break;
            }
            case 4: //J블럭
            {
                if (space == 0 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 1 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 2 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 3 && currentBlock.x < 7 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space = 0;
                }
                break;
            }
            case 5: //z 블럭
            {
                if (space == 0 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{{0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 1 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space = 0;
                }
                break;
            }
            case 6: //s블럭
            {
                if (space == 0 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space += 1;
                }
                else if (space == 1 && currentBlock.x < 8 && currentBlock.x >= 0 && !CheckCollision(0, 1)) {
                    currentBlock = { currentBlock.x,currentBlock.y ,{ {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },currentBlock.color,currentBlock.idx };
                    space = 0;
                }
                break;
            }
            }
            break;

        }

        InvalidateRect(hWnd, NULL, false);
        break;
    case WM_TIMER:
        if (wParam == TIMER_ID) { // 지정된 타이머 ID 확인
            if (score >= 3000) { // 3000점 넘으면 0.5초마다 내려옴
                timer_interval = 500;
                SetTimer(hWnd, TIMER_ID, timer_interval, NULL);
            }
            if (!CheckCollision(0, 1)) {
                currentBlock.y++; // 블록을 아래로 이동
            }
            else { // 블록이 충돌했을 때
                currentBlock.isFin = 1;
                holdCnt = 0;
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                isCollision(hdc, hWnd); // 블록을 보드에 고정

            }
            InvalidateRect(hWnd, NULL, false); // 화면 갱신 요청
        }
        if (gameBoard[1][3] == 1) {  // 생성되는곳에 블럭이 있으면 게임 오버
            gameOver = true;
            KillTimer(hWnd, TIMER_ID);
            InvalidateRect(hWnd, NULL, TRUE);  // 화면 다시 그리기 요청
        }
        break;

    default:
        ClearFullRows();
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void isCollision(HDC hdc, HWND hWnd) { //충돌했을때 고정시키기
    if (CheckCollision(0, 1) && currentBlock.isFin == 1) {
        const int CELL_SIZE = 30;
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (currentBlock.shape[y][x] == 1) {
                    int newX = currentBlock.x + x;
                    int newY = currentBlock.y + y;
                    // 게임 보드에 블록 위치 저장
                    gameBoard[newY][newX] = 1;
                    // 블록 색상 저장
                    blockColors[newY][newX] = currentBlock.color;
                    // 새로 추가된 블록 위치 기록
                    newBlockFlags[newY][newX] = 1;
                    // 블록을 화면에 그리기
                    RECT cell = {
                        newX * CELL_SIZE,
                        newY * CELL_SIZE,
                        (newX + 1) * CELL_SIZE,
                        (newY + 1) * CELL_SIZE
                    };
                    // 셀 내부 색상
                    HBRUSH brush = CreateSolidBrush(currentBlock.color);
                    FillRect(hdc, &cell, brush);
                    DeleteObject(brush);
                    // 셀 테두리
                    FrameRect(hdc, &cell, (HBRUSH)GetStockObject(GRAY_BRUSH));
                }
            }
        }
        space = 0;
        currentBlock.isFin = 0;
        if (gameBoard[1][3] != 1) {
            SpawnNewBlock(); // 새 블록 생성
        }
    }
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}