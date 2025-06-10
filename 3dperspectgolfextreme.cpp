#include "console.h"
#include <math.h>

int main(){
	float px,py,pa=0;
	int cw=console::getConsoleWidth(),ch=console::getConsoleHeight();	
	while(true){
		console::clear();
		for(float th=-0.79f;th<=0.78f;th+=1.0f/cw/1.57f){
			for(float tr=0.0f;tr<10.0f;tr+=0.1f){
				if((sinf(int(tr*sinf(pa+th)+px)*0.7f)+sinf(int(tr*-cosf(pa+th)+py)*0.7f))/2>0){
					int fl=(ch/2)-ch/tr;
					for(int y=fl;y<ch-fl;){
						console::write((0.79f+th)*(cw/1.57f),y++,"#&&*!;;',."[int(tr)],4);
					}
					break;
				}
			}
		}		
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