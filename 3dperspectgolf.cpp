#include "console.h"
#include <math.h>

int main(){
	float px,py,pa=0.0f;
	int cw=console::getConsoleWidth(),ch=console::getConsoleHeight();	
	while(true){
		console::clear();
		for(float th=-0.7854f;th<=0.7854f;th+=1.0f/cw/1.5708f){
			for(float tr=0.0f;tr<10.0f;tr+=0.1f){
				if((sinf(int(tr*sinf(pa+th)+px)*0.75f)+sinf(int(tr*-cosf(pa+th)+py)*0.75f))*0.5f>0.0f){
					int floor=(ch*0.5f)-ch/tr;
					for(int y=floor;y<ch-floor;){
						console::write((0.7854f+th)*(cw/1.5708f),y++,"#&&*!;;',."[int(tr)],FRED|BBLACK);
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
				pa-=0.392f;
				break;
			case 'd':
				pa+=0.392f;
				break;
		}
	}
}