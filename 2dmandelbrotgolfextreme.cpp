#include "console.h"
#include <stdio.h>
int wmain() {
	int cw=console::getConsoleWidth(),ch=console::getConsoleHeight(),x=0,y,n;	
	for(;x<cw;x++){
		for(y=0;y<ch;){
			float c_w=x*((4.0)/cw)+-3.0,
				  c_h=y*((2.7)/ch)+-1.5,
				  zw=0,zh=0,tx;
			n=0;
			while((zw*zw+zh*zh<4)&&(n++<128)){
				tx=zw*zw-zh*zh+c_w;
				zh=2*zw*zh+c_h;
				zw=tx;
			}
			console::write(x,y++,L"#&*!;',."[n%8]);
		}
	}		
	console::readKey();
}
