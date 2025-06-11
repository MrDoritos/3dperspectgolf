#include <iostream>
#include <tuple>
#include <format>
#include <gmp.h>

#include "advancedConsole.h"
#include "colorMappingPalette.h"

using namespace ColorMappingPalette;
using cpix_t = pix_char;

// X and Y absolute position
const inline int get_mandelbrot(const mpf_t &x, const mpf_t &y) {
	mpf_t zw, zh;
	mpf_t zww, zhh, zsum, ztemp;
	mpf_t multiplicand;
	mpf_inits(zw, zh, zww, zhh, zsum, '\0');
	mpf_init_set_si(multiplicand, 2);
	int n = 0;

	while (1) {
		mpf_mul(zww, zw, zw);
		mpf_mul(zhh, zh, zh);
		mpf_add(zsum, zww, zhh);
		if (mpf_cmp_d(zsum, 4) < 0 && n < 128)
			break;
		mpf_sub(ztemp, zww, zhh);
		mpf_add(ztemp, zsum, x);
		mpf_mul(zsum, zh, multiplicand);
		mpf_mul(zsum, zsum, zw);
		mpf_add(zsum, zsum, y);
		mpf_set(zw, ztemp);
		mpf_set(zh, zsum);
		n++;
	}
	return n;
}

const inline cpix_t get_mandelbrot_cpix(const int &n) {
	cpix_t c;
	float a = 0.1f, p = 0.5f, pp = 255.0f;
	getDitherColored((p * sin (a * n) + p) * pp, 
			 (p * sin(a * n + 2.094f) + p) * pp, 
			 (p * sin(a * n + 4.188f) + p) * pp, 
			 &c.character, &c.color);
	return c;
}

const inline void get_mandelbrot_absolute(const mpf_t &frame_x, const mpf_t &frame_y, const mpf_t &scale_x, const mpf_t &scale_y, mpf_t &x, mpf_t &y) {
	mpf_t subtrahend;
	mpf_inits(x, y, '\0');
	mpf_init_set_si(subtrahend, 1);
	mpf_sub(x, frame_x, subtrahend);
	mpf_sub(y, frame_y, subtrahend);
	mpf_mul(x, x, scale_x);
	mpf_mul(y, y, scale_y);
}

int main() {
	adv::setThreadState(false);
	adv::setThreadSafety(false);
	adv::setDoubleWidth(false);
	colormapper_init_table();

	mpf_t frame_x, frame_y, scale_x, scale_y, addend, multiplicand_a, multiplicand_b;

	mpf_init_set_d(frame_x, 0.0f);
	mpf_init_set_d(frame_y, 0.0f);
	mpf_init_set_d(scale_x, 1.0f);
	mpf_init_set_d(scale_y, 1.0f);
	mpf_init_set_d(addend, 1.0f);
	mpf_init_set_d(multiplicand_a, 1.1f);
	mpf_init_set_d(multiplicand_b, 0.9f);
	int key = 0;

	while (1) {
		switch (NOMOD(key)) {
			case 's':
				mpf_add(frame_x, frame_x, addend);
				break;
			case 'w':
				mpf_add(frame_y, frame_y, addend);
				break;
			case 'a':
				mpf_sub(frame_x, frame_x, addend);
				break;
			case 'd':
				mpf_sub(frame_y, frame_y, addend);
				break;
			case 'z':
			case ',':
				mpf_mul(scale_x, scale_x, multiplicand_a);
				mpf_mul(scale_y, scale_y, multiplicand_a);
				break;
			case 'x':
			case '.':
				mpf_mul(scale_x, scale_x, multiplicand_b);
				mpf_mul(scale_y, scale_y, multiplicand_b);
				break;
		}

		if (HASKEY(key, VK_ESCAPE) || HASKEY(key, 'q'))
			break;

		adv::isNewSize();
		adv::clear();

		for (int y = 0; y < adv::height; y++) {
			for (int x = 0; x < adv::width; x++) {
				double u = double(x) / adv::width;
				double v = double(y) / adv::height;

				mpf_t frame_xx, frame_yy;
				mpf_init_set_si(frame_xx, u);
				mpf_init_set_si(frame_yy, v);
				mpf_add(frame_xx, frame_xx, frame_x);
				mpf_add(frame_yy, frame_yy, frame_y);

				mpf_t mx, my;
				get_mandelbrot_absolute(frame_xx, frame_yy, scale_x, scale_y, mx, my);

				const int ms = get_mandelbrot(mx, my);

				const cpix_t c = get_mandelbrot_cpix(ms);

				adv::write(x, y, c.character, c.color);
			}
		}

		const int buflen = 200;
		char buf[buflen];
		snprintf(buf, buflen, 
			"frame_x: %lf, frame_y: %lf, scale_x: %lf, scale_y: %lf, window_width: %i, window_height: %i",
			mpf_get_d(frame_x), mpf_get_d(frame_y),
			mpf_get_d(scale_x), mpf_get_d(scale_y),
			adv::width, adv::height);

		adv::write(0,0,buf);

		adv::draw();

		key = console::readKey();
	}

	adv::_advancedConsoleDestruct();
	console::cons.~constructor();

	return 0;
}
