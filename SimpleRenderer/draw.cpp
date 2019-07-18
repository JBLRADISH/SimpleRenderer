#include "draw.h"
#include "tool.h"

void Draw::DrawPoint(SDL_Surface* surface, int x, int y, const Color& c)
{
	*GetPixelAddress(surface, x, y) = Color2Uint32(c);
}

void Draw::DrawHLine(SDL_Surface* surface, int x1, int x2, int y, const Color& c)
{
	if (x1 > x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}

	ASM_MEMSET_DWORD(GetPixelAddress(surface, x1, y), Color2Uint32(c), x2 - x1 + 1);
}

void Draw::DrawVLine(SDL_Surface* surface, int x, int y1, int y2, const Color& c)
{
	if (y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	Uint32* start = GetPixelAddress(surface, x, y1);
	Uint32 pixel = Color2Uint32(c);
	for (int i = 0; i <= y2 - y1; i++)
	{
		*start = pixel;
		start += surface->pitch >> 2;
	}
}

//�Խ��� k=1
void Draw::DrawDLine(SDL_Surface* surface, int x1, int y1, int x2, int y2, const Color& c)
{
	int xInc = x2 > x1 ? 1 : -1;
	int yInc = y2 > y1 ? (surface->pitch >> 2) : -(surface->pitch >> 2);
	Uint32* start = GetPixelAddress(surface, x1, y1);
	Uint32 pixel = Color2Uint32(c);
	for (int i = 0; i <= x2 - x1; i++)
	{
		*start = pixel;
		start += xInc + yInc;
	}
}

//��ͨ��
void Draw::DrawGLine(SDL_Surface* surface, int x1, int y1, int x2, int y2, const Color& c)
{
	Uint32 pixel = Color2Uint32(c);
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (dx > dy)
	{
		if (x1 > x2)
		{
			int tmp = x1;
			x1 = x2;
			x2 = tmp;

			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		int yInc = y2 > y1 ? (surface->pitch >> 2) : -(surface->pitch >> 2);
		int dydouble = dy << 1;
		int dydoubleminusdxdouble = (dy - dx) << 1;
		int p = dydouble - dx;
		Uint32* start = GetPixelAddress(surface, x1, y1);
		*start = pixel;
		for (int i = 0; i < dx; i++)
		{
			start++;
			if (p < 0)
				p += dydouble;
			else
			{
				start += yInc;
				p += dydoubleminusdxdouble;
			}
			*start = pixel;
		}
	}
	else
	{
		if (y1 > y2)
		{
			int tmp = y1;
			y1 = y2;
			y2 = tmp;

			tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		int xInc = x2 > x1 ? 1 : -1;
		int dxdouble = dx << 1;
		int dxdoubleminusdydouble = (dx - dy) << 1;
		int p = dxdouble - dy;
		Uint32* start = GetPixelAddress(surface, x1, y1);
		*start = pixel;
		for (int i = 0; i < dy; i++)
		{
			start += surface->pitch >> 2;
			if (p < 0)
				p += dxdouble;
			else
			{
				start += xInc;
				p += dxdoubleminusdydouble;
			}
			*start = pixel;
		}
	}
}

void Draw::DrawLine(SDL_Surface* surface, int x1, int y1, int x2, int y2, const Color& c)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	if (dy == 0)
	{
		DrawHLine(surface, x1, x2, y1, c);
	}
	else if (dx == 0)
	{
		DrawVLine(surface, x1, y1, y2, c);
	}
	else if (dx == dy)
	{
		DrawDLine(surface, x1, y1, x2, y2, c);
	}
	else
	{
		DrawGLine(surface, x1, y1, x2, y2, c);
	}
}

bool Draw::ClipTest(float p, float q, float& u1, float& u2)
{
	float r;
	if (p < 0.0f)
	{
		r = q / p;
		if (r > u2)
		{
			return false;
		}
		else if (r > u1)
		{
			u1 = r;
		}
	}
	else if (p > 0.0f)
	{
		r = q / p;
		if (r < u1)
		{
			return false;
		}
		else if (r < u2)
		{
			u2 = r;
		}
	}
	else if (q < 0.0f)
	{
		return false;
	}
	return true;
}

void Draw::DrawClipLine(SDL_Surface* surface, Rect& rect, int x1, int y1, int x2, int y2, const Color& c)
{
	float u1 = 0.0f, u2 = 1.0f;
	float dx = x2 - x1, dy;
	int clipx1 = x1, clipy1 = y1, clipx2 = x2, clipy2 = y2;
	if (ClipTest(-dx, x1 - rect.x, u1, u2))
	{
		if (ClipTest(dx, rect.xmax() - x1, u1, u2))
		{
			dy = y2 - y1;
			if (ClipTest(-dy, y1 - rect.y, u1, u2))
			{
				if (ClipTest(dy, rect.ymax() - y1, u1, u2))
				{
					if (u2 < 1.0f)
					{
						clipx2 = x1 + u2 * dx;
						clipy2 = y1 + u2 * dy;
					}
					if (u1 > 0.0f)
					{
						clipx1 = x1 + u1 * dx;
						clipy1 = y1 + u1 * dy;
					}
					DrawLine(surface, clipx1, clipy1, clipx2, clipy2, c);
				}
			}
		}
	}
}

//ƽ��������
void Draw::DrawTopTriangle_Flat(SDL_Surface* surface, Rect& rect, float x1, float y1, float x2, float x3, float y3, const Color& c)
{
	if (x2 < x1)
	{
		float tmp = x2;
		x2 = x1;
		x1 = tmp;
	}

	float height = y3 - y1;
	float dx_left = (x3 - x1) / height;
	float dx_right = (x3 - x2) / height;

	float xs = x1;
	float xe = x2;

	int iy1, iy3;

	if (y1 < rect.y)
	{
		xs = xs + dx_left * (rect.y - y1);
		xe = xe + dx_right * (rect.y - y1);
		y1 = rect.y;
		iy1 = y1;
	}
	else 
	{
		iy1 = ceilf(y1);
		xs = xs + dx_left * (iy1 - y1);
		xe = xe + dx_right * (iy1 - y1);
	}
	if (y3 > rect.ymax())
	{
		y3 = rect.ymax();
		iy3 = y3 - 1;
	}
	else
	{
		iy3 = ceilf(y3) - 1;
	}

	Uint32* start = GetPixelAddress(surface, 0, iy1);
	Uint32 pixel = Color2Uint32(c);

	if (x1 >= rect.x && x1 <= rect.xmax() && x2 >= rect.x && x2 <= rect.xmax() && x3 >= rect.x && x3 <= rect.xmax())
	{
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			ASM_MEMSET_DWORD(start + (int)xs, pixel, (int)xe - (int)xs + 1);
			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		float left;
		float right;
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			left = xs;
			right = xe;
			xs += dx_left;
			xe += dx_right;
			if (left < rect.x)
			{
				left = rect.x;
				if (right < rect.x)
					continue;
			}
			if (right > rect.xmax())
			{
				right = rect.xmax();
				if (left > rect.xmax())
					continue;
			}
			ASM_MEMSET_DWORD(start + (int)left, pixel, (int)right - (int)left + 1);
		}
	}
}

//ƽ��������
void Draw::DrawBottomTriangle_Flat(SDL_Surface* surface, Rect& rect, float x1, float y1, float x2, float x3, float y3, const Color& c)
{
	if (x3 < x2)
	{
		float tmp = x3;
		x3 = x2;
		x2 = tmp;
	}

	float height = y3 - y1;
	float dx_left = (x2 - x1) / height;
	float dx_right = (x3 - x1) / height;

	float xs = x1;
	float xe = x1;

	int iy1, iy3;

	if (y1 < rect.y)
	{
		xs = xs + dx_left * (rect.y - y1);
		xe = xe + dx_right * (rect.y - y1);
		y1 = rect.y;
		iy1 = y1;
	}
	else
	{
		iy1 = ceilf(y1);
		xs += dx_left * (iy1 - y1);
		xe += dx_right * (iy1 - y1);
	}
	if (y3 > rect.ymax())
	{
		y3 = rect.ymax();
		iy3 = y3 - 1;
	}
	else
	{
		iy3 = ceilf(y3) - 1;
	}

	Uint32* start = GetPixelAddress(surface, 0, iy1);
	Uint32 pixel = Color2Uint32(c);

	if (x1 >= rect.x && x1 <= rect.xmax() && x2 >= rect.x && x2 <= rect.xmax() && x3 >= rect.x && x3 <= rect.xmax())
	{
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			ASM_MEMSET_DWORD(start + (int)xs, pixel, (int)xe - (int)xs + 1);
			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		float left;
		float right;
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			left = xs;
			right = xe;
			xs += dx_left;
			xe += dx_right;
			if (left < rect.x)
			{
				left = rect.x;
				if (right < rect.x)
					continue;
			}
			if (right > rect.xmax())
			{
				right = rect.xmax();
				if (left > rect.xmax())
					continue;
			}
			ASM_MEMSET_DWORD(start + (int)left, pixel, (int)right - (int)left + 1);
		}
	}
}

