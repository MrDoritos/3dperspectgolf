#include <stdio.h>

main() {
	float x,y=-1.5,u,w,v;
	for(;y<1.2;y+=0.027)
	for(x=-3;x<0.96;x+=0.04){
		int n=u=w=0;
		while(u*u+w*w<4&&n++<16){
			v=u*u-w*w+x;
			w=2*u*w+y;
			u=v;
		}
		putchar("#&*!;',."[n%8]);
	}
}

/*
int cw=100,ch=100,x=0,y,n;	
	for(;x<cw;x++)
	for(y=0;y<ch;y++){
		float c_w=x*0.04-3,
			  c_h=y*0.027-1.5,
			  zw=0,zh=0,tx;
		n=0;
		while((zw*zw+zh*zh<4)&&(n++<128)){
			tx=zw*zw-zh*zh+c_w;
			zh=2*zw*zh+c_h;
			zw=tx;
		}
		putchar("#&*!;',."[n%8]);
	}
*/

/*
	
	int cw=100,ch=100,x=0,y,n;	
	for(float x=-3;x<-1;x+=0.04)
	for(float y=-1.5;y<0.5;y+=0.027){
		float zw=0,zh=0,tx;
		n=0;
		while((zw*zw+zh*zh<4)&&(n++<128)){
			tx=zw*zw-zh*zh+x;
			zh=2*zw*zh+y;
			zw=tx;
		}
		putchar("#&*!;',."[n%8]);
	}

*/