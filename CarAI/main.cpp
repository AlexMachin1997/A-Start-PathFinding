//#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN            

#include <windows.h>
#include <time.h>
#include <SDKDDKVer.h>
#include "CPathFinder.h"

// Windows Header Files:

// Column RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <sstream>
#include <string>
#include <list>
#include <iostream>

using namespace std;

// Window properties
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 400

// Function pointers 
void initGame();
void updateGame();
void drawGame(HDC &hdc);

// Global player properties
int playerSize = 20;
int playerx;
int playery;

// Global game goal properties
int GameGoalX = 2;
int GameGoalY = 7;

// Global game start properties
int GameStartX = 1;
int GameStartY = 1;

//LIST declared in CPathfinder.h
STACK directions;

//The map is 10 by 10 pixels (mapSize is defiend in the CPathFinder)
int map[mapsize][mapsize]= {
	{2,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,1,0,0,0,1,1},
	{1,0,1,0,1,0,0,1,0,1},
	{1,0,1,0,1,1,1,0,0,1},
	{1,0,0,0,0,0,1,1,0,1},
	{1,1,1,1,1,0,1,1,0,1}, 
	{1,0,0,0,1,0,1,1,0,1},
	{1,0,3,0,1,0,1,1,0,1},
	{1,0,0,0,0,0,1,1,0,1},
	{1,1,1,1,1,1,1,1,1,1}
};

// Windows message callback
LRESULT CALLBACK WindowProc(HWND hwnd, UINT   msg, WPARAM wParam, LPARAM lParam)
{
	//create some pens to use for drawing
	static HPEN BluePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	static HPEN RedPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	static HPEN OldPen = NULL;

	//create a solid brush
	static HBRUSH RedBrush = CreateSolidBrush(RGB(200, 128, 0));
	static HBRUSH YellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	static HBRUSH LightBlue = CreateSolidBrush(RGB(200, 200, 255));
	static HBRUSH ltGreenBrush = CreateSolidBrush(RGB(50, 255, 255));

	static HBRUSH OldBrush = NULL;

	//these hold the dimensions of the client window area
	static int cxClient, cyClient;


	//used to create the back buffer
	static HDC		hdcBackBuffer;
	static HBITMAP	hBitmap;
	static HBITMAP	hOldBitmap;

	switch (msg)
	{

		//A WM_CREATE msg is sent when your application window is first
		//created
	case WM_CREATE:
	{
		//to get get the size of the client window first we need  to create
		//a RECT and then ask Windows to fill in our RECT structure with
		//the client window size. Then we assign to cxClient and cyClient 
		//accordingly
		RECT rect;

		GetClientRect(hwnd, &rect);

		cxClient = rect.right;
		cyClient = rect.bottom;

		//seed random number generator
		srand((unsigned)time(NULL));

		//---------------create a surface for us to render to(backbuffer)

		//create a memory device context
		hdcBackBuffer = CreateCompatibleDC(NULL);

		//get the DC for the front buffer
		HDC hdc = GetDC(hwnd);

		hBitmap = CreateCompatibleBitmap(hdc,
			cxClient,
			cyClient);


		//select the bitmap into the memory device context
		hOldBitmap = (HBITMAP)SelectObject(hdcBackBuffer, hBitmap);

		//don't forget to release the DC
		ReleaseDC(hwnd, hdc);

		initGame();
	}

	break;

	case WM_KEYUP:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			PostQuitMessage(0);
		}

		break;
		}
	}

	case WM_PAINT:
	{

		PAINTSTRUCT ps;

		BeginPaint(hwnd, &ps);

		//fill our backbuffer with white
		BitBlt(hdcBackBuffer,
			0,
			0,
			cxClient,
			cyClient,
			NULL,
			NULL,
			NULL,
			WHITENESS);

		(HBRUSH)SelectObject(hdcBackBuffer, ltGreenBrush);

		Rectangle(hdcBackBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		drawGame(hdcBackBuffer);
		
		//now blit backbuffer to front
		BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdcBackBuffer, 0, 0, SRCCOPY);

		EndPaint(hwnd, &ps);

		//delay a little
		Sleep(20);

	}

	break;

	//has the user resized the client area?
	case WM_SIZE:
	{
		//if so we need to update our variables so that any drawing
		//we do using cxClient and cyClient is scaled accordingly
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		//now to resize the backbuffer accordingly. First select
		//the old bitmap back into the DC
		SelectObject(hdcBackBuffer, hOldBitmap);

		//don't forget to do this or you will get resource leaks
		DeleteObject(hBitmap);

		//get the DC for the application
		HDC hdc = GetDC(hwnd);

		//create another bitmap of the same size and mode
		//as the application
		hBitmap = CreateCompatibleBitmap(hdc,
			cxClient,
			cyClient);

		ReleaseDC(hwnd, hdc);

		//select the new bitmap into the DC
		SelectObject(hdcBackBuffer, hBitmap);

	}

	break;

	case WM_DESTROY:
	{
		//delete the pens        
		DeleteObject(BluePen);
		DeleteObject(OldPen);

		//and the brushes
		DeleteObject(RedBrush);
		DeleteObject(OldBrush);

		//clean up our backbuffer objects
		SelectObject(hdcBackBuffer, hOldBitmap);

		DeleteDC(hdcBackBuffer);
		DeleteObject(hBitmap);

		// kill the application, this sends a WM_QUIT message  
		PostQuitMessage(0);
	}

	break;

	}//end switch

	 //this is where all the messages not specifically handled by our 
	 //winproc are sent to be processed
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Windows main entrty point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR szCmdLine, int iCmdShow)
{
	//handle to our window
	HWND						hWnd;

	//our window class structure
	WNDCLASSEX     winclass;

	// first fill in the window class stucture
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = NULL;
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = "GA_Car";
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//register the window class
	if (!RegisterClassEx(&winclass))
	{
		MessageBox(NULL, "Registration Failed!", "Error", 0);

		//exit the application
		return 0;
	}

	//create the window and assign its ID to hwnd    
	hWnd = CreateWindowEx(NULL,                 // extended style
		"GA_Car",  // window class name
		"GA Car",  // window caption
		WS_OVERLAPPEDWINDOW,  // window style
		0,                    // initial x position
		0,                    // initial y position
		WINDOW_WIDTH,         // initial x size
		WINDOW_HEIGHT,        // initial y size
		NULL,                 // parent window handle
		NULL,                 // window menu handle
		hInstance,            // program instance handle
		NULL);                // creation parameters

							  //make sure the window creation has gone OK
	if (!hWnd)
	{
		MessageBox(NULL, "CreateWindowEx Failed!", "Error!", 0);
	}

	//make the window visible
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	// Enter the message loop
	bool bDone = false;

	MSG msg;

	while (!bDone)
	{

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				// Stop loop if it's a quit message
				bDone = true;
			}

			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		//this will call WM_PAINT which will render our scene
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);

		//*** your game loop goes here ***//
		updateGame();

	}//end while

	UnregisterClass("Backbuffer", winclass.hInstance);

	return msg.wParam;
}

