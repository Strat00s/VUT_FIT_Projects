#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700


// Global variable
HINSTANCE hInst;
UINT  MessageCount = 0;
UINT  Count = 0;
int posX = 0;
int posY = 0;

//color struct
typedef struct {
	int red;
	int green;
	int blue;
} rgb_space;

//moveable rectangle struct
typedef struct mov_rect {
	RECT sides;
	rgb_space color;
	int x_offset;
	int y_offset;
} moveable_rect;

typedef struct {
	RECT body_down;
	RECT body_top;
	RECT crossH;
	RECT crossV;
	RECT cross_cricle;
	RECT wheel;
	RECT wheelC;
	RECT siren;
	POINT front_window[3];
	int x_offset;
	int y_offset;
	int dir;
	bool color;
} simple_car;

/*---(BRUSH)---*/
#define MAX_PAINTBRUSH_SIZE 100
#define MIN_PAINTBRUSH_SIZE 1
int pb_size = 10;	//default size
int last_posX = 0;
int last_posY = 0;
bool can_paint = false;	//paint flag
bool can_delete = false;//delete flag
bool square = false;	//shape switch
rgb_space cur_color = { 0, 0, 0 }; //current color storage

/*---(TOOLBAR)---*/
#define TOOLBAR_WIDTH 210
#define FOOTER_HEIGHT 30
#define PALETTE_SQUARE 50
#define PALETTE_SPACE 15
int canvas_posX = 0;
int canvas_posY = 0;
bool resize = false;	//clear after toolbar
bool update = true;		//update flag
bool on_slider = false;	//slider check
moveable_rect outline = { {0, 0, 0, 0}, {0, 0, 0}, 0, 0 };		//selector
moveable_rect slider = { {0, 0, 0, 0}, {0, 0, 0}, 14, 135 };	//y_offset = max width
//struct containing predefined palette colors
rgb_space colors[12] = { {0, 255, 255}, {0, 0, 255  }, {138, 0, 186},	//cian blue purple
							   {255, 172, 0}, {255, 255, 0}, {0, 255, 0  }, //orange yellow green
							   {255, 0, 255}, {255, 0, 0  }, {150, 85, 0 },	//magenta red brown
							   {255, 255, 255}, {128, 128, 128}, {0, 0, 0} };	//white grey black

/*---(CAR)---*/
simple_car ambulance = { {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {{0, 0}, {0, 0}, {0, 0}}, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 1, true};
bool show_ambulance = false;
UINT_PTR IDT_TIMER1;

//Tools macros
#define CREATE_TOOLS(style, width, pcolor, bcolor) HPEN pen = CreatePen(style, width, RGB(colors[pcolor].red, colors[pcolor].green, colors[pcolor].blue));\
												   SelectObject(hDC, pen);\
												   SelectObject(hDC, GetStockObject(DC_BRUSH));\
												   SetDCBrushColor(hDC, RGB(colors[bcolor].red, colors[bcolor].green, colors[bcolor].blue))

RECT window_side = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};	//current window size
RECT old_window_side = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };



// Function prototypes.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
void drawStrokes(HWND hWnd, HDC hDC, PAINTSTRUCT ps, int posX, int posY, POINT cursorPosition, int selected);
void paintPosition(HWND hWnd, HDC hDC, PAINTSTRUCT ps);

void mousePos(POINT cursorPosition, HWND hWnd);
void drawPalette(HWND hWnd, HDC hDC, PAINTSTRUCT ps);
void drawToolbar(HDC hDC, int selected_color);
void drawOutline(HWND hWnd, HDC hDC, PAINTSTRUCT ps, moveable_rect outline);
void removeOldStuff(HWND hWnd, HDC hDC, PAINTSTRUCT ps);
void drawAmbulance(HDC hDC, int selected_color);
void setAmbulance();

