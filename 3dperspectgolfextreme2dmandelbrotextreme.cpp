#include "console.h"
#include <math.h>
int main(){
	float px,py,pa=0; //playerPositionX,playerPositionY,playerAngle
	int cw=console::getConsoleWidth(),ch=console::getConsoleHeight();//Get console dimensions
		
	while(1){ //Infinite loop
		console::clear(); //Clear console
		for(float th=-0.79f;th<=0.78f;th+=1.0f/cw/1.57f){ //Iterate the screenspace in terms of radians, x0 on the console is -0.79 rad or -45 deg x99 on the console is 0.79 rad or 45 deg, 90 deg FOV
			for(float tr=2.0f;tr<150.0f;tr+=0.05f+(tr*0.05f)){ //Iterate a ray, starting at 2.0, stopping at 150.0, and adding f(x)=0.05+f(x-1) each loop
				float u,w,v,x=(tr*sinf(pa+th)+px)/40,y=(tr*-cosf(pa+th)+py)/40;//u,w,v for mandelbrot, x is the x for the ray cast on the player angle and ray distance, y is the y for the ray same as x except -cosf, divided by 40 to scale the set good
				int n=u=w=0,fl=(ch/2)-ch/tr;//n is set to 0, its the iterator for the mandelbrot set, u and w also set to zero. fl is the distance to the floor for a vertical column on the screen, divides the console height by two and subtracts console height divided by the ray distance from the camera
				while(u*u+w*w<4&&n++<256){ //mandelbrot calculation
					v=u*u-w*w+x; // tempx = z_real*_real - z_imaginary*z_imaginary + c_real
					w=2*u*w+y; // 2*z_real*z_imaginary + c_imaginary
					u=v; // Updating z_real = tempx
				}				
				if (n%8==3){ //if the number of iterations is %8 equal to 3 its a valid part of the map
					for(int y=fl;y<ch-fl;) //iterate y starting at floor and ending at the console height minus floor, centering the drawing
						console::write((0.79f+th)*(cw/1.57f),y++,"#@MNO&%*+=;:,. "[int(sqrt(tr * 1.5))],7); //take the real x value by calculating it from the current test ray, take y and increment, index the ascii gradient from the square root of the ray distance times 3/2, set the color to white on black
					break; //end the loop, saving cpu time and potentially ordering the columns correctly and closer ones in front
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