void initGame()
{
	//Sets the starting coordinates for the player
	playerx = 0;
	playery = 0 ;

	//The following two lines of codes are to create the goal
	CPathFinder newPath(&map[0][0], GameStartX, GameStartY, GameGoalX, GameGoalY);
	directions = newPath.doIt();
}

void drawGame(HDC &hdcBackBuffer)
{
	// Creates instances of some brushes
	static HBRUSH LightBlue = CreateSolidBrush(RGB(200, 200, 255));
	static HBRUSH DarkBlue = CreateSolidBrush(RGB(100, 100, 255));
	static HBRUSH Red = CreateSolidBrush(RGB(255, 0, 0));
	static HBRUSH Green = CreateSolidBrush(RGB(0, 255, 0));

	// Drawing rows
	for (int Row = 0; Row < mapsize; Row++)
	{
		// Drawing columns
		for (int Column = 0; Column < mapsize; Column++)
		{
			// Rest of the grid (If its not 1,2 or 3)
			SelectObject(hdcBackBuffer, DarkBlue);

			// 1 - Wall colour 
			if (map[Row][Column] == 1) 
				SelectObject(hdcBackBuffer, LightBlue);

			// 2 - start colour
			if (map[Row][Column] == 2)
				SelectObject(hdcBackBuffer, Red);

			// 3 - End colour
			if (map[Row][Column] == 3)
				SelectObject(hdcBackBuffer, Green);

			// Draws the map grid 
			Rectangle(hdcBackBuffer, Column * playerSize, Row * playerSize, Column * playerSize + playerSize, Row * playerSize + playerSize);
		}
	}

	// Drawing the player (Small Ellipse on the map)
	SelectObject(hdcBackBuffer, LightBlue);
	Ellipse(hdcBackBuffer, playerx, playery, playerx + playerSize, playery + playerSize);
}

bool touchedGoal = false;

void updateGame()
{	
	if (GameGoalX == playerx && GameGoalY == playery) {
		
		// Checks to see if the stacks not equal to on
		if (directions.size() != 0) {
			cNode temp = directions.front();
			GameGoalX = temp.x * playerSize;
			GameGoalY = temp.y * playerSize;
			directions.pop_front();
		} else {
			if (touchedGoal == false) {
				touchedGoal = true;
				Beep(500, 500);
				MessageBox(NULL, "Success", "You have reached your destination", 0);
				exit(42);
			}
		}
	}

	// X cordinates check
	if (playerx > GameGoalX) 
		playerx--;

	if (playerx < GameGoalX) 
		playerx++;

	// Y cordinates check
	if (playery > GameGoalY) 
		playery--; 

	if (playery < GameGoalY) 
		playery++;
}