// Application entry point. This is the same as main() in standart C.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	/*---(my stuff)---*/
	/*---(end of my stuff)---*/

	MSG msg;
	BOOL bRet;
	WNDCLASS wcx;          // register class
	HWND hWnd;

	hInst = hInstance;     // Save the application-instance handle.
		// Fill in the window class structure with parameters that describe the main window.

    wcx.style = CS_BYTEALIGNCLIENT;
    //wcx.style = CS_HREDRAW | CS_VREDRAW;			// redraw if size changes
	wcx.lpfnWndProc = (WNDPROC)MainWndProc;          // points to window procedure
	wcx.cbClsExtra = 0;                               // no extra class memory
	wcx.cbWndExtra = 0;                               // no extra window memory
	wcx.hInstance = hInstance;                        // handle to instance
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);      // predefined app. icon
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);        // predefined arrow
	wcx.hbrBackground = GetStockObject(WHITE_BRUSH);  // white background brush
	wcx.lpszMenuName = (LPCSTR)"MainMenu";          // name of menu resource
	wcx.lpszClassName = (LPCSTR)"MainWClass";        // name of window class

	// Register the window class.

	if (!RegisterClass(&wcx)) return FALSE;

	// create window of registered class

	hWnd = CreateWindow(
		"MainWClass",			// name of window class
		"ITU",					// title-bar string
		WS_OVERLAPPEDWINDOW,	// top-level window
		200,					// default horizontal position
		25,						// default vertical position
		WINDOW_WIDTH,			// default width
		WINDOW_HEIGHT,			// default height
		(HWND)NULL,				// no owner window
		(HMENU)NULL,			// use class menu
		hInstance,				// handle to application instance
		(LPVOID)NULL);			// no window-creation data
	if (!hWnd) return FALSE;

	// Show the window and send a WM_PAINT message to the window procedure.
	// Record the current cursor position.

	ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);

	// loop of message processing
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}


