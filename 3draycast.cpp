#include "console.h"

#include <math.h>

#include "../imgcat/colorMappingFast.h"


typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}

float sampleMandelBrot(float _x, float _y) {
	//float x,y=-1.5,u,w,v;
	float u,w,v;
	float x = _x /40;// - 3.0f;
	float y = _y /40;// - 1.5f;		
	int n=u=w=0;
	while(u*u+w*w<4&&n++<16){
		v=u*u-w*w+x;
		w=2*u*w+y;
		u=v;
	}
	//return 1.0f - ((n % 8) * 0.125f);
	return 1.0f - (n / 16.0f);
	
	//return float(n + 1) / 65.0f;
	//return float(n) / 256.0f;
	//return (((sin(x * 0.5f) + sin(y * 0.5f)) * 0.5f) + 1) * 0.5f;	
	//return rand() % 3 == 1 ? 1.0f : 0.0f;
}

float sampleMap(float x, float y) {
	return sampleMandelBrot(x,y);
	//return (sin(x * 0.5f) + 1) * 0.5f;
	return (((sin(x * 0.5f) + sin(y * 0.5f)) * 0.5f) + 1) * 0.5f;
	//return rand() % 3 == 1 ? 1.0f : 0.0f;
	//return 0.95f;
}

int wmain() {
	float playerX = 0.0f, playerY = 0.0f, playerA = 0.7854f;
	float horizon = 0.90f, displayScale = 20.0f;
	int cwidth = console::getConsoleWidth();
	int cheight = console::getConsoleHeight();
	
	int chars = cwidth * cheight;
	wchar_t fb[chars + 1];
	color_t cb[chars + 1];
	
	//near
	//big   //1 - 1/100 / 1/100*2 = 1 - 0.01 / 0.01 * 2 = 0.99 / 0.02 = 49.5 = 0.0202
	//small //.1- 1/100 / 1/100*2 =.1 - 0.01 / 0.01 * 2 = 0.09 / 0.02 = 4.5  = 0.2222
	//big		//1 / 1/100 = 100
	//small		//.1/ 1/100 = 10
	//far
	//big   //1 - 99/100 / 99/100*2 = 1 - .99 / .99 * 2 = 0.01 / 1.98 = 0.005= 200
	//small //.1- 99/100 / 99/100*2 =.1 - .99 / .99 * 2 =-0.89 / 1.98 =-0.449=-2.2247
	//big		//1 / 99/100= 1.01
	//small		//.1/ 99/100= 0.101
	
	//bigfar   //100 - 99 / 50 = 0.01
	//smallfar //100 - 1 / 50  = 1.95
	//bignear  //100 - 99 / 0.5= 2
	//smallnear//100 - 1 / 0.5 = 198
	
	//far  //100 - 0.5 * 100 / 100/100 = 50
	//near //100 - 0.5 * 100 / 1/100   = 0.5
	
	
	
	while (true) {
		//memset(&fb[0], L' ', chars * 2 - 1);
		//memset(&cb[0], FWHITE|BBLACK, chars * 2 - 1);
		for (int i = 0; i < chars; i++) {
			fb[i] = L' ';
			cb[i] = FWHITE|BBLACK;
		}
		
		
		playerX += 0.25f;
		playerY += 0.25f;
		//playerA += 0.00001f;
		float depthMax = 50.0f;
		for (float depth = depthMax; depth >= 1.0f; depth -= 0.5f) {
			float normalDepth = depth / depthMax;
			double angle = (depth / depthMax) * 180.0d;
			wchar_t character; color_t color;
			for (int x = 0; x < cwidth; x++) {
				float viewingAngle = (float(x) / cwidth) * 1.570795f - 0.7854f;
				float castX = sinf(playerA + viewingAngle) * depth + playerX;
				float castY = -(cosf(playerA + viewingAngle) * depth) + playerY;				
				//if (sample < 0.0f) sample = 0.0f;
				//if (sample > 1.0f) sample = 1.0f;
				//for (int y = cheight - (sample * cheight * 0.5f); y < cheight-1; y++) {
				float sample = ((float(cheight) - ((sampleMap(castX, castY) / displayScale + horizon) * float(cheight))) / (depth / depthMax));
				//if (sample > cheight) sample = cheight;
				//if (sample < -cheight) sample = -cheight;
				
			hsv hsv = { 0.5d, 1.0d - ((1.0d - sampleMap(castX, castY)) * normalDepth), 1.0d };
			rgb rgb = hsv2rgb(hsv);
			getDitherColored(rgb.r * 256, rgb.g * 256, rgb.b * 256, &character, &color);
				
				for (int y = (float(cheight) / 2.0f) + sample; y < cheight-1; y++) {
					if (y > cheight || y < 0)
						continue;
					//console::write(x, y, "ABCDEFGHIJKLMNOPQRSTUVWXYZ."[int((depth / depthMax) * 26.0f)]);
					//fb[y * cwidth + x] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,\"^`'.  "[int((depth / depthMax) * 71.0f)];

					cb[y * cwidth + x] = color;
					fb[y * cwidth + x] = character;
					//cb[y * cwidth + x] = FWHITE|BBLACK;
					//console::write(x, y, fb[y * cwidth + x], cb[y * cwidth + x]);
				}
			}
			//console::sleep(1);
		}
		
		//console::sleep(2000);
		//console::sleep(33);
		console::write(fb, cb, chars - 1);
		
		switch (NOMOD(console::readKey())) {
			case 'a':
				playerX += sinf(playerA - 1.5708) * 0.5f;
				playerY -= cosf(playerA - 1.5708) * 0.5f;
				break;
			case 'd':
				playerX += sinf(playerA + 1.5708) * 0.5f;
				playerY -= cosf(playerA + 1.5708) * 0.5f;
				break;
			case 's':
				playerX -= sinf(playerA) * 0.5f;
				playerY += cosf(playerA) * 0.5f;
				break;
			case 'w':
				playerX += sinf(playerA) * 0.5f;
				playerY -= cosf(playerA) * 0.5f;
				break;
			case '7':
				horizon += 0.01f;
				break;
			case '4':
				horizon -= 0.01f;
				break;
			case '8':
				displayScale	+= 1.0f;
				break;
			case '5':
				displayScale -= 1.0f;
				break;
			case '.':
				playerA -= 3.1416f / 8.0f;
				break;
			case ',':
				playerA += 3.1416f / 8.0f;
				break;
			case VK_ESCAPE:
				return 0;
				break;
		}
	}	
}