void Draw::DrawTriangle_Flat(SDL_Surface* surface, Rect& rect, float x1, float y1, float x2, float y2, float x3, float y3, const Color& c)
{
	if ((Equal(x1, x2) && Equal(x2, x3)) || (Equal(y1, y2) && Equal(y2, y3)))
	{
		return;
	}

	if (y2 < y1)
	{
		float tmp1 = x2;
		float tmp2 = y2;
		x2 = x1;
		y2 = y1;
		x1 = tmp1;
		y1 = tmp2;
	}

	if (y3 < y1)
	{
		float tmp1 = x3;
		float tmp2 = y3;
		x3 = x1;
		y3 = y1;
		x1 = tmp1;
		y1 = tmp2;
	}

	if (y3 < y2)
	{
		float tmp1 = x3;
		float tmp2 = y3;
		x3 = x2;
		y3 = y2;
		x2 = tmp1;
		y2 = tmp2;

	}

	if (y3<rect.y || y1>rect.ymax() || (x1 < rect.x && x2 < rect.x && x3 < rect.x) || (x1 > rect.xmax() && x2 > rect.xmax() && x3 > rect.xmax()))
	{
		return;
	}

	if (Equal(y1, y2))
	{
		DrawTopTriangle_Flat(surface, rect, x1, y1, x2, x3, y3, c);
	}
	else if (Equal(y2, y3))
	{
		DrawBottomTriangle_Flat(surface, rect, x1, y1, x2, x3, y3, c);
	}
	else
	{
		float new_x = x1 + (y2 - y1) * (x3 - x1) / (y3 - y1);
		DrawBottomTriangle_Flat(surface, rect, x1, y1, new_x, x2, y2, c);
		DrawTopTriangle_Flat(surface, rect, x2, y2, new_x, x3, y3, c);
	}
}