LRESULT CALLBACK MainWndProc(
	HWND hWnd,        // handle to window
	UINT uMsg,        // message identifier
	WPARAM wParam,    // first message parameter
	LPARAM lParam)    // second message parameter
{
	HDC         hDC;
	PAINTSTRUCT ps;
	POINT cursorPosition;

	// init cursor position 
	GetCursorPos(&cursorPosition);
	ScreenToClient(hWnd, &cursorPosition);

	switch (uMsg)
	{

	//resizing stuff
	case WM_SIZE:
		GetClientRect(hWnd, &window_side);
		
		//2 diferent types of movement change. First one using OffsetRect(), second one using stored values (2nd is much better)
		//update outline, also default settings
		outline.sides.right = window_side.right - 7;
		outline.sides.left = outline.sides.right - 64;
		outline.sides.top = 7;
		outline.sides.bottom = outline.sides.top + 64;
		outline.y_offset = 0;
		outline.x_offset = 2;

		//update slider on resize
		slider.sides.left = window_side.right - TOOLBAR_WIDTH + 30;	//30 - spacing form sides
		slider.sides.right = slider.sides.left + 15;
		slider.sides.top = PALETTE_SPACE * 5 + PALETTE_SQUARE * 4 + 50; //50 - spacing
		slider.sides.bottom = slider.sides.top + 25;

		update = true;	//update toolbar
		resize = true;
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	//timer
	case WM_TIMER:
		ambulance.color = !ambulance.color;
		update = true;
		InvalidateRect(hWnd, NULL, FALSE);
		break;

		// key input
	case WM_KEYDOWN:	// more virtual codes can be found here: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
		switch (wParam) {

			//clear canvas
			case VK_DELETE:
				update = true;
				InvalidateRect(hWnd, NULL, TRUE);
				break;

			//move slider
			case VK_ADD:
				if (slider.x_offset < slider.y_offset) slider.x_offset++;
				update = true;
				InvalidateRect(hWnd, NULL, FALSE);
				break;
			case VK_SUBTRACT:
				if (slider.x_offset > 0) slider.x_offset--;
				update = true;
				InvalidateRect(hWnd, NULL, FALSE);
				break;

			case VK_SPACE:

				//set ambualnce
				//ambulance.x_offset = (window_side.right - TOOLBAR_WIDTH)/2;
				//ambulance.y_offset = (window_side.bottom - FOOTER_HEIGHT)/2;

				show_ambulance = !show_ambulance;	//toggle ambulance
				update = true;	//update
				if (!show_ambulance) {
					InvalidateRect(hWnd, NULL, FALSE);	//update but keep mabulance
					KillTimer(hWnd, IDT_TIMER1);
				}
				else {
					SetTimer(hWnd, IDT_TIMER1, 500, (TIMERPROC)NULL);
					InvalidateRect(hWnd, NULL, TRUE);	//erase everything
				}
				break;

			//move left
			case VK_LEFT:
				//if ambulance mode
				if (show_ambulance) {
					ambulance.x_offset -= 10;
					InvalidateRect(hWnd, NULL, TRUE);
				}
				//if draw mode
				else if (outline.x_offset > 0) {
					outline.x_offset--;	//move left in colors
					OffsetRect(&outline.sides, (PALETTE_SQUARE + PALETTE_SPACE) * (-1), 0);	//move outline
					update = true;	//update toolbar
					InvalidateRect(hWnd, NULL, FALSE);
				}
				//cur_color = colors[(3-outline.y_offset)*3 + outline.x_offset];	//select color
				break;

			//move right
			case VK_RIGHT:
				//if ambulance mode
				if (show_ambulance) {
					ambulance.x_offset += 10;
					InvalidateRect(hWnd, NULL, TRUE);
				}
				//if draw mode
				else if (outline.x_offset < 2) {
					outline.x_offset++;	//move right in colors
					OffsetRect(&outline.sides, (PALETTE_SQUARE + PALETTE_SPACE) * (1), 0);
					update = true;	//update toolbar
					InvalidateRect(hWnd, NULL, FALSE);
				}
				break;

			//move up
			case VK_UP:
				//if ambulance mode
				if (show_ambulance) {
					ambulance.y_offset -= 10;
					InvalidateRect(hWnd, NULL, TRUE);
				}
				//if draw mode
				else if (outline.y_offset > 0) {
					outline.y_offset--;	//move down in color struc, but up in reality
					OffsetRect(&outline.sides, 0, (PALETTE_SQUARE + PALETTE_SPACE) * (-1));
					update = true;	//update toolbar
					InvalidateRect(hWnd, NULL, FALSE);
				}
				break;

			//move down
			case VK_DOWN:
				//if ambulance mode
				if (show_ambulance) {
					ambulance.y_offset += 10;
					InvalidateRect(hWnd, NULL, TRUE);
				}
				//if draw mode
				else if (outline.y_offset < 3) {
					outline.y_offset++;	//move up in color struc, but down in reality
					OffsetRect(&outline.sides, 0, (PALETTE_SQUARE + PALETTE_SPACE) * (1));
					update = true;	//update toolbar
					InvalidateRect(hWnd, NULL, FALSE);
				}
				break;

			//change shape
			case VK_NUMPAD0:
				square = !square;
				update = true;
				InvalidateRect(hWnd, NULL, FALSE);
				break;
		}
		break;

	//get cursor position 
	case WM_MOUSEMOVE:

		//disable mouse when using ambulance (just because)
		if (show_ambulance) break;

		//while L mbutton is pressed
		if (wParam & MK_LBUTTON) {

			//if we are on slider
			if (on_slider){

				slider.x_offset = slider.x_offset + (posX - last_posX);	//offset = mouse movement

				//limit slider movement
				if (slider.x_offset < 0) slider.x_offset = 0;
				if (slider.x_offset > slider.y_offset) slider.x_offset = slider.y_offset;


				update = true;	 //update toolbar

				//save position
				last_posX = posX;
				last_posY = posY;
			}
			else {
				can_paint = true;
			}
		}

		//while right
		else if (wParam & MK_RBUTTON) {
			can_delete = true;
			can_paint = true;
		}
		mousePos(cursorPosition, hWnd);
		break;

	//if L mbutton is pressed
	case WM_LBUTTONDOWN:

		//check if we are on slider "hitbox"
		if (posX <= slider.sides.right + slider.x_offset && posX >= slider.sides.left + slider.x_offset\
		 && posY <= slider.sides.bottom && posY >= slider.sides.top) {
			on_slider = true;
		}
		//otherwise L can paint
		else {
			can_paint = true;
		}

		//save current position (for lines
		last_posX = posX;
		last_posY = posY;
		mousePos(cursorPosition, hWnd);
		break;

	case WM_RBUTTONDOWN:
		//save current position (for lines)
		last_posX = posX;
		last_posY = posY;

		//R button can delete
		can_delete = true;
		can_paint = true;
		mousePos(cursorPosition, hWnd);
		break;

	case WM_LBUTTONUP:
		on_slider = false;	//no longer on slider
		break;

		// paint objects
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		//get and set color
		int selected_color = (3 - outline.y_offset) * 3 + outline.x_offset;
		cur_color = colors[selected_color];

		pb_size = (slider.x_offset * 100) / slider.y_offset;	//calculate paint brush size

		//we can't paint when in ambulance mode
		if (show_ambulance) {
			drawAmbulance(hDC, selected_color);
			update = true;
		}
		//if over "canvas" and we can paint
		else if (can_paint && posX <= window_side.right - TOOLBAR_WIDTH && posY <= window_side.bottom - FOOTER_HEIGHT) {

			//if we are around the toolbar or footer, we should remove any unwanted paint (redraw the footer and toolbar)
			if ((posX >= window_side.right -  TOOLBAR_WIDTH - pb_size/2 - 5 && posX <= window_side.right - TOOLBAR_WIDTH + pb_size/2)\
			 || (posY >= window_side.bottom - FOOTER_HEIGHT - pb_size/2 - 5 && posY <= window_side.bottom -FOOTER_HEIGHT + pb_size/2)) {
				update = true;
			}
			drawStrokes(hWnd, hDC, ps, posX, posY, cursorPosition, selected_color);
		}
		can_paint = false;

		//clear resized area
		if (resize) {
			removeOldStuff(hWnd, hDC, ps);
		}
		resize = false;

		//update toolbars
		if (update) {
			drawToolbar(hDC, selected_color);	//draw color palette
			drawOutline(hWnd, hDC, ps, outline);					//move outline arround selected color TODO
			drawPalette(hWnd, hDC, ps);								//draw palette
		}
		update = false;

		paintPosition(hWnd, hDC, ps);	//paint text
		
		EndPaint(hWnd, &ps);
		DeleteDC(hDC);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void setAmbulance() {
	//down
	ambulance.body_down.left = ambulance.x_offset - (100*ambulance.dir);
	ambulance.body_down.right = ambulance.x_offset + (100*ambulance.dir);
	ambulance.body_down.top = ambulance.y_offset - 50;
	ambulance.body_down.bottom = ambulance.y_offset;
	//top
	ambulance.body_top.bottom = ambulance.body_down.top + 1;
	ambulance.body_top.top = ambulance.body_down.top - 40;
	ambulance.body_top.right = ambulance.body_down.right;
	ambulance.body_top.left = ambulance.body_down.left + 50;
	//siren
	ambulance.siren.top = ambulance.body_top.top + 10;
	ambulance.siren.bottom = ambulance.siren.top - 20;
	ambulance.siren.left = ambulance.body_top.left + 10;
	ambulance.siren.right = ambulance.siren.left + 20;
	//circle
	ambulance.cross_cricle.top = ambulance.body_top.top + 10;
	ambulance.cross_cricle.bottom = ambulance.cross_cricle.top + 60;
	ambulance.cross_cricle.right = ambulance.body_top.right - 25;
	ambulance.cross_cricle.left = ambulance.cross_cricle.right - 60;
	//crossH
	ambulance.crossH.right = ambulance.body_top.right - 30;
	ambulance.crossH.left = ambulance.crossH.right - 50;
	ambulance.crossH.top = ambulance.body_top.top + 45;
	ambulance.crossH.bottom = ambulance.crossH.top - 10;
	//crossV
	ambulance.crossV.right = ambulance.body_top.right - 50;
	ambulance.crossV.left = ambulance.crossV.right - 10;
	ambulance.crossV.top = ambulance.body_top.top + 15;
	ambulance.crossV.bottom = ambulance.crossV.top + 50;
	//window
	ambulance.front_window[0].x = ambulance.body_down.left;
	ambulance.front_window[0].y = ambulance.body_down.top;
	ambulance.front_window[1].x = ambulance.body_top.left;
	ambulance.front_window[1].y = ambulance.body_top.top;
	ambulance.front_window[2].x = ambulance.body_top.left;
	ambulance.front_window[2].y = ambulance.body_top.bottom;
	//wheel
	ambulance.wheel.top = ambulance.body_down.bottom - 25;
	ambulance.wheel.bottom = ambulance.wheel.top + 50;
	ambulance.wheel.left = ambulance.x_offset - (90*ambulance.dir);	//wheel offset
	ambulance.wheel.right = ambulance.wheel.left + 50;
	ambulance.wheelC.top = ambulance.wheel.top + 15;
	ambulance.wheelC.bottom = ambulance.wheel.bottom - 15;
	ambulance.wheelC.left = ambulance.wheel.left + 15;
	ambulance.wheelC.right = ambulance.wheel.right - 15;
}

void drawAmbulance(HDC hDC, int selected_color){

	setAmbulance();

	//siren
	if (ambulance.color) {
		selected_color = 1;
	}
	else {
		selected_color = 7;
	}
	CREATE_TOOLS(PS_NULL, 0, 11, selected_color);
	Ellipse(hDC, ambulance.siren.left, ambulance.siren.top, ambulance.siren.right, ambulance.siren.bottom);

	//body
	pen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));	//solid black pen
	SelectObject(hDC, pen);
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hDC, RGB(colors[9].red, colors[9].green, colors[9].blue));	//white body
	Rectangle(hDC, ambulance.body_down.left, ambulance.body_down.top, ambulance.body_down.right, ambulance.body_down.bottom);
	Rectangle(hDC, ambulance.body_top.left, ambulance.body_top.top, ambulance.body_top.right, ambulance.body_top.bottom);

	pen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
	SelectObject(hDC, pen);
	//circle
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hDC, RGB(colors[7].red, colors[7].green, colors[7].blue));	//cian windows
	Ellipse(hDC, ambulance.cross_cricle.left, ambulance.cross_cricle.top, ambulance.cross_cricle.right, ambulance.cross_cricle.bottom);
	//cross
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hDC, RGB(colors[9].red, colors[9].green, colors[9].blue));	//white
	Rectangle(hDC, ambulance.crossH.left, ambulance.crossH.top, ambulance.crossH.right, ambulance.crossH.bottom);
	Rectangle(hDC, ambulance.crossV.left, ambulance.crossV.top, ambulance.crossV.right, ambulance.crossV.bottom);
	//window
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hDC, RGB(colors[0].red, colors[0].green, colors[0].blue));
	Polygon(hDC, ambulance.front_window, 3);
	//wheels
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hDC, RGB(colors[11].red, colors[11].green, colors[11].blue));
	Ellipse(hDC, ambulance.wheel.left, ambulance.wheel.top, ambulance.wheel.right, ambulance.wheel.bottom);
	Ellipse(hDC, ambulance.wheel.left + 140, ambulance.wheel.top, ambulance.wheel.right + 140, ambulance.wheel.bottom);
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hDC, RGB(colors[10].red, colors[10].green, colors[10].blue));
	Ellipse(hDC, ambulance.wheelC.left, ambulance.wheelC.top, ambulance.wheelC.right, ambulance.wheelC.bottom);
	Ellipse(hDC, ambulance.wheelC.left + 140, ambulance.wheelC.top, ambulance.wheelC.right + 140, ambulance.wheelC.bottom);

	DeleteObject(pen);
}

