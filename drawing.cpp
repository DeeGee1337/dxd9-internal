#include "includes.h"

#define PI 3.14159265 //Defining what PI is. PI is a Circle

extern Hack* hack;

void draw_fill_rect(int x, int y, int w, int h, D3DCOLOR color) {
	D3DRECT rect = { x,y,x + w,y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void draw_fill_rect_transparent(int x, int y, int w, int h, D3DCOLOR color) {
	//D3DRECT rect = { x,y,x + w,y + h };
	
	if (!hack->LineL)
		D3DXCreateLine(pDevice, &hack->LineL);

	D3DXVECTOR2 DotPosition[2];

	DotPosition[0] = D3DXVECTOR2(x + w / 2, y);
	DotPosition[1] = D3DXVECTOR2(x + w / 2, y + h);

	hack->LineL->SetAntialias(false);
	hack->LineL->SetWidth(w);
	hack->LineL->Draw(DotPosition, 2, color);
	
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


	//draw_line(tl, tr, thickness, color);
	//draw_line(bl, br, thickness, color);
	//draw_line(tl, bl, thickness, color);
	//draw_line(tr, br, thickness, color);

	//outlines
	
	//unten
	draw_line(bl.x, tl.y-1, br.x, tr.y-1, thickness, D3DCOLOR_ARGB(255, 0, 0, 0));
	draw_line(bl.x, tl.y, br.x, tr.y, thickness, color);
	draw_line(bl.x, tl.y+1, br.x, tr.y+1, thickness, D3DCOLOR_ARGB(255, 0, 0, 0));
	//oben
	draw_line(bl.x, bl.y - 1, br.x, br.y - 1, thickness, D3DCOLOR_ARGB(255, 0, 0, 0));
	draw_line(bl, br, thickness, color);
	draw_line(bl.x, bl.y + 1, br.x, br.y + 1, thickness, D3DCOLOR_ARGB(255, 0, 0, 0));
	//links
	draw_line(bl.x - 1, tl.y, bl.x - 1, bl.y, thickness, D3DCOLOR_ARGB(255, 0, 0, 0));
	draw_line(bl.x, tl.y, bl.x, bl.y, thickness, color);
	draw_line(bl.x + 1, tl.y, bl.x + 1, bl.y, thickness, D3DCOLOR_ARGB(255, 0, 0, 0));
	//rechts
	draw_line(br.x - 1, tr.y, br.x - 1, br.y, thickness, D3DCOLOR_ARGB(255, 0, 0, 0));
	draw_line(br.x, tr.y, br.x, br.y, thickness, color);
	draw_line(br.x + 1, tr.y, br.x + 1, br.y, thickness, D3DCOLOR_ARGB(255, 0, 0, 0));
}

void draw_text(const char* text, float x, float y, D3DCOLOR color) {
	RECT rect;

	if (!hack->FontF)
		D3DXCreateFont(pDevice, 14, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &hack->FontF);

	SetRect(&rect, x + 1, y + 1, x + 1, y + 1);
	hack->FontF->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));

	SetRect(&rect, x, y, x, y);
	hack->FontF->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, color);
}