void Draw::DrawTopTriangle_Gouraud(SDL_Surface* surface, Rect& rect, float x1, float y1, float x2, float x3, float y3, const Color& c1, const Color& c2, const Color& c3)
{
	Color newC1 = c1;
	Color newC2 = c2;
	if (x2 < x1)
	{
		float tmp = x2;
		x2 = x1;
		x1 = tmp;
		Color tmp2 = newC2;
		newC2 = newC1;
		newC1 = tmp2;
	}

	float height = y3 - y1;
	float invHeight = 1 / (y3 - y1);
	float dx_left = (x3 - x1) * invHeight;
	float dx_right = (x3 - x2) * invHeight;
	Color dc_left = (c3 - newC1) * invHeight;
	Color dc_right = (c3 - newC2) * invHeight;

	float xs = x1;
	float xe = x2;
	Color cs = newC1;
	Color ce = newC2;

	int iy1, iy3;

	if (y1 < rect.y)
	{
		xs = xs + dx_left * (rect.y - y1);
		xe = xe + dx_right * (rect.y - y1);
		cs = cs + dc_left * (rect.y - y1);
		ce = ce + dc_right * (rect.y - y1);
		y1 = rect.y;
		iy1 = y1;
	}
	else
	{
		iy1 = ceilf(y1);
		xs = xs + dx_left * (iy1 - y1);
		xe = xe + dx_right * (iy1 - y1);
		cs = cs + dc_left * (iy1 - y1);
		ce = ce + dc_right * (iy1 - y1);
	}
	if (y3 > rect.ymax())
	{
		y3 = rect.ymax();
		iy3 = y3 - 1;
	}
	else
	{
		iy3 = ceilf(y3) - 1;
	}

	Uint32* start = GetPixelAddress(surface, 0, iy1);

	if (x1 >= rect.x && x1 <= rect.xmax() && x2 >= rect.x && x2 <= rect.xmax() && x3 >= rect.x && x3 <= rect.xmax())
	{
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			Color curC = cs;
			Uint32* curP = start + (int)xs;
			Color dc = (ce - cs) / ((int)xe - (int)xs);
			for (int j = xs; j <= xe; j++)
			{
				*curP = Color2Uint32(curC);
				curC = curC + dc;
				curP++;
			}
			xs += dx_left;
			xe += dx_right;
			cs = cs + dc_left;
			ce = ce + dc_right;
		}
	}
	else
	{
		float left;
		float right;
		Color tmpCS;
		Color tmpCe;
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			left = xs;
			right = xe;
			tmpCS = cs;
			tmpCe = ce;
			xs += dx_left;
			xe += dx_right;
			cs = cs + dc_left;
			ce = ce + dc_right;
			if (left < rect.x)
			{
				left = rect.x;
				if (right < rect.x)
					continue;
			}
			if (right > rect.xmax())
			{
				right = rect.xmax();
				if (left > rect.xmax())
					continue;
			}
			Color curC = tmpCS;
			Uint32* curP = start + (int)left;
			Color dc = (tmpCe - tmpCS) / ((int)right - (int)left);
			for (int j = left; j <= right; j++)
			{
				*curP = Color2Uint32(curC);
				curC = curC + dc;
				curP++;
			}
		}
	}
}