void removeOldStuff(HWND hWnd, HDC hDC, PAINTSTRUCT ps) {
	CREATE_TOOLS(PS_NULL, 0, 0, 9);

	//clear area between old toolbar/footer and new toolbar/footer
	Rectangle(hDC, old_window_side.right - TOOLBAR_WIDTH - 1, 0, window_side.right - TOOLBAR_WIDTH, window_side.bottom);
	Rectangle(hDC, 0, old_window_side.bottom - FOOTER_HEIGHT - 1, window_side.right - TOOLBAR_WIDTH, window_side.bottom);

	DeleteObject(pen);

	old_window_side = window_side;	//update old size
}

void mousePos(POINT cursorPosition, HWND hWnd) {
	GetCursorPos(&cursorPosition);			//get mouse cursor
	ScreenToClient(hWnd, &cursorPosition);	//transform it to current window pos

	//save it
	posX = cursorPosition.x;
	posY = cursorPosition.y;
	InvalidateRect(hWnd, NULL, FALSE);		//call draw
}

void drawOutline(HWND hWnd, HDC hDC, PAINTSTRUCT ps, moveable_rect outline) {
	CREATE_TOOLS(PS_NULL, 0, 10, 9);	//no border, white fill

	Rectangle(hDC, window_side.right - (PALETTE_SPACE*3 + 9 + PALETTE_SQUARE*3), 0, window_side.right, PALETTE_SPACE*4 + 9 + PALETTE_SQUARE*4);	//clear space where we draw
	
	SetDCBrushColor(hDC, RGB(0, 0, 0));																				//change fill to black
	Rectangle(hDC, outline.sides.left-2, outline.sides.top, outline.sides.right, outline.sides.bottom+2);			//draw black square
	SetDCBrushColor(hDC, RGB(255, 255, 255));																		//change fill to white
	Rectangle(hDC, outline.sides.left-2+5, outline.sides.top+5, outline.sides.right-5, outline.sides.bottom+2-5);	//"make" thicc outline
	
	DeleteObject(pen);
}

