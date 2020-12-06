#include "includes.h"

#define PI 3.14159265 //Defining what PI is. PI is a Circle

extern Hack* hack;

void draw_fill_rect(int x, int y, int w, int h, D3DCOLOR color) {
	D3DRECT rect = { x,y,x + w,y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void draw_line(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color) {

	if (!hack->LineL)
		D3DXCreateLine(pDevice, &hack->LineL);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	hack->LineL->SetWidth(thickness);
	hack->LineL->Draw(Line, 2, color);
}

void draw_line(Vec2 src, Vec2 dst, int thickness, D3DCOLOR color)
{
	draw_line(src.x, src.y, dst.x, dst.y, thickness, color);
}

void draw_crosshair(int x, int y, int w, int h, D3DCOLOR color)
{
	D3DRECT rec = { x, y, x + w, y + h };
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
}

void draw_esp_box_2d(Vec2 top, Vec2 bot, int thickness, D3DCOLOR color)
{
	int height = ABS(top.y - bot.y);
	Vec2 tl, tr;
	Vec2 bl, br;

	tl.x = top.x - height / 4;
	tr.x = top.x + height / 4;
	tl.y = tr.y = top.y;

	bl.x = bot.x - height / 4;
	br.x = bot.x + height / 4;
	bl.y = br.y = bot.y;


	draw_line(tl, tr, thickness, color);
	draw_line(bl, br, thickness, color);
	draw_line(tl, bl, thickness, color);
	draw_line(tr, br, thickness, color);
}

//void draw_esp_cornerbox_2d(Vec2 top, Vec2 bot, int thickness, D3DCOLOR inlinecolor, D3DCOLOR OutLine)
//{
//	int H = ABS(top.y - bot.y);
//	int W = ABS(top.x - top.x);
//	int X = top.x - H / 4;
//	int Y = top.y;
//
//	float lineW = (100 / 5); // hardcoded
//	float lineH = (100 / 6); // hardcoded
//	float lineT = 1;
//
//	//outline;
//	draw_line(X - lineT, Y - lineT, X + lineW, Y - lineT, thickness, OutLine); //top left
//	draw_line(X - lineT, Y - lineT, X - lineT, Y + lineH, thickness, OutLine);
//	draw_line(X - lineT, Y + H - lineH, X - lineT, Y + H + lineT, thickness, OutLine); //bot left
//	draw_line(X - lineT, Y + H + lineT, X + lineW, Y + H + lineT, thickness, OutLine);
//	draw_line(X + W - lineW, Y - lineT, X + W + lineT, Y - lineT, thickness, OutLine); // top right
//	draw_line(X + W + lineT, Y - lineT, X + W + lineT, Y + lineH, thickness, OutLine);
//	draw_line(X + W + lineT, Y + H - lineH, X + W + lineT, Y + H + lineT, thickness, OutLine); // bot right
//	draw_line(X + W - lineW, Y + H + lineT, X + W + lineT, Y + H + lineT, thickness, OutLine);
//
//	//inline
//	draw_line(X, Y, X, Y + lineH, thickness, inlinecolor);//top left
//	draw_line(X, Y, X + lineW, Y, thickness, inlinecolor);
//	draw_line(X + W - lineW, Y, X + W, Y, thickness, inlinecolor); //top right
//	draw_line(X + W, Y, X + W, Y + lineH, thickness, inlinecolor);
//	draw_line(X, Y + H - lineH, X, Y + H, thickness, inlinecolor); //bot left
//	draw_line(X, Y + H, X + lineW, Y + H, thickness, inlinecolor);
//	draw_line(X + W - lineW, Y + H, X + W, Y + H, thickness, inlinecolor);//bot right
//	draw_line(X + W, Y + H - lineH, X + W, Y + H, thickness, inlinecolor);
//}