void Draw::DrawBottomTriangle_Gouraud(SDL_Surface* surface, Rect& rect, float x1, float y1, float x2, float x3, float y3, const Color& c1, const Color& c2, const Color& c3)
{
	Color newC2 = c2;
	Color newC3 = c3;
	if (x3 < x2)
	{
		float tmp = x3;
		x3 = x2;
		x2 = tmp;
		Color tmp2 = newC3;
		newC3 = newC2;
		newC2 = tmp2;
	}

	float height = y3 - y1;
	float invHeight = 1 / (y3 - y1);
	float dx_left = (x2 - x1) * invHeight;
	float dx_right = (x3 - x1) * invHeight;
	Color dc_left = (newC2 - c1) * invHeight;
	Color dc_right = (newC3 - c1) * invHeight;

	float xs = x1;
	float xe = x1;
	Color cs = c1;
	Color ce = c1;

	int iy1, iy3;

	if (y1 < rect.y)
	{
		xs = xs + dx_left * (rect.y - y1);
		xe = xe + dx_right * (rect.y - y1);
		cs = cs + dc_left * (rect.y - y1);
		ce = ce + dc_right * (rect.y - y1);
		y1 = rect.y;
		iy1 = y1;
	}
	else
	{
		iy1 = ceilf(y1);
		xs = xs + dx_left * (iy1 - y1);
		xe = xe + dx_right * (iy1 - y1);
		cs = cs + dc_left * (iy1 - y1);
		ce = ce + dc_right * (iy1 - y1);
	}
	if (y3 > rect.ymax())
	{
		y3 = rect.ymax();
		iy3 = y3 - 1;
	}
	else
	{
		iy3 = ceilf(y3) - 1;
	}

	Uint32* start = GetPixelAddress(surface, 0, iy1);

	if (x1 >= rect.x && x1 <= rect.xmax() && x2 >= rect.x && x2 <= rect.xmax() && x3 >= rect.x && x3 <= rect.xmax())
	{
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			Color curC = cs;
			Uint32* curP = start + (int)xs;
			Color dc = (ce - cs) / ((int)xe - (int)xs);
			for (int j = xs; j <= xe; j++)
			{
				*curP = Color2Uint32(curC);
				curC = curC + dc;
				curP++;
			}
			xs += dx_left;
			xe += dx_right;
			cs = cs + dc_left;
			ce = ce + dc_right;
		}
	}
	else
	{
		float left;
		float right;
		Color tmpCS;
		Color tmpCe;
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			left = xs;
			right = xe;
			tmpCS = cs;
			tmpCe = ce;
			xs += dx_left;
			xe += dx_right;
			cs = cs + dc_left;
			ce = ce + dc_right;
			if (left < rect.x)
			{
				left = rect.x;
				if (right < rect.x)
					continue;
			}
			if (right > rect.xmax())
			{
				right = rect.xmax();
				if (left > rect.xmax())
					continue;
			}
			Color curC = tmpCS;
			Uint32* curP = start + (int)left;
			Color dc = (tmpCe - tmpCS) / ((int)right - (int)left);
			for (int j = left; j <= right; j++)
			{
				*curP = Color2Uint32(curC);
				curC = curC + dc;
				curP++;
			}
		}
	}
}

