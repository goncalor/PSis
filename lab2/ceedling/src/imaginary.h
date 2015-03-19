#ifndef imaginary_H
#define imaginary_H

typedef struct imag {
	int re, im;
} imag;

typedef char* str;

imag atoimag(char *);
str * imagtoa(imag i);
imag imag_mult(imag i1, imag i2);

#endif // imaginary_H
