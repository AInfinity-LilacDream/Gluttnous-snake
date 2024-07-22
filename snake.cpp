#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <string>
#include <wchar.h>
#include <thread>

#include "widgets.h"

using namespace std;

void LoadInterface();
void ShowWelcomeMenu();				// 开始界面
void DrawBeginningAnimation();
void DrawSnakePart(int);			// 画蛇身
void DrawFood();					// 画食物
void LoadFood();
void Flash();
void SnakeMove();
void DrawGameFrame();
void DrawEndWindow();

bool CheckFood();
bool SnakeCrash();

int interfaceWidth;
int interfaceHeight;
int screenWidth;
int screenHeight;

int flag = 0;

int snakeX[1000000], snakeY[1000000];
int snakeLength;
int foodX, foodY, lastX, lastY;
int score;

int speed = 100;
int direction = 4;					// 1 2 3 4 上 下 左 右

int main() {
	LoadInterface();

	ShowWelcomeMenu();

	label:
	DrawBeginningAnimation();

	ExMessage msg;
	while(1) {
		if (peekmessage(&msg, EX_KEY) && !msg.prevdown) {
			switch (msg.vkcode) {
			case 0x57: if (direction != 2) direction = 1; break;
			case 0x41: if (direction != 4) direction = 3; break;
			case 0x53: if (direction != 1) direction = 2; break;
			case 0x44: if (direction != 3) direction = 4; break;
			case VK_ESCAPE: flag = 1; break;
			}
			if (flag == 1) break;
		}
		if (flag == 1) break;
		lastX = snakeX[snakeLength], lastY = snakeY[snakeLength];
		SnakeMove();
		if ((snakeX[1] == foodX) && (snakeY[1] == foodY)) {
			score++;
			LoadFood();
			snakeLength++;
			snakeX[snakeLength] = lastX, snakeY[snakeLength] = lastY;
		}
		if (SnakeCrash()) break;
		Sleep(speed);
		BeginBatchDraw();
		DrawGameFrame();
		EndBatchDraw();
	}
	
	if (flag == 1) {
		closegraph();
		return 0;
	}
	cleardevice();
	RECT rect = { 0, 0, interfaceWidth, interfaceHeight };
	TCHAR s1[10009] = _T("最终得分：");
	TCHAR s3[29] = _T("\n按下Enter键重新开始，按下ESC键以退出。");
	TCHAR s2[10009];
	swprintf_s(s2, _T("%d"), score);
	_tcscat_s(s1, s2);
	_tcscat_s(s1, s3);
	drawtext(s1, &rect, DT_CENTER);
	ExMessage msg2;
	while (1) {
		getmessage(&msg2, EX_KEY);
		if (msg2.vkcode == VK_ESCAPE) break;
		else if (msg2.vkcode == VK_RETURN) {
			score = 0;
			direction = 4;
			goto label;
		}
	}
	closegraph();
	return 0;
}

