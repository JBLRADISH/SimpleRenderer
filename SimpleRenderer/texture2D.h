#pragma once

#include "tool.h"
#include "texture.h"
#include <string>
#include <iostream>
#include <Windows.h>
using namespace std;

enum TextureFilter 
{
	Filter_Point,
	Filter_Bilinear,
};

class Texture2D : public Texture
{
public:
	int width;
	int height;
	int bpp;
	unsigned char** buffer;
	int mipmapLevel;

	TextureFilter texFilter;

	Texture2D()
	{
		width = 0;
		height = 0;
		texFilter = TextureFilter::Filter_Bilinear;
	}

	void SetMipMapLevel(int mipmapLevel)
	{
		this->mipmapLevel = mipmapLevel;
		buffer = (unsigned char**)malloc(sizeof(unsigned char*) * (mipmapLevel + 1));
	}

	void UnLoad()
	{
		for (int i = 0; i <= mipmapLevel; i++)
		{
			free(buffer[i]);
			buffer[i] = 0;
		}
		free(buffer);
		buffer = NULL;
	}

	Color GetColor(int idx, int level) const
	{
		//BGRA
		return Color(buffer[level][idx + 2], buffer[level][idx + 1], buffer[level][idx]);
	}

	Color GetColor(const Vector3& uv) const
	{
		Color res;
		if (width == 0)
		{
			res = Color::white;
			return res;
		}
		if (texFilter == TextureFilter::Filter_Point)
		{
			int x = uv.x * (width - 1);
			int y = uv.y * (height - 1);
			int idx = (y * width + x) * bpp;
			res = GetColor(idx, 0) / 255.0f;
		}
		else if (texFilter == TextureFilter::Filter_Bilinear)
		{
			res = Bilinear(uv, width, height, 0) / 255.0f;
		}
		return res;
	}

	Color Bilinear(const Vector3& uv, int width, int height, int mipmapLevel) const
	{
		Color res;
		float x = uv.x * (width - 1);
		float y = uv.y * (height - 1);
		int floorx = Floor(x);
		int floory = Floor(y);
		int ceilx = Ceil(x);
		int ceily = Ceil(y);
		int lefttopidx = (floory * width + floorx) * bpp;
		int leftbottomidx = (ceily * width + floorx) * bpp;
		int righttopidx = (floory * width + ceilx) * bpp;
		int rightbottomidx = (ceily * width + ceilx) * bpp;
		Color topColor = Color::Lerp(GetColor(lefttopidx, mipmapLevel), GetColor(righttopidx, mipmapLevel), x - floorx);
		Color bottomColor = Color::Lerp(GetColor(leftbottomidx, mipmapLevel), GetColor(rightbottomidx, mipmapLevel), x - floorx);
		res = Color::Lerp(topColor, bottomColor, y - floory);
		return res;
	}

	void GenerateMipMap()
	{
		int curWidth = width;
		int curHeight = height;
		for (int i = 1; i <= mipmapLevel; i++)
		{
			int originWidth = curWidth;
			curWidth = curWidth >> 1;
			curHeight = curHeight >> 1;
			buffer[i] = (unsigned char*)malloc(curWidth * curHeight * bpp);
			for (int x = 0; x < curWidth; x++)
			{
				for (int y = 0; y < curHeight; y++)
				{
					int lefttopidx = (y * 2 * originWidth + x * 2) * bpp;
					int leftbottomidx = ((y * 2 + 1) * originWidth + x * 2) * bpp;
					int righttopidx = (y * 2 * originWidth + x * 2 + 1) * bpp;
					int rightbottomidx = ((y * 2 + 1) * originWidth + x * 2 + 1) * bpp;
					Color average = GetColor(lefttopidx, i - 1) + GetColor(leftbottomidx, i - 1) + GetColor(righttopidx, i - 1) + GetColor(rightbottomidx, i - 1);
					average = average / 4;
					int idx = (y * curWidth + x) * bpp;
					buffer[i][idx] = Round(average.b);
					buffer[i][idx + 1] = Round(average.g);
					buffer[i][idx + 2] = Round(average.r);
				}
			}
		}
	}

	void Load(const string& filename, bool mipmap)
	{
		FILE* fp;
		BITMAPFILEHEADER header;
		RGBQUAD palette[256];
		BITMAPINFOHEADER info;
		fp = fopen(filename.c_str(), "rb");
		if (fp != NULL)
		{
			//cout << "λͼ�ļ���:" << filename << endl;
			fread(&header, 1, sizeof(BITMAPFILEHEADER), fp);
			//showBmpHead(header);
			if (0x4d42 != header.bfType)
			{
				cout << filename << " is not a bmp file!" << endl;
			}
			fread(&info, 1, sizeof(BITMAPINFOHEADER), fp);
			//showBmpInforHead(info);
			for (unsigned int nCounti = 0; nCounti < info.biClrUsed; nCounti++)
			{
				fread((char*)& palette[nCounti].rgbBlue, 1, sizeof(BYTE), fp);
				fread((char*)& palette[nCounti].rgbGreen, 1, sizeof(BYTE), fp);
				fread((char*)& palette[nCounti].rgbRed, 1, sizeof(BYTE), fp);
				cout << "strPla[nCounti].rgbBlue" << palette[nCounti].rgbBlue << endl;
				cout << "strPla[nCounti].rgbGreen" << palette[nCounti].rgbGreen << endl;
				cout << "strPla[nCounti].rgbRed" << palette[nCounti].rgbRed << endl;
			}
			width = info.biWidth;
			height = info.biHeight;
			bpp = info.biBitCount / 8;
			SetMipMapLevel(log2f(fminf(width, height)));
			buffer[0] = (BYTE*)calloc(sizeof(BYTE), info.biSizeImage);
			fseek(fp, -(int)(info.biSizeImage), SEEK_END);
			fread(buffer[0], sizeof(BYTE), info.biSizeImage, fp);
			fclose(fp);
			if (mipmap)
			{
				GenerateMipMap();
			}
		}
		else
		{
			cout << filename << " open error!" << endl;
		}
	}

private:
	static void showBmpHead(BITMAPFILEHEADER header)
	{
		cout << "λͼ�ļ�ͷ:" << endl;
		cout << "�ļ�����:" << header.bfType << endl;
		cout << "�ļ���С:" << header.bfSize << endl;
		cout << "������_1:" << header.bfReserved1 << endl;
		cout << "������_2:" << header.bfReserved2 << endl;
		cout << "ʵ��λͼ���ݵ�ƫ���ֽ���:" << header.bfOffBits << endl;
	}

	static void showBmpInforHead(BITMAPINFOHEADER info)
	{
		cout << "λͼ��Ϣͷ:" << endl;
		cout << "�ṹ��ĳ���:" << info.biSize << endl;
		cout << "λͼ��:" << info.biWidth << endl;
		cout << "λͼ��:" << info.biHeight << endl;
		cout << "biPlanesƽ����:" << info.biPlanes << endl;
		cout << "biBitCount������ɫλ��:" << info.biBitCount << endl;
		cout << "ѹ����ʽ:" << info.biCompression << endl;
		cout << "biSizeImageʵ��λͼ����ռ�õ��ֽ���:" << info.biSizeImage << endl;
		cout << "X����ֱ���:" << info.biXPelsPerMeter << endl;
		cout << "Y����ֱ���:" << info.biYPelsPerMeter << endl;
		cout << "ʹ�õ���ɫ��:" << info.biClrUsed << endl;
		cout << "��Ҫ��ɫ��:" << info.biClrImportant << endl << endl;
	}
};