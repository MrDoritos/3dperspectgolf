#include <iostream>

#include <gmp.h>

int main() {
    mpf_t a, b;

    mpf_init_set_si(a, 1);
    mpf_init_set_si(b, 2);

    mpf_add(a, a, b);

    mpf_out_str(stdout, 10, 10, a);

    return 0;
}