void squareOrCircle(int pType, HDC hDC, int posX, int posY, bool type, int selected_color, int radius) {
	CREATE_TOOLS(pType, 2, 11, selected_color);

	//select and draw the shape
	if (type) {
		Rectangle(hDC, posX - radius, posY - radius, posX + radius, posY + radius);
	}
	else {
		Ellipse(hDC, posX - radius, posY - radius, posX + radius, posY + radius);
	}

	DeleteObject(pen);
}

void drawPalette(HWND hWnd, HDC hDC, PAINTSTRUCT ps) {
	int indent = PALETTE_SQUARE * 2 + 25 + PALETTE_SPACE * 3;	//3*square (but last one only half, to start at the center) + 3* padding (we don't need the last one)
	int start_height = indent + PALETTE_SQUARE + PALETTE_SPACE; //4*3
	int start_width = window_side.right - indent;
	int selected = 0;	//first color on first index

	//go through the colors list and draw 3 on each palette line
	for (int i = 0; i < 4; i++) {
		int height = start_height - i*(PALETTE_SQUARE + PALETTE_SPACE);		//move up to next line
		for (int j = 0; j < 3; j++) {
			int width = start_width + j*(PALETTE_SQUARE + PALETTE_SPACE);	//move right by 1 square + padding

			squareOrCircle(0, hDC, width, height, true, selected, 25);		//draw individual color squares
			selected++;														//select next color
		}
	}
}

