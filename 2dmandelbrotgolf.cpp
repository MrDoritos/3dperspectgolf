#include "console.h"
#include <stdio.h>
#include <math.h>

int wmain() {
	int cw = console::getConsoleWidth(), ch = console::getConsoleHeight();
	int chars = cw * ch;
	wchar_t fb[chars + 1];
	color_t cb[chars + 1];
	float pixtlx = 0;
	float pixtly = 0;
	float pixbrx = cw;
	float pixbry = ch;
	float fractlx = -3.0f;
	float fractly = -1.5f;
	float fracbrx = 1.0f;
	float fracbry = 1.0f;
	float vscalex = cw;
	float vscaley = ch;
	float offsetx = 0.0f;
	float offsety = 0.0f;
	
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
				float tempx;
				while ((zw * zw + zh * zh < 4) && (n < 128)) {
					tempx = zw * zw - zh * zh + c_w;
					zh = 2.0f * zw * zh + c_h;
					zw = tempx;
					n++;
				}
				console::write(x, y, L"#&*!;',."[n % 8], 7);
			}
		}
				
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
