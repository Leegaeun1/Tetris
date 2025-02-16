//#include "framework.h"
//#include "WinAPI.h"
//#include <vector>
//#include <ctime>
//#include <cstdlib>
//
//#define MAX_LOADSTRING 100
//
//// ���� ����:
//HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
//WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
//WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
//
//// �� �ڵ� ��⿡ ���Ե� �Լ��� ������ �����մϴ�:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//
//struct Block {
//    int x, y;              // ���� ��ġ
//    int shape[4][4];       // ��� ��� (4x4 �迭)
//};
//
//Block currentBlock;         // ���� �����̴� ���
//const int BOARD_WIDTH = 10; // ���� ���� ���� ũ��
//const int BOARD_HEIGHT = 20; // ���� ���� ���� ũ��
//int gameBoard[BOARD_HEIGHT][BOARD_WIDTH] = { 0 }; // ���� ���� ���� (0: �� ĭ, 1: ���)
//
//// ��� ���ø� ����
//std::vector<Block> blockTemplates = {
//    { 3, 0, { {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // T ���
//    { 3, 0, { {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // I ���
//    { 3, 0, { {1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // O ���
//    { 3, 0, { {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // L ���
//    { 3, 0, { {1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // J ���
//    { 3, 0, { {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }, // Z ���
//    { 3, 0, { {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } }  // S ���
//};
//
//// ���� ���带 �ʱ�ȭ�ϴ� �Լ�
//void InitializeBoard() {
//    for (int y = 0; y < BOARD_HEIGHT; y++) {
//        for (int x = 0; x < BOARD_WIDTH; x++) {
//            gameBoard[y][x] = 0;
//        }
//    }
//}
//
//// ���� ��� ����
//Block GenerateRandomBlock() {
//    srand(static_cast<unsigned int>(time(0))); // ���� �õ� ����
//    int randomIndex = rand() % blockTemplates.size();
//    return blockTemplates[randomIndex];
//}
//
//// ���ο� ��� ����
//void SpawnNewBlock() {
//    currentBlock = GenerateRandomBlock();
//}
//
//// ��� ȸ�� �Լ�
//void RotateBlock() {
//    int temp[4][4];
//
//    // 90�� ȸ�� (�ð� ����)
//    for (int y = 0; y < 4; y++) {
//        for (int x = 0; x < 4; x++) {
//            temp[x][3 - y] = currentBlock.shape[y][x];
//        }
//    }
//
//    // ȸ�� �� �浹 �˻�
//    Block rotatedBlock = currentBlock; // ���� ��� ����
//    memcpy(rotatedBlock.shape, temp, sizeof(temp));
//
//    if (!CheckCollision(0, 0)) { // ȸ�� �� �浹�� ������ ����
//        memcpy(currentBlock.shape, temp, sizeof(temp));
//    }
//}
//
//// �浹 ����
//bool CheckCollision(int dx, int dy) {
//    for (int y = 0; y < 4; y++) {
//        for (int x = 0; x < 4; x++) {
//            if (currentBlock.shape[y][x] == 1) {
//                int newX = currentBlock.x + x + dx;
//                int newY = currentBlock.y + y + dy;
//
//                // ��� üũ
//                if (newX < 0 || newX >= BOARD_WIDTH || newY >= BOARD_HEIGHT) {
//                    return true;
//                }
//
//                // ���� ��ϰ� �浹 üũ
//                if (gameBoard[newY][newX] == 1) {
//                    return true;
//                }
//            }
//        }
//    }
//    return false;
//}
//
//// ����� �׸��� �Լ�
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
//                HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0)); // ��� ����
//                FillRect(hdc, &cell, brush);
//                DeleteObject(brush);
//
//                // �׵θ�
//                FrameRect(hdc, &cell, (HBRUSH)GetStockObject(BLACK_BRUSH));
//            }
//        }
//    }
//}
//
//// ���带 �׸��� �Լ�
//void DrawBoard(HDC hdc) {
//    const int CELL_SIZE = 30; // �� ���� ũ��
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
//                brush = CreateSolidBrush(RGB(240, 240, 240)); // �� ĭ ����
//            }
//            else {
//                brush = CreateSolidBrush(RGB(0, 0, 255)); // ��� ����
//            }
//
//            FillRect(hdc, &cell, brush);
//            DeleteObject(brush);
//        }
//    }
//}
//
//// �޽��� ó�� �Լ�
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
//// ��Ʈ�� ����Ʈ
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
