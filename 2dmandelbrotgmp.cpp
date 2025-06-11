#include <iostream>
#include <tuple>
#include <format>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <string.h>
#include <signal.h>
#include <gmp.h>
#include <math.h>
#include <vector>
#include <cassert>

#include "advancedConsole.h"
#include "colorMappingPalette.h"

using namespace ColorMappingPalette;
using cpix_t = pix_char;

mpf_t multiplicand;
mpf_t mpf_zero;

// X and Y absolute position
const inline float get_mandelbrot(const mpf_t &x, const mpf_t &y) {
	mpf_t zw, zh;
	mpf_t zww, zhh, zsum, ztemp;

	__mpf_struct *vals[6] = { &zw[0], &zh[0], &zww[0], &zhh[0], &zsum[0], &ztemp[0] };
	for (int i = 0; i < 6; i++) {
		//memcpy(vals[i], &mpf_zero[0], sizeof(__mpf_struct));
		mpf_init_set_si(vals[i], 0);
	}
	//mpf_inits(zw, zh, zww, zhh, zsum, ztemp, '\0');



	const int max_iterations = 256;
	int n = 0;

	while (1) {
		mpf_mul(zww, zw, zw);
		mpf_mul(zhh, zh, zh);
		mpf_add(zsum, zww, zhh);
		if (!(mpf_cmp_d(zsum, 4.0f) < 0 && n < 1024))
			break;
		mpf_sub(ztemp, zww, zhh);
		mpf_add(ztemp, ztemp, x);
		mpf_mul(zsum, zw, multiplicand);
		mpf_mul(zsum, zsum, zh);
		mpf_add(zsum, zsum, y);
		mpf_set(zw, ztemp);
		mpf_set(zh, zsum);
		n++;
	}

	float ret = n;
	// Interpolate
	if (n < max_iterations) {
		long oo = -1;
		double log_zn = (1.0 - mpf_get_d_2exp(&oo,zsum))/2.0;
		ret = float(logf64(log_zn / logf64(2)) / logf64(2));
		ret = n + 1 - ret;
	}

	return ret;
}

const inline cpix_t get_mandelbrot_cpix(const float &n) {
	cpix_t c;
	const float ooo[] = { 128.0f, 192.0f, 224.0f, 164.0f };
	float a = 0.1f, p = 0.5f, pp = ooo[int(n * (1/256.0f)) % 4];
	getDitherColored((p * sin (a * n) + p) * pp, 
			 (p * sin(a * n + 2.094f) + p) * pp, 
			 (p * sin(a * n + 4.188f) + p) * pp, 
			 &c.character, &c.color);
	return c;
}

const inline void get_mandelbrot_absolute(const mpf_t &frame_x, const mpf_t &frame_y, const mpf_t &scale_x, const mpf_t &scale_y, const float &frame_u, const float &frame_v, mpf_t &x, mpf_t &y) {
	mpf_t subtrahend, u, v;
	mpf_inits(x, y, '\0');
	mpf_init_set_d(subtrahend, 0.5f);
	mpf_init_set_d(u, frame_u);
	mpf_init_set_d(v, frame_v);
	mpf_mul(x, u, scale_x);
	mpf_mul(y, v, scale_y);
	//mpf_sub(x, frame_x, subtrahend);
	//mpf_sub(y, frame_y, subtrahend);
	mpf_add(x, x, frame_x);
	mpf_add(y, y, frame_y);
}

void scale_by_factor(const mpf_t &scale, mpf_t &pos, double factor=0.1, bool dir=true) {
	mpf_t t;
	mpf_init_set_d(t, factor);
	mpf_mul(t, scale, t);
	if (dir)
		mpf_add(pos, pos, t);
	else
		mpf_sub(pos, pos, t);
}

int thread_count = 4;
mp_bitcnt_t precision = 64;
bool run_ui, redraw, quit_request;
// Render from main thread
bool adv_offload = false;
std::mutex mutex_lock;
std::condition_variable cv_lock;
std::vector<std::thread> drawing_threads;

mpf_t frame_x, frame_y, scale_x, scale_y, addend, multiplicand_a, multiplicand_b;

void wait_for_cv() {
	std::unique_lock<std::mutex> lock(mutex_lock);
	cv_lock.wait(lock);
}

void draw_debug_info() {
	const int buflen = 200;
	char buf[buflen];
	int p = gmp_snprintf(buf, buflen, "x: %.Ff, y: %.Ff, scale: %.Ff", frame_x, frame_y, scale_x);
	snprintf(&buf[p], buflen-p, ", precision: %i", int(precision));

	adv::write(0,0,buf);
}

void draw_block(const int &x, const int &y, const cpix_t &c, const int &blocksize) {
	if (blocksize == 1) {
		adv::write(x, y, c.character, c.color);
	} else {
		for (int xi = 0; xi < blocksize; xi++) {
			for (int yi = 0; yi < blocksize; yi++) {
				adv::write(x+xi,y+yi,c.character,c.color);
			}
		}
	}
}

const inline cpix_t get_mandelbrot_sample(const float &u, const float &v) {
	mpf_t mx, my;
	get_mandelbrot_absolute(frame_x, frame_y, scale_x, scale_y, u - 0.5, v - 0.5, mx, my);
	const float ms = get_mandelbrot(mx, my);
	return get_mandelbrot_cpix(ms);
}

