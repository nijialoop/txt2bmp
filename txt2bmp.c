#include "txt2bmp.h"

int main(int argv, char **argc)
{
    BMP_BUFFER one_bmp_BUFFER;
	int x,y,i,j,k,cnt=0;

	int en_pitch = 0;	//字体间距累加值
	int en_font_H;		//字体高度 中英文一致 
	int en_font_W;		//字体宽度 中文X2

	int font_H;			//生成的位图的字体高度
	int font_W;			//生成的位图的字体宽度

    int bitmap_width_sum 	= 0;//字体在图像上X轴起始位置
    int bitmap_high_sum 	= 0;//字体在图像上Y轴起始位置
    int width_mul;		//字体宽度倍率，中文x2

	unsigned char *pBitmap;  //位图缓冲区
	unsigned char *temp;
	unsigned char tmpChar;
	DWORD *wszString; //字符串缓冲区，unicode编码，3字节宽度
	wchar_t wctemp;
	WORD *String_UCS;  // 转换长度之后的字符串
	int	ptStr_utf8; //UTF-8字符指针
    int ptStr_char; //字节序字符指针
    int lenStr_utf8;//UTF-8字符长度
    int lenStr_char;//UTF-8字符长度
    int cnt_utf8=0;
    WORD word;	//单个字符缓冲


	int TEXT_WIDTH;		//生成图片的宽度
	int TEXT_HEIGHT;	//生成图片的高度
	int TEXT_PITCH;		//生成图片的字间距
	int C_FRGND;		//前景色
	int C_BKGND;		//背景色

	//Free Type 的库
    FT_Library 	pFT_CNLib 	= NULL;
    FT_Face 	pFT_CNFace 	= NULL;
    FT_Library 	pFT_ENLib 	= NULL;
    FT_Face 	pFT_ENFace 	= NULL;
    FT_Error error = 0;




	//初始化各个输入参数，判断边界条件
	if(argv != 10)
	{
		printf("Usage : %s cn_font en_font width height pitch out_file_name fg_color bg_color wszString\r\n",argc[0]);
		return -1;
	}
	
	TEXT_WIDTH = atoi(argc[3]);
	if((TEXT_WIDTH < 1)||(TEXT_WIDTH >100000))
	{
		printf("TEXT_WIDTH out of range\n"); 
		return -1;
	}
	
	TEXT_HEIGHT =  atoi(argc[4]);
	if((TEXT_HEIGHT < 1)||(TEXT_HEIGHT >100000))
	{
		printf("TEXT_HEIGHT out of range\n"); 
		return -1;
	}
	
	TEXT_PITCH =  atoi(argc[5]);	
	if((TEXT_PITCH < 1)||(TEXT_PITCH >10000))
	{
		printf("TEXT_PITCH out of range\n"); 
		return -1;
	}	


	C_FRGND = strtol(argc[7],NULL,16);
	if((C_FRGND < 0)||(C_FRGND >0xFFFFFF))
	{
		printf("C_FRGND out of range\n"); 
		return -1;
	}
	
	C_BKGND =  strtol(argc[8],NULL,16);
	if((C_BKGND < 0)||(C_BKGND >0xFFFFFF))
	{
		printf("C_BKGND out of range\n"); 
		return -1;
	}
	lenStr_char = strlen(argc[9]);
	wszString = malloc(lenStr_char*4);
	String_UCS =  malloc(lenStr_char*4);
	temp = malloc(lenStr_char*1);
	memcpy(temp,argc[9],lenStr_char);

	lenStr_utf8 = 0;
	ptStr_utf8  = 0;
	ptStr_char  = 0;
	en_pitch = 0;
	cnt = 0;	
	for (i=0;i<lenStr_char;i++)
	{
		tmpChar = *(temp+i);
		//printf("1-%d %d %02X \n",i,ptStr_utf8,tmpChar);
		if (tmpChar <= 0x7F)  // ASCII码 长度 -1
		{
			*(wszString + ptStr_utf8)  = tmpChar;
			//printf("2-%d %d %08X\n",i,ptStr_utf8,*(wszString + ptStr_utf8));
			ptStr_utf8++;
			cnt_utf8++;
			cnt++;
			
			
		}
		else if ((tmpChar >= 0xC0) && (tmpChar <= 0xDF))// UTF-8首字节 长度 -2
		{
			*(wszString + ptStr_utf8)  = tmpChar << 8;
			lenStr_utf8 = 2;
		}
		else if ((tmpChar >= 0xE0) && (tmpChar <= 0xEF))// UTF-8首字节 长度 -3
		{
			*(wszString + ptStr_utf8)  = tmpChar << 16;
			lenStr_utf8 = 3;
			//printf("3-%d %d %08X\n",i,ptStr_utf8,*(wszString + ptStr_utf8));
			
		}
		else if ((tmpChar > 0x7F) && (tmpChar < 0xC0))// UTF-8 中间字节
		{
			lenStr_utf8--;
			*(wszString + ptStr_utf8)  += (tmpChar<<((lenStr_utf8-1) * 8));
			//printf("4-%d %d %08X\n",i,ptStr_utf8,*(wszString + ptStr_utf8));
			
			if (lenStr_utf8 == 1)
			{
				ptStr_utf8++;
				cnt_utf8++;
				cnt+=2;
			}
		}	

	}
	printf("Chars Number %d\n",cnt_utf8);

	for (i=0;i<cnt_utf8;i++)
	{
		wctemp = *(wszString+i);
		if (wctemp>0x7F)
			*(String_UCS+i) = (wctemp & 0x00003F) + ((wctemp & 0x003F00)>>2)+((wctemp & 0x0F0000)>>4);
		else
			*(String_UCS+i) = wctemp;
		printf("%d %08X %04X\n",i,*(wszString+i),*(String_UCS+i));
	}
	en_font_W = (TEXT_WIDTH - (TEXT_PITCH * (cnt_utf8-1)))/cnt;
	en_font_H = TEXT_HEIGHT;
	if((en_font_W < 1)||(en_font_W >100000))
	{
		printf("en_font_W out of range\n"); 
		return -1;
	}

	printf("Paras:\nWidth: %d\nHeight: %d\nPitch: %d\n",TEXT_WIDTH, TEXT_HEIGHT,TEXT_PITCH);
	printf("FG: %06X\nBG: %06X\n",C_FRGND,C_BKGND);
	printf("Width_Char_EN: %d\nHeight_Char: %d \n",en_font_W, en_font_H,cnt);

	//初始化BMP文件头
    one_bmp_BUFFER.mbfType                =0X4D42;
    one_bmp_BUFFER.hand.bfOffBits         =54;
    one_bmp_BUFFER.hand.bfReserved1       =0;
    one_bmp_BUFFER.hand.bfReserved2       =0;
    one_bmp_BUFFER.hand.bfSize            =TEXT_WIDTH*TEXT_HEIGHT*3+40+14;
    one_bmp_BUFFER.BUFFER                 =malloc(TEXT_WIDTH*TEXT_HEIGHT*sizeof(DATA));	
	//需要判断是否分配成功
    one_bmp_BUFFER.info.biSize            =40;
    one_bmp_BUFFER.info.biWidth           =TEXT_WIDTH;
    one_bmp_BUFFER.info.biHeight          =TEXT_HEIGHT;	
    one_bmp_BUFFER.info.biPlanes          =1;
    one_bmp_BUFFER.info.biBitCount        =24;
    one_bmp_BUFFER.info.biCompression     =0;
    one_bmp_BUFFER.info.biSizeImage       =TEXT_WIDTH*TEXT_HEIGHT*3;
    one_bmp_BUFFER.info.biXPelsPerMeter   =0;
    one_bmp_BUFFER.info.biYPelsPerMeter   =0;
    one_bmp_BUFFER.info.biClrUsed         =0;
	one_bmp_BUFFER.info.biClrImportant    =0;
	//初始化底色
    for(x=0;x<TEXT_WIDTH;x++)
    {
        for(y=0;y<TEXT_HEIGHT;y++)
        {
            one_bmp_BUFFER.BUFFER[x*TEXT_HEIGHT+y].red=RColor(C_BKGND);
            one_bmp_BUFFER.BUFFER[x*TEXT_HEIGHT+y].blue=BColor(C_BKGND);
            one_bmp_BUFFER.BUFFER[x*TEXT_HEIGHT+y].green=GColor(C_BKGND);
        }
    }

   //1.初始化一个freetype库
    error = FT_Init_FreeType(&pFT_CNLib);
    if(error){
        pFT_CNLib = 0;
        printf("There is some error when init library\n");
        return -1;
    }
	//2.加载字体样式
    error = FT_New_Face(pFT_CNLib, argc[1], 0, &pFT_CNFace);
    if(error){
        printf("Open font failed\n");
        return -1;
    }
	//3.设置字体的大小
    FT_Set_Pixel_Sizes(pFT_CNFace, 2*en_font_W,en_font_H);//通过设置长宽范围设置
    FT_Glyph CN_glyph;

	//1.初始化一个freetype库
	 error = FT_Init_FreeType(&pFT_ENLib);
	 if(error){
		 pFT_ENLib = 0;
		 printf("There is some error when init 2library\n");
		 return -1;
	 }
	 //2.加载字体样式
	 error = FT_New_Face(pFT_ENLib, argc[2], 0, &pFT_ENFace);
	 if(error){
		 printf("Open 2font failed\n");
		 return -1;
	 }
	//3.设置字体的大小
    FT_Set_Pixel_Sizes(pFT_ENFace, 2*en_font_W,en_font_H);//通过设置长宽范围设置
    FT_Glyph En_glyph;
    //FT_Glyph glyph;	
	
    for(k=0; k < cnt_utf8; k++)
    {
    	//循环处理字串内每个字
		//memcpy(&word, *(wszString+k), 4);
		word =  *(String_UCS+k);
	    //printf("StringCode : %02X\n",word);
		//区分中文字符，英文字符。宽度不同占位不同。
		if (word < 0x80) // English Char
		{
			//转化成位图
			FT_Load_Glyph(pFT_ENFace, FT_Get_Char_Index(pFT_ENFace,word), FT_LOAD_RENDER |FT_LOAD_TARGET_NORMAL);
			FT_Get_Glyph(pFT_ENFace->glyph, &En_glyph);
			FT_Glyph_To_Bitmap(&En_glyph, ft_render_mode_normal, 0, 1);
			FT_BitmapGlyph bitmap_glyph_en = (FT_BitmapGlyph)En_glyph;
			//取位图数据
			FT_Bitmap  *bitmap_en = &bitmap_glyph_en->bitmap;
			font_H=bitmap_en->rows;
			font_W=bitmap_en->width;
			pBitmap = bitmap_en->buffer;
			width_mul = 1;
		}
		else  // Chinese Char
		{
			//转化成位图
			FT_Load_Glyph(pFT_CNFace, FT_Get_Char_Index(pFT_CNFace,word), FT_LOAD_RENDER |FT_LOAD_TARGET_NORMAL);
			FT_Get_Glyph(pFT_CNFace->glyph, &CN_glyph);
			FT_Glyph_To_Bitmap(&CN_glyph, ft_render_mode_normal, 0, 1);
			FT_BitmapGlyph bitmap_glyph_cn = (FT_BitmapGlyph)CN_glyph;
			//取位图数据
			FT_Bitmap  *bitmap_cn = &bitmap_glyph_cn->bitmap;
			font_H=bitmap_cn->rows;
			font_W=bitmap_cn->width;
			pBitmap = bitmap_cn->buffer;
			width_mul = 2;
		}
	    //printf("StringParas W,H,P: %d, %d, %d\n",font_W,font_H,font_P);
	    //扫描位图，根据位图黑色还是白色来重绘前景色与背景色
	    for( i=0;i<font_H;++i)
	    {
	        for( j=0;j<font_W;++j)
	        {
				bitmap_high_sum = (int)((TEXT_HEIGHT - font_H) /2);
				if(pBitmap[i * font_W +j] != 0)					
				{
					one_bmp_BUFFER.BUFFER[(TEXT_HEIGHT-i-bitmap_high_sum)*TEXT_WIDTH+j+bitmap_width_sum].red	=RColor(C_FRGND);
					one_bmp_BUFFER.BUFFER[(TEXT_HEIGHT-i-bitmap_high_sum)*TEXT_WIDTH+j+bitmap_width_sum].blue	=BColor(C_FRGND);
					one_bmp_BUFFER.BUFFER[(TEXT_HEIGHT-i-bitmap_high_sum)*TEXT_WIDTH+j+bitmap_width_sum].green	=GColor(C_FRGND);
				}
				else
				{
					one_bmp_BUFFER.BUFFER[(TEXT_HEIGHT-i-bitmap_high_sum)*TEXT_WIDTH+j+bitmap_width_sum].red	=RColor(C_BKGND);
					one_bmp_BUFFER.BUFFER[(TEXT_HEIGHT-i-bitmap_high_sum)*TEXT_WIDTH+j+bitmap_width_sum].blue	=BColor(C_BKGND);
					one_bmp_BUFFER.BUFFER[(TEXT_HEIGHT-i-bitmap_high_sum)*TEXT_WIDTH+j+bitmap_width_sum].green	=GColor(C_BKGND);
				}
	        }
	    }
		bitmap_width_sum += (en_font_W *width_mul) + TEXT_PITCH;
    }

	//写入文件
    FILE *fpo1;
    fpo1=fopen(argc[6],"wb");
    fwrite(&one_bmp_BUFFER.mbfType,1,sizeof(WORD),fpo1);
    fwrite(&one_bmp_BUFFER.hand,1,sizeof(BITMAPFILEHEADER),fpo1);
    fwrite(&one_bmp_BUFFER.info,1,sizeof(BITMAPINFOHEADER),fpo1);
    fwrite(one_bmp_BUFFER.BUFFER,1,sizeof(DATA)*(TEXT_HEIGHT*TEXT_WIDTH),fpo1);
    fclose(fpo1);

    return 0;
}