void LoadInterface() {
	interfaceWidth = 640;
	interfaceHeight = 480;
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	initgraph(interfaceWidth, interfaceHeight);
	HWND hwnd = GetHWnd();
	SetWindowPos(hwnd, NULL, (screenWidth - interfaceWidth) / 2, (screenHeight - interfaceHeight) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void ShowWelcomeMenu() {
	cleardevice();
	RECT rect = { 0, 0, interfaceWidth, interfaceHeight };
	drawtext(_T("按任意键开始游戏"), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	getmessage(EX_KEY);
}

void DrawBeginningAnimation() {
	cleardevice();

	// 16 * 16像素为一个单位坐标
	// 40 * 30
	snakeLength = 2;
	snakeX[1] = 19, snakeY[1] = 15;
	snakeX[2] = 18, snakeY[2] = 15;

	DrawSnakePart(2);
	Sleep(500);
	DrawSnakePart(1);
	Sleep(500);
	LoadFood();
	DrawFood();
	Sleep(1000);
	Flash();
}

void DrawSnakePart(int id) {
	solidrectangle(snakeX[id] * 16 + 2, snakeY[id] * 16 + 2, (snakeX[id] + 1) * 16 - 2, (snakeY[id] + 1) * 16 - 2);
	if (id < snakeLength) {
		if ((snakeX[id + 1] == snakeX[id]) && (snakeY[id + 1] == snakeY[id] + 1))
			solidrectangle(snakeX[id] * 16 + 2, (snakeY[id] + 1) * 16 - 2, (snakeX[id] + 1) * 16 - 2, (snakeY[id] + 1) * 16);
		else if ((snakeX[id + 1] == snakeX[id]) && (snakeY[id + 1] == snakeY[id] - 1))
			solidrectangle(snakeX[id] * 16 + 2, snakeY[id] * 16, (snakeX[id] + 1) * 16 - 2, snakeY[id] * 16 + 2);
		else if ((snakeX[id + 1] == snakeX[id] + 1) && (snakeY[id + 1] == snakeY[id]))
			solidrectangle((snakeX[id] + 1) * 16 - 2, snakeY[id] * 16 + 2, (snakeX[id] + 1) * 16, (snakeY[id] + 1) * 16 - 2);
		else if ((snakeX[id + 1] == snakeX[id] - 1) && (snakeY[id + 1] == snakeY[id]))
			solidrectangle(snakeX[id] * 16, snakeY[id] * 16 + 2, snakeX[id] * 16 + 2, (snakeY[id] + 1) * 16 - 2);
	}
	if (id > 1) {
		if ((snakeX[id - 1] == snakeX[id]) && (snakeY[id - 1] == snakeY[id] + 1))
			solidrectangle(snakeX[id] * 16 + 2, (snakeY[id] + 1) * 16 - 2, (snakeX[id] + 1) * 16 - 2, (snakeY[id] + 1) * 16);
		else if ((snakeX[id - 1] == snakeX[id]) && (snakeY[id - 1] == snakeY[id] - 1))
			solidrectangle(snakeX[id] * 16 + 2, snakeY[id] * 16, (snakeX[id] + 1) * 16 - 2, snakeY[id] * 16 + 2);
		else if ((snakeX[id - 1] == snakeX[id] + 1) && (snakeY[id - 1] == snakeY[id]))
			solidrectangle((snakeX[id] + 1) * 16 - 2, snakeY[id] * 16 + 2, (snakeX[id] + 1) * 16, (snakeY[id] + 1) * 16 - 2);
		else if ((snakeX[id - 1] == snakeX[id] - 1) && (snakeY[id - 1] == snakeY[id]))
			solidrectangle(snakeX[id] * 16, snakeY[id] * 16 + 2, snakeX[id] * 16 + 2, (snakeY[id] + 1) * 16 - 2);
	}
}

void DrawFood() {
	setfillcolor(RED);
	solidrectangle(foodX * 16 + 2, foodY * 16 + 2, (foodX + 1) * 16 - 2, (foodY + 1) * 16 - 2);
	setfillcolor(WHITE);
}

void LoadFood() {
	srand(time(0));
	do {
		foodX = rand() % 40;
		foodY = rand() % 30;
	} while (!CheckFood());
}

void Flash() {
	for (int i = 1; i <= 3; ++i) {
		cleardevice();
		Sleep(100);
		BeginBatchDraw();
		for (int i = 1; i <= snakeLength; ++i) DrawSnakePart(i);
		DrawFood();
		EndBatchDraw();
		Sleep(100);
	}
}

void SnakeMove() {
	switch (direction) {
	case 1:
		for (int i = snakeLength - 1; i >= 1; --i)
			snakeX[i + 1] = snakeX[i], snakeY[i + 1] = snakeY[i];
		if (snakeY[1] == 0) snakeY[1] = 29;
		else snakeY[1]--;
		break;
	case 2:
		for (int i = snakeLength - 1; i >= 1; --i)
			snakeX[i + 1] = snakeX[i], snakeY[i + 1] = snakeY[i];
		if (snakeY[1] == 29) snakeY[1] = 0;
		else snakeY[1]++;
		break;
	case 3:
		for (int i = snakeLength - 1; i >= 1; --i)
			snakeX[i + 1] = snakeX[i], snakeY[i + 1] = snakeY[i];
		if (snakeX[1] == 0) snakeX[1] = 39;
		else snakeX[1]--;
		break;
	case 4:
		for (int i = snakeLength - 1; i >= 1; --i)
			snakeX[i + 1] = snakeX[i], snakeY[i + 1] = snakeY[i];
		if (snakeX[1] == 39) snakeX[1] = 0;
		else snakeX[1]++;
		break;
	}
}

void DrawGameFrame() {
	cleardevice();
	for (int i = 1; i <= snakeLength; ++i) DrawSnakePart(i);
	DrawFood();
}

bool SnakeCrash() {
	for (int i = 2; i <= snakeLength; ++i)
		if (snakeX[i] == snakeX[1] && snakeY[i] == snakeY[1]) return true;
	return false;
}

bool CheckFood() {
	for (int i = 1; i <= snakeLength; ++i)
		if (foodX == snakeX[i] && foodY == snakeY[i]) return false;
	return true;
}