void Draw::DrawTriangle_Gouraud(SDL_Surface* surface, Rect& rect, float x1, float y1, float x2, float y2, float x3, float y3, const Color& c1, const Color& c2, const Color& c3)
{
	if ((Equal(x1, x2) && Equal(x2, x3)) || (Equal(y1, y2) && Equal(y2, y3)))
	{
		return;
	}

	Color newC1 = c1;
	Color newC2 = c2;
	Color newC3 = c3;

	if (y2 < y1)
	{
		float tmp1 = x2;
		float tmp2 = y2;
		x2 = x1;
		y2 = y1;
		x1 = tmp1;
		y1 = tmp2;
		Color tmp3 = newC2;
		newC2 = newC1;
		newC1 = tmp3;
	}

	if (y3 < y1)
	{
		float tmp1 = x3;
		float tmp2 = y3;
		x3 = x1;
		y3 = y1;
		x1 = tmp1;
		y1 = tmp2;
		Color tmp3 = newC3;
		newC3 = newC1;
		newC1 = tmp3;
	}

	if (y3 < y2)
	{
		float tmp1 = x3;
		float tmp2 = y3;
		x3 = x2;
		y3 = y2;
		x2 = tmp1;
		y2 = tmp2;
		Color tmp3 = newC3;
		newC3 = newC2;
		newC2 = tmp3;
	}

	if (y3<rect.y || y1>rect.ymax() || (x1 < rect.x && x2 < rect.x && x3 < rect.x) || (x1 > rect.xmax() && x2 > rect.xmax() && x3 > rect.xmax()))
	{
		return;
	}

	if (Equal(y1, y2))
	{
		DrawTopTriangle_Gouraud(surface, rect, x1, y1, x2, x3, y3, newC1, newC2, newC3);
	}
	else if (Equal(y2, y3))
	{
		DrawBottomTriangle_Gouraud(surface, rect, x1, y1, x2, x3, y3, newC1, newC2, newC3);
	}
	else
	{
		float inv = (y2 - y1) / (y3 - y1);
		float new_x = x1 + (x3 - x1) * inv;
		Color newC4 = newC1 + (newC3 - newC1) * inv;
		DrawBottomTriangle_Gouraud(surface, rect, x1, y1, new_x, x2, y2, newC1, newC4, newC2);
		DrawTopTriangle_Gouraud(surface, rect, x2, y2, new_x, x3, y3, newC2, newC4, newC3);
	}
}

