#pragma once

#include <Windows.h>
#include "texture.h"
#include <string>
#include <iostream>
using namespace std;

class Bmp
{
public:
	static Texture Load(const string& filename)
	{
		Texture res;
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
				return res;
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
			res.width = info.biWidth;
			res.height = info.biHeight;
			res.SetMipMapLevel(log2f(fminf(res.width, res.height)));
			res.buffer[0] = (BYTE*)calloc(sizeof(BYTE), info.biSizeImage);
			fseek(fp, -(int)(info.biSizeImage), SEEK_END);
			fread(res.buffer[0], sizeof(BYTE), info.biSizeImage, fp);
			fclose(fp);
			//flip bmp
			int bytes_per_line = sizeof(BYTE) * info.biBitCount / 8 * info.biWidth;
			BYTE* tmp = (BYTE*)malloc(info.biSizeImage);
			memcpy(tmp, res.buffer[0], info.biSizeImage);
			for (int i = 0; i < info.biHeight; i++)
			{
				memcpy(&res.buffer[0][((info.biHeight - 1) - i) * bytes_per_line], &tmp[i * bytes_per_line], bytes_per_line);
			}
			free(tmp);
			res.GenerateMipMap();
			return res;
		}
		else
		{
			cout << filename << " open error!" << endl;
			return res;
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