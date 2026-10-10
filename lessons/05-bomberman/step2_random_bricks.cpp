// step2_random_bricks.cpp -- Bomberman step 2: scatter breakable bricks at random

//#include "stdafx.h"
#include <conio.h>
#include <graphics.h>
#include <stdlib.h>
#include <time.h>

int x = 0, y = 0;
int w = 850, h = 850;
int r = 20;
int i = 0, j = 0;
int speed = 5;
bool Box[17][17];

void KeyControl()
{
  // reserve for keyboard control
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

void CreateBox()
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

	DrawMap();  //  create map
		
	_getch();				
	closegraph();			
	return 0;
}