void Draw::DrawTopTriangle_Tex_Gouraud(SDL_Surface* surface, Rect& rect, float x1, float y1, float x2, float x3, float y3, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, const Texture& tex)
{
	Vector2 newUV1 = uv1;
	Vector2 newUV2 = uv2;
	if (x2 < x1)
	{
		float tmp = x2;
		x2 = x1;
		x1 = tmp;
		Vector2 tmp2 = newUV2;
		newUV2 = newUV1;
		newUV1 = tmp2;
	}

	float height = y3 - y1;
	float invHeight = 1 / (y3 - y1);
	float dx_left = (x3 - x1) * invHeight;
	float dx_right = (x3 - x2) * invHeight;
	Vector2 duv_left = (uv3 - newUV1) * invHeight;
	Vector2 duv_right = (uv3 - newUV2) * invHeight;

	float xs = x1;
	float xe = x2;
	Vector2 uvs = newUV1;
	Vector2 uve = newUV2;

	int iy1, iy3;

	if (y1 < rect.y)
	{
		xs = xs + dx_left * (rect.y - y1);
		xe = xe + dx_right * (rect.y - y1);
		uvs = uvs + duv_left * (rect.y - y1);
		uve = uve + duv_right * (rect.y - y1);
		y1 = rect.y;
		iy1 = y1;
	}
	else
	{
		iy1 = ceilf(y1);
		xs = xs + dx_left * (iy1 - y1);
		xe = xe + dx_right * (iy1 - y1);
		uvs = uvs + duv_left * (iy1 - y1);
		uve = uve + duv_right * (iy1 - y1);
	}
	if (y3 > rect.ymax())
	{
		y3 = rect.ymax();
		iy3 = y3 - 1;
	}
	else
	{
		iy3 = ceilf(y3) - 1;
	}

	Uint32* start = GetPixelAddress(surface, 0, iy1);

	if (x1 >= rect.x && x1 <= rect.xmax() && x2 >= rect.x && x2 <= rect.xmax() && x3 >= rect.x && x3 <= rect.xmax())
	{
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			Vector2 curUV = uvs;
			Uint32* curP = start + (int)xs;
			Vector2 duv = (uve - uvs) / ((int)xe - (int)xs);
			for (int j = xs; j <= xe; j++)
			{
				*curP = tex.GetPixel(curUV);
				curUV = curUV + duv;
				curP++;
			}
			xs += dx_left;
			xe += dx_right;
			uvs = uvs + duv_left;
			uve = uve + duv_right;
		}
	}
	else
	{
		float left;
		float right;
		Vector2 tmpUVS;
		Vector2 tmpUVE;
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			left = xs;
			right = xe;
			tmpUVS = uvs;
			tmpUVE = uve;
			xs += dx_left;
			xe += dx_right;
			uvs = uvs + duv_left;
			uve = uve + duv_right;
			if (left < rect.x)
			{
				left = rect.x;
				if (right < rect.x)
					continue;
			}
			if (right > rect.xmax())
			{
				right = rect.xmax();
				if (left > rect.xmax())
					continue;
			}
			Vector2 curUV = tmpUVS;
			Uint32* curP = start + (int)left;
			Vector2 duv = (tmpUVE - tmpUVS) / ((int)right - (int)left);
			for (int j = left; j <= right; j++)
			{
				*curP = tex.GetPixel(curUV);
				curUV = curUV + duv;
				curP++;
			}
		}
	}
}

void Draw::DrawBottomTriangle_Tex_Gouraud(SDL_Surface* surface, Rect& rect, float x1, float y1, float x2, float x3, float y3, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, const Texture& tex)
{
	Vector2 newUV2 = uv2;
	Vector2 newUV3 = uv3;
	if (x3 < x2)
	{
		float tmp = x3;
		x3 = x2;
		x2 = tmp;
		Vector2 tmp2 = newUV3;
		newUV3 = newUV2;
		newUV2 = tmp2;
	}

	float height = y3 - y1;
	float invHeight = 1 / (y3 - y1);
	float dx_left = (x2 - x1) * invHeight;
	float dx_right = (x3 - x1) * invHeight;
	Vector2 duv_left = (newUV2 - uv1) * invHeight;
	Vector2 duv_right = (newUV3 - uv1) * invHeight;

	float xs = x1;
	float xe = x1;
	Vector2 uvs = uv1;
	Vector2 uve = uv1;

	int iy1, iy3;

	if (y1 < rect.y)
	{
		xs = xs + dx_left * (rect.y - y1);
		xe = xe + dx_right * (rect.y - y1);
		uvs = uvs + duv_left * (rect.y - y1);
		uve = uve + duv_right * (rect.y - y1);
		y1 = rect.y;
		iy1 = y1;
	}
	else
	{
		iy1 = ceilf(y1);
		xs = xs + dx_left * (iy1 - y1);
		xe = xe + dx_right * (iy1 - y1);
		uvs = uvs + duv_left * (iy1 - y1);
		uve = uve + duv_right * (iy1 - y1);
	}
	if (y3 > rect.ymax())
	{
		y3 = rect.ymax();
		iy3 = y3 - 1;
	}
	else
	{
		iy3 = ceilf(y3) - 1;
	}

	Uint32* start = GetPixelAddress(surface, 0, iy1);

	if (x1 >= rect.x && x1 <= rect.xmax() && x2 >= rect.x && x2 <= rect.xmax() && x3 >= rect.x && x3 <= rect.xmax())
	{
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			Vector2 curUV = uvs;
			Uint32* curP = start + (int)xs;
			Vector2 duv = (uve - uvs) / ((int)xe - (int)xs);
			for (int j = xs; j <= xe; j++)
			{
				*curP = tex.GetPixel(curUV);
				curUV = curUV + duv;
				curP++;
			}
			xs += dx_left;
			xe += dx_right;
			uvs = uvs + duv_left;
			uve = uve + duv_right;
		}
	}
	else
	{
		float left;
		float right;
		Vector2 tmpUVS;
		Vector2 tmpUVE;
		for (int i = iy1; i <= iy3; i++, start += surface->pitch >> 2)
		{
			left = xs;
			right = xe;
			tmpUVS = uvs;
			tmpUVE = uve;
			xs += dx_left;
			xe += dx_right;
			uvs = uvs + duv_left;
			uve = uve + duv_right;
			if (left < rect.x)
			{
				left = rect.x;
				if (right < rect.x)
					continue;
			}
			if (right > rect.xmax())
			{
				right = rect.xmax();
				if (left > rect.xmax())
					continue;
			}
			Vector2 curUV = tmpUVS;
			Uint32* curP = start + (int)left;
			Vector2 duv = (tmpUVE - tmpUVS) / ((int)right - (int)left);
			for (int j = left; j <= right; j++)
			{
				*curP = tex.GetPixel(curUV);
				curUV = curUV + duv;
				curP++;
			}
		}
	}
}

