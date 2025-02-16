//#include "framework.h"
//#include "WinAPI.h"
//#include <vector>
//#include <ctime>
//#include <cstdlib>
//
//#define MAX_LOADSTRING 100
//
//// 전역 변수:
//HINSTANCE hInst;                                // 현재 인스턴스입니다.
//WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
//WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
//
//// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//
//struct Block {
//    int x, y;              // 현재 위치
//    int shape[4][4];       // 블록 모양 (4x4 배열)
//};
//
//Block currentBlock;         // 현재 움직이는 블록
//const int BOARD_WIDTH = 10; // 게임 보드 가로 크기
//const int BOARD_HEIGHT = 20; // 게임 보드 세로 크기
//int gameBoard[BOARD_HEIGHT][BOARD_WIDTH] = { 0 }; // 게임 보드 상태 (0: 빈 칸, 1: 블록)
//
//// 블록 템플릿 정의
//std::vector<Block> blockTemplates = {
//    { 3, 0, { {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // T 블록
//    { 3, 0, { {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // I 블록
//    { 3, 0, { {1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // O 블록
//    { 3, 0, { {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // L 블록
//    { 3, 0, { {1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // J 블록
//    { 3, 0, { {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // Z 블록
//    { 3, 0, { {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }  // S 블록
//};
//
//// 게임 보드를 초기화하는 함수
//void InitializeBoard() {
//    for (int y = 0; y < BOARD_HEIGHT; y++) {
//        for (int x = 0; x < BOARD_WIDTH; x++) {
//            gameBoard[y][x] = 0;
//        }
//    }
//}
//
//// 랜덤 블록 생성
//Block GenerateRandomBlock() {
//    srand(static_cast<unsigned int>(time(0))); // 랜덤 시드 설정
//    int randomIndex = rand() % blockTemplates.size();
//    return blockTemplates[randomIndex];
//}
//
//// 새로운 블록 생성
//void SpawnNewBlock() {
//    currentBlock = GenerateRandomBlock();
//}
//
//// 블록 회전 함수
//void RotateBlock() {
//    int temp[4][4];
//
//    // 90도 회전 (시계 방향)
//    for (int y = 0; y < 4; y++) {
//        for (int x = 0; x < 4; x++) {
//            temp[x][3 - y] = currentBlock.shape[y][x];
//        }
//    }
//
//    // 회전 후 충돌 검사
//    Block rotatedBlock = currentBlock; // 기존 블록 복사
//    memcpy(rotatedBlock.shape, temp, sizeof(temp));
//
//    if (!CheckCollision(0, 0)) { // 회전 후 충돌이 없으면 적용
//        memcpy(currentBlock.shape, temp, sizeof(temp));
//    }
//}
//
//// 충돌 감지
//bool CheckCollision(int dx, int dy) {
//    for (int y = 0; y < 4; y++) {
//        for (int x = 0; x < 4; x++) {
//            if (currentBlock.shape[y][x] == 1) {
//                int newX = currentBlock.x + x + dx;
//                int newY = currentBlock.y + y + dy;
//
//                // 경계 체크
//                if (newX < 0 || newX >= BOARD_WIDTH || newY >= BOARD_HEIGHT) {
//                    return true;
//                }
//
//                // 기존 블록과 충돌 체크
//                if (gameBoard[newY][newX] == 1) {
//                    return true;
//                }
//            }
//        }
//    }
//    return false;
//}
//
//// 블록을 그리는 함수
//void DrawBlock(HDC hdc) {
//    const int CELL_SIZE = 30;
//
//    for (int y = 0; y < 4; y++) {
//        for (int x = 0; x < 4; x++) {
//            if (currentBlock.shape[y][x] == 1) {
//                RECT cell = {
//                    (currentBlock.x + x) * CELL_SIZE,
//                    (currentBlock.y + y) * CELL_SIZE,
//                    (currentBlock.x + x + 1) * CELL_SIZE,
//                    (currentBlock.y + y + 1) * CELL_SIZE
//                };
//                HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0)); // 블록 색상
//                FillRect(hdc, &cell, brush);
//                DeleteObject(brush);
//
//                // 테두리
//                FrameRect(hdc, &cell, (HBRUSH)GetStockObject(BLACK_BRUSH));
//            }
//        }
//    }
//}
//
//// 보드를 그리는 함수
//void DrawBoard(HDC hdc) {
//    const int CELL_SIZE = 30; // 각 셀의 크기
//
//    for (int y = 0; y < BOARD_HEIGHT; y++) {
//        for (int x = 0; x < BOARD_WIDTH; x++) {
//            RECT cell = {
//                x * CELL_SIZE,
//                y * CELL_SIZE,
//                (x + 1) * CELL_SIZE,
//                (y + 1) * CELL_SIZE
//            };
//            HBRUSH brush;
//
//            if (gameBoard[y][x] == 0) {
//                brush = CreateSolidBrush(RGB(240, 240, 240)); // 빈 칸 색상
//            }
//            else {
//                brush = CreateSolidBrush(RGB(0, 0, 255)); // 블록 색상
//            }
//
//            FillRect(hdc, &cell, brush);
//            DeleteObject(brush);
//        }
//    }
//}
//
//// 메시지 처리 함수
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
//    switch (message) {
//    case WM_CREATE:
//        InitializeBoard();
//        SpawnNewBlock();
//        break;
//
//    case WM_PAINT: {
//        PAINTSTRUCT ps;
//        HDC hdc = BeginPaint(hWnd, &ps);
//        DrawBoard(hdc);
//        DrawBlock(hdc);
//        EndPaint(hWnd, &ps);
//    } break;
//
//    case WM_KEYDOWN:
//        switch (wParam) {
//        case VK_LEFT:
//            if (!CheckCollision(-1, 0)) currentBlock.x--;
//            break;
//        case VK_RIGHT:
//            if (!CheckCollision(1, 0)) currentBlock.x++;
//            break;
//        case VK_DOWN:
//            if (!CheckCollision(0, 1)) currentBlock.y++;
//            break;
//        case VK_SPACE:
//            RotateBlock();
//            break;
//        }
//        InvalidateRect(hWnd, NULL, TRUE);
//        break;
//
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}
//
//// 엔트리 포인트
//int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//
//    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
//    LoadStringW(hInstance, IDC_WINAPI, szWindowClass, MAX_LOADSTRING);
//    MyRegisterClass(hInstance);
//
//    if (!InitInstance(hInstance, nCmdShow)) {
//        return FALSE;
//    }
//
//    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPI));
//    MSG msg;
//
//    while (GetMessage(&msg, nullptr, 0, 0)) {
//        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//    }
//
//    return (int)msg.wParam;
//}
