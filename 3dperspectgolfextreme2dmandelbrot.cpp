#include "console.h"
#include <math.h>

bool sampleMandelBrot(float _x, float _y) {
	//float x,y=-1.5,u,w,v;
	float u,w,v;
	float x = _x * 0.025f;// - 3.0f;
	float y = _y * 0.025f;// - 1.5f;		
	int n=u=w=0;
	while(u*u+w*w<4&&n++<256){
		v=u*u-w*w+x;
		w=2*u*w+y;
		u=v;
	}
	return n % 8 == 3;		
}

int wmain(){
	float px,py,pa=0;
	int cw=console::getConsoleWidth(),ch=console::getConsoleHeight();	
	
	
	int _chars = cw * ch;
	wchar_t _fb[_chars + 1];
	color_t _cb[_chars + 1];
	
	
	while(true){
		
		
		for (int i = 0; i < _chars; i++) {
			_fb[i] = L' ';
			_cb[i] = FWHITE|BBLACK;
		}
		
		
		
		//console::clear();
						//_fb[20*cw] = L'-';
		for(float th=-0.79f;th<=0.78f;th+=1.0f/cw/1.57f){
			int x = int((0.79f+th)*(cw/1.57f));
			//float x = ((0.79f+th)*(cw/1.57f));
						//_fb[20*cw+1] = L'-';
			for(float tr=0.0f;tr<150.0f;tr+=0.05f){
						//_fb[20*cw+2] = L'-';
						//_fb[int(2 * cw + ((0.79f+th)*(cw/1.57f)))] = L'*';
				//if(sampleMandelBrot((0.79f + th)*(cw/1.57f),y)){
				//if (sampleMandelBrot(int(tr*sinf(pa+th)+px)*0.7f,int(tr*-cosf(pa+th)+py)*0.7f)){
				if (sampleMandelBrot((tr * sinf(pa + th) + px), (tr * -cosf(pa + th) + py))){
					
						//_fb[int(1 * cw + ((0.79f+th)*(cw/1.57f)))] = L'.';
						//_fb[20*cw+3] = L'>';
					int fl=(ch/2)-ch/tr;
					if (fl < 0) fl = 0;
					for(int y=fl;y<ch-fl;){									
						
						//_fb[int(y * cw + x)] = L"#@MNO&%*+=;:,."[int(tr * 0.13334f)];
						_fb[int(y * cw + x)] = L"#@MNO&%*+=;:,. "[int(sqrt(tr * 1.5))];
						//_fb[int(y * cw + x)] = '#';
						_cb[int(y++ * cw + x)] = 7;
											
						//console::write((0.79f+th)*(cw/1.57f),y++,"#&&*!;;',."[int(tr)],4);
						
					}
					break;
				}
			}
		}		
		
		
		
		for (int x = 0; x < 11; x++) {
			for (int y = 0; y < 11; y++) {
				//console::write(x, y, sampleMandelBrot(x + px - 5.5f,y + py - 5.5f) ? L'#' : L' ', FWHITE | BBLACK);
				_fb[y * cw + x] = sampleMandelBrot(x + px - 5.5f,y + py - 5.5f) ? L'#' : L' ';
			}
		}
		
		//_fb[5 * cw + 5] = L'o';
		//_fb[int(int(5.5f - cosf(pa)) * cw + int(5.5f + sinf(pa)))] = L'+';
		
		//console::write(5,5,L'o',FRED|BBLACK);
		//console::write(5.5f + sinf(pa), 5.5f - cosf(pa), L'+', FGREEN|BBLACK);
		
		
		console::write(_fb, _cb, _chars - 1);
		
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