void draw_thread(float startx, float starty, float sizex, float sizey, int flag) {
	fprintf(stderr, "thread: %i\n", flag);
	
	while (run_ui) {
		//adv::clear();

		//draw_debug_info();

		double w = adv::width, h = adv::height;
		int sx = startx * w, sy = starty * h;
		int ex = sizex * w + sx, ey = sizey * h + sy;

		//int bsinit = 2 * 2 * 2;
		int bsinit = 2;
		for (float bs = bsinit; bs >= 1; bs /= 2) {
			for (int y = sy; y < ey; y+=bs) {
				for (int x = sx; x < ex; x+=bs) {
					draw_block(x, y, get_mandelbrot_sample(x/w,y/h), bs);
					
					if (redraw & flag) {
						x = 0;
						y = 0;
						bs = bsinit;
						redraw &= ~flag;
					}
				}
			}
		}
		
		if (!(redraw & flag))
			wait_for_cv();
		else {
			redraw &= ~flag;
		}
	}
}

void params_change() {
	redraw = 0xffffffff;
	cv_lock.notify_all();
}

void reset() {
	mpf_set_default_prec(precision);
	mpf_init_set_d(frame_x, -0.721439766601316804051f);
	mpf_init_set_d(frame_y, 0.259233725333566523653f);
	mpf_init_set_d(scale_x, 0.79560530175511f);
	mpf_init_set_d(scale_y, 0.79560530175511f);
	mpf_init_set_d(addend, 1.0f);
	mpf_init_set_d(multiplicand_a, 1.5f);
	mpf_init_set_d(multiplicand_b, 0.5f);
	mpf_init_set_d(multiplicand, 2.0);
	mpf_init_set_si(mpf_zero, 0);
	params_change();
}

void handle_signal(int signal) {
	quit_request = true;
}

void set_precision(mp_bitcnt_t precision) {
	mpf_set_default_prec(precision);
	__mpf_struct *vals[] = { &frame_x[0], &frame_y[0], &scale_x[0], &scale_y[0], &addend[0], &multiplicand[0], &multiplicand_a[0], &multiplicand_b[0] };
	for (int i = 0; i < sizeof(vals[0]) / sizeof(vals); i++)
		mpf_set_prec(vals[i], precision);
	params_change();
}

void start_threads() {
	if (int(sqrt(thread_count)) != sqrt(thread_count)) {
		assert("WAHT");
	}
	double tpy = sqrt(thread_count);
	double tsy = 1.0 / tpy;
	double tsx = 1.0 / tpy;
	int i = 0;
	for (int x = 0; x < tpy; x++) {
		for (int y = 0; y < tpy; y++) {
			drawing_threads.push_back(std::thread(draw_thread, double(x)/tpy, double(y)/tpy, tsx, tsy, 1 << i));
			i++;
		}
	}
}

void init() {
	adv::setThreadState(adv_offload);
	adv::setThreadSafety(adv_offload);
	adv::setDoubleWidth(false);
	colormapper_init_table();

	reset();

	run_ui = true;
	redraw = 0;
	quit_request = false;

	start_threads();

	signal(SIGINT, handle_signal);
	signal(SIGQUIT, handle_signal);
	signal(SIGKILL, handle_signal);
	signal(SIGTERM, handle_signal);
}

void join_threads() {
	for (auto &thread : drawing_threads)
		//if (thread.joinable())	
			thread.join();
}

void destroy() {
	run_ui = false;

	cv_lock.notify_all();
	join_threads();

	adv::_advancedConsoleDestruct();
	console::cons.~constructor();
}

void input_loop() {
	int key = 0;

	while (run_ui && !quit_request) {
		switch (NOMOD(key)) {
			case 'd':
				scale_by_factor(scale_x, frame_x);
				//mpf_add(frame_x, frame_x, addend);
				//mpf_add(frame_x, frame_x, scale_x);
				params_change();
				break;
			case 's':
				scale_by_factor(scale_y, frame_y);
				//mpf_add(frame_y, frame_y, addend);
				//mpf_add(frame_y, frame_y, scale_y);
				params_change();
				break;
			case 'a':
				scale_by_factor(scale_x, frame_x, 0.1, false);
				//mpf_sub(frame_x, frame_x, addend);
				//mpf_sub(frame_x, frame_x, scale_x);
				params_change();
				break;
			case 'w':
				scale_by_factor(scale_y, frame_y, 0.1, false);
				//mpf_sub(frame_y, frame_y, addend);
				//mpf_sub(frame_y, frame_y, scale_y);
				params_change();
				break;
			case 'z':
			case ',':
				mpf_mul(scale_x, scale_x, multiplicand_a);
				mpf_mul(scale_y, scale_y, multiplicand_a);
				params_change();
				break;
			case 'x':
			case '.':
				mpf_mul(scale_x, scale_x, multiplicand_b);
				mpf_mul(scale_y, scale_y, multiplicand_b);
				params_change();
				break;
			case '1':
				if (precision > 32)
					set_precision(precision -= 8);
				break;
			case '2':	
				set_precision(precision += 8);
				break;
			case '0':
				reset();
				break;
		}

		if (HASKEY(key, VK_ESCAPE) || HASKEY(key, 'q') || quit_request)
			break;

		adv::isNewSize();

		//draw_debug_info();

		if (!adv_offload)
			adv::draw();

		key = console::readKeyAsync();

		adv::waitForReadyFrame();
	}
}

int main() {
	init();
	input_loop();
	destroy();

	return 0;
}