void drawLine(HDC hDC, int fromx, int fromy, int tox, int toy, int width, int color) {
	//get pen
	HPEN pen = CreatePen(PS_SOLID, width, RGB(colors[color].red, colors[color].green, colors[color].blue));
	SelectObject(hDC, pen);
	//draw line
	MoveToEx(hDC, fromx, fromy, NULL);
	LineTo(hDC, tox, toy);

	DeleteObject(pen);
}

void drawSlider(HDC hDC, int posX, int posY) {
	//draw 2 lines where the slider can move (2 lines for 2 pixel width), otherwise not pretty 
	drawLine(hDC, window_side.right - TOOLBAR_WIDTH + 30, slider.sides.top + 12, window_side.right - 30, slider.sides.top + 12, 1, 11);
	drawLine(hDC, window_side.right - TOOLBAR_WIDTH + 30, slider.sides.top + 13, window_side.right - 30, slider.sides.top + 13, 1, 11);

	//draw the slider
	CREATE_TOOLS(PS_SOLID, 2, 11, 10);
	Rectangle(hDC, slider.sides.left + slider.x_offset, slider.sides.top, slider.sides.right + slider.x_offset, slider.sides.bottom);
	DeleteObject(pen);
}

void drawToolbar(HDC hDC, int selected_color) {
	CREATE_TOOLS(PS_NULL, 0, 10, 9);	//create default tools

	//clear space for drawing
	Rectangle(hDC, window_side.right - TOOLBAR_WIDTH, 0, window_side.right, window_side.bottom + 1);	//toolbar
	Rectangle(hDC, window_side.left, window_side.bottom - FOOTER_HEIGHT, window_side.right - TOOLBAR_WIDTH, window_side.bottom + 1);	//footer

	DeleteObject(pen);

	//draw footer and toolbar lines
	drawLine(hDC, window_side.right - TOOLBAR_WIDTH, 0, window_side.right - TOOLBAR_WIDTH, window_side.bottom, 2, 11);
	drawLine(hDC, 0, window_side.bottom - FOOTER_HEIGHT, window_side.right - TOOLBAR_WIDTH, window_side.bottom - FOOTER_HEIGHT,2 , 11);

	//draw slider
	drawSlider(hDC, posX, posY);

	//draw example
	int radius = (MIN_PAINTBRUSH_SIZE + pb_size / 2);	//calculate radius
	squareOrCircle(0, hDC, window_side.right - (TOOLBAR_WIDTH / 2), 450, square, selected_color, radius);
}

