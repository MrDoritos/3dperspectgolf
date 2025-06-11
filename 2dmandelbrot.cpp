//#include "console.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <vector>

#include "console.h"
#include "colorMappingPalette.h"

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

using mb_t = long double;
//typedef double mb_t;

int wmain() {
	colormapper_init_table();

	int cw = console::getConsoleWidth(), ch = console::getConsoleHeight();
	int chars = cw * ch;
	wchar_t fb[chars + 1];
	color_t cb[chars + 1];
	//float pixtlx = 0;
	//float pixtly = 0;
	//float pixbrx = cw;
	//float pixbry = ch;
	//mb_t fractlx = -2.0f;
	//mb_t fractly = -1.0f;
	//mb_t fracbrx = 1.0f;
	//mb_t fracbry = 1.0f;
	//mb_t vscalex = cw;
	//mb_t vscaley = ch;
	//mb_t offsetx = 0.0f;
	//mb_t offsety = 0.0f;
	//fractlx = (pixtlx) / vscalex + 0.0;
	//fractly = (pixtly) / vscaley + 0.0;
	//fracbrx = (pixbrx) / vscalex + 0.0;
	//fracbry = (pixbry) / vscaley + 0.0;
	mb_t frame_x = 0, frame_y = 0, scale_x = 1, scale_y = 1;
	mb_t width = console::getConsoleWidth(), height = console::getConsoleHeight();

	while (true) {
		//mb_t xscale = (fracbrx - fractlx) / mb_t(float(pixbrx) - float(pixtlx));
		//mb_t yscale = (fracbry - fractly) / mb_t(float(pixbry) - float(pixtly));
		console::clear();
		for (int x = 0; x < cw; x++) {
			for (int y = 0; y < ch; y++) {
				mb_t u = mb_t(x)/width-0.5, v = mb_t(y)/height-0.5;
				mb_t c_w = scale_x * u + frame_x;
				mb_t c_h = scale_y * v + frame_y;
				mb_t zx = 0.0, zy = 0.0;
				mb_t zx2 = 0.0, zy2 = 0.0;
				int n = 0;

				mb_t tempx;
				while (1) {
					zx2 = zx * zx, zy2 = zy * zy;
					if (zx2 + zy2 >= 4.0 || n > 255)
						break;
					tempx = zx2 - zy2 + c_w;
					zy = 2.0 * zx * zy + c_h;
					zx = tempx;
					n++;
				}

				float a = 0.1f;
				float p = 0.5f;
				float pp = 255.0f;
				getDitherColored((p * sin(a * n) + p) * pp, (p * sin(a * n + 2.094f) + p) * pp, (p * sin(a * n + 4.188f) + p) * pp, &fb[y * cw + x], &cb[y * cw + x]);
				
			}
		}
		
		console::write(fb, cb, chars - 1);
		
		switch (NOMOD(console::readKey())){
			case 's':
				frame_y += scale_y * 0.1;
				break;
			case 'w':
				frame_y -= scale_y * 0.1;
				break;
			case 'a':
				frame_x -= scale_x * 0.1;
				break;
			case 'd':
				frame_x += scale_x * 0.1;
				break;
			case '.':
			case 'x':
				scale_x *= 1.5;
				scale_y *= 1.5;
				break;
			case ',':
			case 'z':
				scale_x *= 0.66667;
				scale_y *= 0.66667;
				break;
			case 'q':
			case VK_ESCAPE:
				return 0;
		}
		//fractlx = (pixtlx) / vscalex - 0.5 + offsetx;
		//fractly = (pixtly) / vscaley - 0.5 + offsety;
		//fracbrx = (pixbrx) / vscalex - 0.5 + offsetx;
		//fracbry = (pixbry) / vscaley - 0.5 + offsety;
	}
}