void Draw::DrawTriangle_Tex_Gouraud(SDL_Surface* surface, Rect& rect, float x1, float y1, float x2, float y2, float x3, float y3, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, const Texture& tex)
{
	if ((Equal(x1, x2) && Equal(x2, x3)) || (Equal(y1, y2) && Equal(y2, y3)))
	{
		return;
	}

	Vector2 newUV1 = uv1;
	Vector2 newUV2 = uv2;
	Vector2 newUV3 = uv3;

	if (y2 < y1)
	{
		float tmp1 = x2;
		float tmp2 = y2;
		x2 = x1;
		y2 = y1;
		x1 = tmp1;
		y1 = tmp2;
		Vector2 tmp3 = newUV2;
		newUV2 = newUV1;
		newUV1 = tmp3;
	}

	if (y3 < y1)
	{
		float tmp1 = x3;
		float tmp2 = y3;
		x3 = x1;
		y3 = y1;
		x1 = tmp1;
		y1 = tmp2;
		Vector2 tmp3 = newUV3;
		newUV3 = newUV1;
		newUV1 = tmp3;
	}

	if (y3 < y2)
	{
		float tmp1 = x3;
		float tmp2 = y3;
		x3 = x2;
		y3 = y2;
		x2 = tmp1;
		y2 = tmp2;
		Vector2 tmp3 = newUV3;
		newUV3 = newUV2;
		newUV2 = tmp3;
	}

	if (y3<rect.y || y1>rect.ymax() || (x1 < rect.x && x2 < rect.x && x3 < rect.x) || (x1 > rect.xmax() && x2 > rect.xmax() && x3 > rect.xmax()))
	{
		return;
	}

	if (Equal(y1, y2))
	{
		DrawTopTriangle_Tex_Gouraud(surface, rect, x1, y1, x2, x3, y3, newUV1, newUV2, newUV3, tex);
	}
	else if (Equal(y2, y3))
	{
		DrawBottomTriangle_Tex_Gouraud(surface, rect, x1, y1, x2, x3, y3, newUV1, newUV2, newUV3, tex);
	}
	else
	{
		float inv = (y2 - y1) / (y3 - y1);
		float new_x = x1 + (x3 - x1) * inv;
		Vector2 newUV4 = newUV1 + (newUV3 - newUV1) * inv;
		DrawBottomTriangle_Tex_Gouraud(surface, rect, x1, y1, new_x, x2, y2, newUV1, newUV4, newUV2, tex);
		DrawTopTriangle_Tex_Gouraud(surface, rect, x2, y2, new_x, x3, y3, newUV2, newUV4, newUV3, tex);
	}
}

void Draw::DrawClearColor(SDL_Surface* surface, const Color& c)
{
	//�ڴ����Ŀ��� ��ʱ�᲻���
	if (surface->pitch == surface->w << 2)
	{
		ASM_MEMSET_DWORD(GetPixelAddress(surface, 0, 0), Color2Uint32(c), surface->w * surface->h);
	}
	else
	{
		Uint32* start = GetPixelAddress(surface, 0, 0);
		Uint32 pixel = Color2Uint32(c);
		for (int i = 0; i < surface->h; i++)
		{
			ASM_MEMSET_DWORD(start, pixel, surface->w);
			start += (surface->pitch >> 2);
		}
	}
}