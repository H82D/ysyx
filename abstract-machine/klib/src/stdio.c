#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
	va_list ap;
	int j;
	char buf[4096] = {0};
	va_start(ap, fmt);
	j = vsprintf(buf, fmt, ap);
	for (int i = 0; buf[i]; i++) {
		putch(buf[i]);
	}
	va_end(ap);
	return j;
}

static void parse_format(char *ins, int n, char *flags, int *width, int *precision, char *length, char *specifier) {
	*specifier = ins[n];
	int i = 0;
	if (ins[i++] != '%') panic("invalid ins\n");
	switch (ins[i]) {
		case '-':
		case '+':
		case '0':
		case '#':
			*flags = ins[i];
			i++;
			break;
		default:
			*flags = ' ';
			break;
	}
	*width = 0;
	while (ins[i] >= '0' && ins [i] <= '9') {
		*width = *width * 10 + ins[i] - '0';
		i++;
	}
	*precision = 0;
	if (ins[i] == '.') {
		i++;
		while (ins[i] >= '0' && ins[i] <= '9') {
			*precision = *precision * 10 + ins[i] - '0';
			i++;
		}
	}
	switch (ins[i]) {
		case 'h':
		case 'l':
		case 'L':
			*length = ins[i];
			i++;
		default:
			*length = ' ';
			break;
	}
}

int vsprintf(char *out, const char *fmt, va_list ap) {
	char flags, length, specifier;
	int width, precision;
	int i = 0, j = 0, k = 0;
	int n = 19;
	uint32_t d;
	union u_f {
		double f;
		long int d;
	};
	char ins[32], tmp[20];
	while (fmt[i]) {
		if (fmt[i] == '%') {
			k = 0;
			while (fmt[i] != 'd' && fmt[i] != 's' && fmt[i] != 'x' && fmt[i] != 'c') {
				ins[k++] = fmt[i++];
				if (fmt[i] == '\0') panic("invalid format\n");
			}
			ins[k] = fmt[i++];
			parse_format(ins, k, &flags, &width, &precision, &length, &specifier);
			switch (specifier) {
				case 's':
					char *s = va_arg(ap, char*);
					for (k = 0; s[k]; k++) {
						out[j++] = s[k];
					}
					break;
				//case 'f':
				case 'd':
				case 'x':
					//if (specifier == 'f') {
						//u_f.f = va_arg(ap, double);
						//d = u_f.d;
					//}
					//else d = va_arg(ap, int);
					d = va_arg(ap, int);
					n = 19;
					if (d == 0) {
						tmp[n--] = '0';
					}
					while (d) {
						if (specifier == 'd') {
							tmp[n--] = d % 10 + '0';
							d = d / 10;
						}
						if (specifier == 'x') {
							tmp[n--] = (d & 0xf) + (((d & 0xf) < 10) ? '0' : ('a' - 10));
							d = d >> 4;
						}
					}
					while (19 - n < width) {
						tmp[n--] = flags;
					}
					for (k = n + 1; k <= 19; k++) {
						out[j++] = tmp[k];
					}
					break;
				case 'c':
					char c = va_arg(ap, int);
					out[j++] = c;
					break;
				default:
					panic("unsupported format\n");
					break;
			}
		}
		else {
			out[j++] = fmt[i++];
		}
	}
	out[j] = '\0';
	return j;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	int j;
	va_start(ap, fmt);
	j = vsprintf(out, fmt, ap);
	va_end(ap);
	return j;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
