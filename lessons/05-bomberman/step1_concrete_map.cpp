// step1_concrete_map.cpp -- Bomberman step 1: draw the permanent concrete blocks
// Compile: g++ step1_concrete_map.cpp -o step1.exe -leasyx -lgdi32 -lole32 -luuid

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

void DrawMap()
{
	for (i = 0; i < 17; i++) // 850 / 50 = 17
	{
		for (j = 0; j < 17; j++)
		{
			if (i % 2 != 0 && j % 2 != 0) // even number of both row and column, then we place a concrete block
			{
				int left = (i)* 50;
				int top = (j)* 50;
				int right = left + 50;
				int bottom = top + 50;
				setfillcolor(RGB(190, 190, 190)); // grey color
				setlinecolor(RGB(150, 150, 150));
				fillrectangle(left, top, right, bottom);
			}
		}
	}
}

void CreateBox()
{
	// reserve for the creation of bricks 
}

int main()
{
	initgraph(w, h);	// Create screen, 850 * 850
	setbkcolor(RGB(0, 120, 0));
	cleardevice();

	DrawMap();  //  create map with concrete blocks

	CreateBox(); //  create bricks
	
	_getch();				
	closegraph();			
	return 0;
}

