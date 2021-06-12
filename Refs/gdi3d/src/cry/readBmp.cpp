#include "StdAfx.h"
#include <stdio.h>
#include "readBmp.h"
bool CmyBmp::readBmp(const char *bmpName) //ref: http://wenku.baidu.com/view/901572d5360cba1aa811da61.html
{  
	//�����ƶ���ʽ��ָ����ͼ���ļ�
	FILE *fp=fopen(bmpName,"rb"); 
	if(fp==0) return 0; 
	//����λͼ�ļ�ͷ�ṹBITMAPFILEHEADER 
	fseek(fp, sizeof(BITMAPFILEHEADER),0); 
	//����λͼ��Ϣͷ�ṹ��������ȡλͼ��Ϣͷ���ڴ棬����ڱ���head��
	BITMAPINFOHEADER head;    
	fread(&head, sizeof(BITMAPINFOHEADER), 1,fp); 
	//��ȡͼ����ߡ�ÿ������ռλ������Ϣ
	bmpWidth = head.biWidth; 
	bmpHeight = head.biHeight; 
	biBitCount = head.biBitCount;  
	//�������������ͼ��ÿ��������ռ���ֽ�����������4�ı�����
	lineByte=(bmpWidth * biBitCount/8+3)/4*4; 
	//�Ҷ�ͼ������ɫ������ɫ�����Ϊ256 
	if(biBitCount==8){  
		//������ɫ������Ҫ�Ŀռ䣬����ɫ����ڴ�
		pColorTable=new RGBQUAD[256];  
		fread(pColorTable,sizeof(RGBQUAD),256,fp); 
	}  
	//����λͼ��������Ҫ�Ŀռ䣬��λͼ���ݽ��ڴ�
	pBmpBuf=new unsigned char[lineByte * bmpHeight]; 
	fread(pBmpBuf,1,lineByte * bmpHeight,fp); 
	//�ر��ļ�
	fclose(fp); 

	return 1; 
}
void CmyBmp::drawBmp(HDC hdc){
	//ref: http://wenku.baidu.com/view/901572d5360cba1aa811da61.html
	for(int i=0;i<bmpHeight;i++){
		for(int j=0;j<bmpWidth;j++){
			unsigned char B=*(pBmpBuf+i*lineByte+j*3+0);
			unsigned char G=*(pBmpBuf+i*lineByte+j*3+1);
			unsigned char R=*(pBmpBuf+i*lineByte+j*3+2);
			SetPixel(hdc, j, i, RGB(R,G,B));
		}
	}

}
Crgb CmyBmp::getPixelColor(int i,int j){
	//ref: http://wenku.baidu.com/view/901572d5360cba1aa811da61.html
	Crgb rgb;
	if(i<0||i>=bmpHeight||j<0||j>=bmpWidth){
		rgb.init(255,255,255);
		return rgb;
	}
	unsigned char *base=pBmpBuf+i*lineByte+j*3;
	unsigned char B=*(base+0);
	unsigned char G=*(base+1);
	unsigned char R=*(base+2);
	rgb.init(R,G,B);
	return rgb;
}