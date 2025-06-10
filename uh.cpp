#include "console.h"
#include <math.h>

bool sampleMandelBrot(float _x, float _y) {
	//float x,y=-1.5,u,w,v;
	float u,w,v;
	float x = _x * 0.2f;// - 3.0f;
	float y = _y * 0.2f;// - 1.5f;		
	int n=u=w=0;
	while(u*u+w*w<4&&n++<128){
		v=u*u-w*w+x;
		w=2*u*w+y;
		u=v;
	}
	return n % 8 == 3;		
}

int wmain(){
	float px,py,pa=0;
	int cw=console::getConsoleWidth(),ch=console::getConsoleHeight();	
	
	/*
	int chars = cw * ch;
	wchar_t fb[chars + 1];
	color_t cb[chars + 1];
	*/
	
	while(true){
		
		/*
		for (int i = 0; i < chars; i++) {
			fb[i] = L' ';
			cb[i] = FWHITE|BBLACK;
		}
		*/
		
		
		console::clear();
						//fb[20*cw] = L'-';
		for(float th=-0.79f;th<=0.78f;th+=1.0f/cw/1.57f){
						//fb[20*cw+1] = L'-';
			for(float tr=0.0f;tr<10.0f;tr+=0.1f){
						//fb[20*cw+2] = L'-';
						//fb[int(2 * cw + ((0.79f+th)*(cw/1.57f)))] = L'*';
				//if(sampleMandelBrot((0.79f + th)*(cw/1.57f),y)){
				//if (sampleMandelBrot(int(tr*sinf(pa+th)+px)*0.7f,int(tr*-cosf(pa+th)+py)*0.7f)){
				if (sampleMandelBrot((tr * sinf(pa + th) + px), (tr * -cosf(pa + th) + py))){
					
						//fb[int(1 * cw + ((0.79f+th)*(cw/1.57f)))] = L'.';
						//fb[20*cw+3] = L'>';
					int fl=(ch/2)-ch/tr;
					for(int y=fl;y<ch-fl;){									
						console::write((0.79f+th)*(cw/1.57f),y++,"#&&*!;;',."[int(tr)],4);
						
						//fb[int(y * cw + ((0.79f+th)*(cw/1.57f)))] = L"#&&*!;;',."[int(tr)];
						//cb[int(y++ * cw + ((0.79f+th)*(cw/1.57f)))] = 4;
						
					}
					break;
				}
			}
		}		
		
		//console::write(fb, cb, chars - 1);
		
		for (int x = 0; x < 11; x++) {
			for (int y = 0; y < 11; y++) {
				console::write(x, y, sampleMandelBrot(x + px - 5.5f,y + py - 5.5f) ? L'#' : L' ', FWHITE | BBLACK);
			}
		}	
		console::write(5,5,L'o',FRED|BBLACK);
		console::write(5.5f + sinf(pa), 5.5f - cosf(pa), L'+', FGREEN|BBLACK);
		
		
		switch (NOMOD(console::readKey())){
			case 's':
				px-=sinf(pa)*0.5f;
				py+=cosf(pa)*0.5f;
				break;
			case 'w':
				px+=sinf(pa)*0.5f;
				py-=cosf(pa)*0.5f;
				break;
			case 'a':
				pa-=0.39f;
				break;
			case 'd':
				pa+=0.39f;
				break;
		}
	}
}