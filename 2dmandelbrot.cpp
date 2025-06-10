#include "console.h"
#include <stdio.h>
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


int wmain() {
	int cw = console::getConsoleWidth(), ch = console::getConsoleHeight();
	int chars = cw * ch;
	wchar_t fb[chars + 1];
	color_t cb[chars + 1];
	float pixtlx = 0;
	float pixtly = 0;
	float pixbrx = cw;
	float pixbry = ch;
	float fractlx = -2.0f;
	float fractly = -1.0f;
	float fracbrx = 1.0f;
	float fracbry = 1.0f;
	float vscalex = cw;
	float vscaley = ch;
	float offsetx = 0.0f;
	float offsety = 0.0f;
	fractlx = (pixtlx) / vscalex + 0.0f;
	fractly = (pixtly) / vscaley + 0.0f;
	fracbrx = (pixbrx) / vscalex + 0.0f;
	fracbry = (pixbry) / vscaley + 0.0f;
	
	while (true) {
		float xscale = (fracbrx - fractlx) / (float(pixbrx) - float(pixtlx));
		float yscale = (fracbry - fractly) / (float(pixbry) - float(pixtly));
		console::clear();
		for (int x = 0; x < cw; x++) {
			for (int y = 0; y < ch; y++) {
				float c_w = x * xscale + fractlx;
				float c_h = y * yscale + fractly;
				float zw = 0.0f;
				float zh = 0.0f;
				int n = 0;
				/*
				while (abs(zh) < 2.0f && abs(zw) < 2.0f && n < 128) {
					zw = (zw * zw) + c_w;
					zh = (zh * zh) + c_h;
					n++;
				}
				*/
				float tempx;
				while ((zw * zw + zh * zh < 4) && (n < 128)) {
					tempx = zw * zw - zh * zh + c_w;
					zh = 2.0f * zw * zh + c_h;
					zw = tempx;
					n++;
				}
				//console::write(x, y, L"#&*!;',."[n % 8], 7);
				fb[y * cw + x] = L"#&*!;',."[n % 8];
				cb[y * cw + x] = 7;
								
				//hsv hsv = { (n % 16) / 16.0f, n / 128.0f, 1.0d };
				//rgb rgb = hsv2rgb(hsv);
				//getDitherColored(rgb.r * 256, rgb.g * 256, rgb.b * 256, &fb[y * cw + x], &cb[y * cw + x]);
				float a = 0.1f;
				float p = 0.5f;
				float pp = 255.0f;
				getDitherColored((p * sin(a * n) + p) * pp, (p * sin(a * n + 2.094f) + p) * pp, (p * sin(a * n + 4.188f) + p) * pp, &fb[y * cw + x], &cb[y * cw + x]);
				
			}
		}
		
		console::write(fb, cb, chars - 1);
		
		switch (NOMOD(console::readKey())){
			case 's':
				offsety += 10.0f/vscaley;
				break;
			case 'w':
				offsety -= 10.0f/vscaley;
				break;
			case 'a':
				offsetx -= 10.0f/vscalex;
				break;
			case 'd':
				offsetx += 10.0f/vscalex;
				break;
			case 'z':
				vscalex *= 1.1f;
				vscaley *= 1.1f;
				break;
			case 'x':
				vscalex *= 0.9f;
				vscaley *= 0.9f;
				break;
			case VK_ESCAPE:
				return 0;
		}
		fractlx = (pixtlx) / vscalex + offsetx;
		fractly = (pixtly) / vscaley + offsety;
		fracbrx = (pixbrx) / vscalex + offsetx;
		fracbry = (pixbry) / vscaley + offsety;
	}
}
