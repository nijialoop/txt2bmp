typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <wchar.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H



typedef long LONG;

//位图文件头定义;
typedef struct  tagBITMAPFILEHEADER{
        DWORD bfSize;//文件大小
        WORD bfReserved1;//保留字
        WORD bfReserved2;//保留字
        DWORD bfOffBits;//从文件头到实际位图数据的偏移字节数
}BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER{
        DWORD biSize;//信息头大小
        DWORD biWidth;//图像宽度
        DWORD biHeight;//图像高度
        WORD biPlanes;//位平面数，必须为1
        WORD biBitCount;//每像素位数
        DWORD  biCompression; //压缩类型
        DWORD  biSizeImage; //压缩图像大小字节数
        DWORD  biXPelsPerMeter; //水平分辨率
        DWORD  biYPelsPerMeter; //垂直分辨率
        DWORD  biClrUsed; //位图实际用到的色彩数
        DWORD  biClrImportant; //本位图中重要的色彩数
}BITMAPINFOHEADER; //位图信息头定义

//像素信息
typedef struct tagIMAGEDATA
{
        BYTE blue;
        BYTE green;
        BYTE red;
}DATA;

//BMP整体信息
typedef struct tagBMP_BUFFER
{
    WORD                mbfType;
    BITMAPFILEHEADER    hand;
    BITMAPINFOHEADER    info;
    DATA*               BUFFER;
}BMP_BUFFER;





#define C_BLACK 0x000000
#define C_WHITE 0xFFFFFF
#define C_RED 	0xFF0000
#define C_GREEN 0x00FF00
#define C_BLUE 	0x0000FF

#define RColor(HexValue) (unsigned char)((HexValue & 0xFF0000) >> 16)
#define GColor(HexValue) (unsigned char)((HexValue & 0x00FF00) >> 8)
#define BColor(HexValue) (unsigned char)(HexValue & 0x0000FF) 