void drawStrokes(HWND hWnd, HDC hDC, PAINTSTRUCT ps, int posX, int posY, POINT vertices, int selected){
	CREATE_TOOLS(PS_NULL, 0, 0, 9);	//create null pen and white brush

	//change color when not deleting
	if (can_delete) {
		//SetDCBrushColor(hDC, RGB(cur_color.red, cur_color.green, cur_color.blue));
		selected = 9;
	}

	int radius = (MIN_PAINTBRUSH_SIZE + pb_size / 2);	//calculate radius

	squareOrCircle(5, hDC, posX, posY, square, selected, radius);	//draw circle or square

	//draw lines where we are not fast enough
	//has to be after drawing the shapes for some reason
	if (last_posX != posX || last_posY != posY) {
		drawLine(hDC, last_posX, last_posY, posX, posY, pb_size+1, can_delete ? 9 : selected);
	}
	last_posX = posX;
	last_posY = posY;

	can_delete = false;	//disable deletion

	DeleteObject(pen);	//delete pen
}

void paintPosition(HWND hWnd, HDC hDC, PAINTSTRUCT ps){

	char        text[100];          // buffer to store an output text
	HFONT       font;              // new large font
	HFONT       oldFont;           // saves the previous font

	font = CreateFont(25, 0, 0, 0, 0, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, 0);
	oldFont = (HFONT)SelectObject(hDC, font);

	//cursor position on "canvas"
	if (posX <= window_side.right - TOOLBAR_WIDTH && posY < window_side.bottom - FOOTER_HEIGHT) {
		canvas_posX = posX;
		canvas_posY = posY;
	}
	sprintf(text, "Position -- x:%04d, y:%04d", canvas_posX, canvas_posY);
	TextOut(hDC, 30, window_side.bottom - 25, text, (int)strlen(text));

	//"Canvas" size
	sprintf(text, "Canvas: %04d x %04d", window_side.right - TOOLBAR_WIDTH, window_side.bottom - FOOTER_HEIGHT);
	TextOut(hDC, (window_side.right - TOOLBAR_WIDTH)/2-100, window_side.bottom - 25, text, (int)strlen(text));

	//current window size
	sprintf(text, "Window size: %04d x %04d", window_side.right, window_side.bottom);
	TextOut(hDC, window_side.right - TOOLBAR_WIDTH - 260, window_side.bottom-25, text, (int)strlen(text));

	//RGB values
	sprintf(text, "R: %03d G: %03d B: %03d", cur_color.red, cur_color.green, cur_color.blue);
	TextOut(hDC, window_side.right - TOOLBAR_WIDTH + 5, 270, text, (int)strlen(text));

	//Brush size
	sprintf(text, "Size: %03dp", pb_size);
	TextOut(hDC, window_side.right - TOOLBAR_WIDTH + 50, 360, text, (int)strlen(text));

	//Delete
	sprintf(text, "Delete: Clear");
	TextOut(hDC, window_side.right - TOOLBAR_WIDTH + 40, 520, text, (int)strlen(text));

	//Num0
	sprintf(text, "Num0: Shape");
	TextOut(hDC, window_side.right - TOOLBAR_WIDTH + 40, 550, text, (int)strlen(text));

	//+-
	sprintf(text, "+/-: Size");
	TextOut(hDC, window_side.right - TOOLBAR_WIDTH + 42, 580, text, (int)strlen(text));

	//<>
	sprintf(text, "Arrows: Color");
	TextOut(hDC, window_side.right - TOOLBAR_WIDTH + 40, 610, text, (int)strlen(text));

	//space
	sprintf(text, "Space: Ambulance");
	TextOut(hDC, window_side.right - TOOLBAR_WIDTH + 20, 635, text, (int)strlen(text));
	SelectObject(hDC, oldFont);
	DeleteObject(font);
}
