// step3_player_control.cpp -- Bomberman step 3: a red ball you move with the arrow keys (entry point)
//

//#include "stdafx.h"
#include <conio.h>
#include <graphics.h>
#include <stdlib.h>
#include <time.h>
#include <winuser.h>

int x = 0, y = 0;
int w = 850, h = 850;
int r = 20;
int i = 0, j = 0;
int speed = 5;
bool Box[17][17];


void KeyControl()
{
	char userkey = _getch();        
	if (GetAsyncKeyState(VK_UP) && ((x - 25) %  100 ==0 ))   // Up
	{
		y -= speed;
		if (y <= r) y = r;
	}
	if (GetAsyncKeyState(VK_DOWN) && ((x - 25) % 100 ==0 )) // Down
	{
		y += speed;
		if (y >= w-r) y = w-r;
	}
	if (GetAsyncKeyState(VK_LEFT) && ((y - 25) % 100 == 0) ) // Left
	{
		x -= speed;
		if (x <= r) x = r;
	}
	if (GetAsyncKeyState(VK_RIGHT) && ((y - 25) % 100 == 0) ) // Right
	{
		x += speed;
		if (x >= w-r) x = w-r;
	}
}

void DrawMap()
{
	for (i = 0; i < 17; i++)
	{
		for (j = 0; j < 17; j++)
		{
			if (i % 2 != 0 && j % 2 != 0)
			{
				int left = (i)* 50;
				int top = (j)* 50;
				int right = left + 50;
				int bottom = top + 50;
				setfillcolor(RGB(190, 190, 190));
				setlinecolor(RGB(150, 150, 150));
				fillrectangle(left, top, right, bottom);
			}
			else if (Box[i][j])
			{
				int left_b = (i)* 50 + 5;
				int top_b = (j)* 50 + 5;
				int right_b = left_b + 40;
				int bottom_b = top_b + 40;
				setlinecolor(RGB(50, 50, 50));
				setfillcolor(RGB(50, 50, 50));
				fillrectangle(left_b, top_b, right_b, bottom_b);
			}
		}
	}
}

void CreateBox() // generate the bricks
{
	for (i = 0; i < 17; i++)
	{
		for (j = 0; j < 17; j++)
		{
			if ((i+j) % 2 != 0 && rand() % 3 == 0)
				Box[i][j] = true;
			else Box[i][j] = false;
		}
	}
}

int main()
{
	initgraph(w, h);	// Create screen
	
	setbkcolor(RGB(0, 120, 0));
	cleardevice();
	
	srand((unsigned int)time(NULL));

	x = rand() % 9 * 100 + r + 5; //  random position
	y = rand() % 9 * 100 + r + 5;

	CreateBox();

	BeginBatchDraw();
	while (true)
	{

		DrawMap();  //  create map

		setfillcolor(RED);

		if (_kbhit())
			KeyControl();

		fillcircle(x, y, r);

		FlushBatchDraw();
		Sleep(100);
		cleardevice();
	}

	_getch();				
	closegraph();			
	return 0;
}

