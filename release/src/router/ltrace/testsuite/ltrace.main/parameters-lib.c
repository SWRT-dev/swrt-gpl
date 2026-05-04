#include <string.h>
#include <stdio.h>

void func_ignore(int a, int b, int c)
{
	printf("%d\n", a + b + c);
}

void func_intptr(int *i)
{
	printf("%d\n", *i);
}

void func_intptr_ret(int *i)
{
	*i = 42;
}

int func_strlen(char* p)
{
	strcpy(p, "Hello world");
	return strlen(p);
}

int func_arg0(char *p)
{
	strcpy(p, "Hello another world!");
	return strlen(p);
}

void func_strfixed(char* p)
{
	strcpy(p, "Hello world");
}

void func_string(char* p)
{
	printf("%s\n", p);
}

void func_ppp(int*** ppp)
{
	printf("%d\n", ***ppp);
}

void func_stringp(char** sP)
{
	printf("%s\n", *sP);
}

void func_enum(int x)
{
	printf("enum: %d\n", x);
}

void func_short(short x1, short x2)
{
	printf("short: %hd %hd\n", x1, x2);
}

void func_ushort(unsigned short x1, unsigned short x2)
{
	printf("ushort: %hu %hu\n", x1, x2);
}

float func_float(float f1, float f2)
{
	printf("%f %f\n", f1, f2);
	return f1;
}

double func_double(double f1, double f2)
{
	printf("%f %f\n", f1, f2);
	return f2;
}

void func_typedef(int x)
{
	printf("typedef'd enum: %d\n", x);
}

void func_arrayi(int* a, int N)
{
    int i;
    printf("array[int]: ");
    for (i = 0; i < N; i++)
	printf("%d ", a[i]);
    printf("\n");
}

void func_arrayf(float* a, int N)
{
    int i;
    printf("array[float]: ");
    for (i = 0; i < N; i++)
	printf("%f ", a[i]);
    printf("\n");
}

struct test_struct {
    int simple;
    int alen;
    int slen;
    struct { int a; int b; }* array;
    struct { int a; int b; } seq[3];
    char* str;
    char* outer_str;
};

void func_struct(struct test_struct* x)
{
    char buf[100];
    int i;

    printf("struct: ");

    printf("%d, [", x->simple);
    for (i = 0; i < x->alen; i++) {
	printf("%d/%d", x->array[i].a, x->array[i].b);
	if (i < x->alen - 1)
	    printf(" ");
    }
    printf("] [");
    for (i = 0; i < 3; i++) {
	printf("%d/%d", x->seq[i].a, x->seq[i].b);
	if (i < 2)
	    printf(" ");
    }
    printf("] ");

    strncpy(buf, x->str, x->slen);
    buf[x->slen] = '\0';
    printf("%s\n", buf);
}

void func_work (char *x)
{
  *x = 'x';
}

void func_call (char *x, char* y, void (*cb) (char *))
{
  cb (y);
  *x = (*y)++;
}

struct S2 {
	float f;
	char a;
	char b;
};

struct S3 {
	char a[6];
	float f;
};

struct S2
func_struct_2(int i, struct S3 s3, double d)
{
	return (struct S2){ s3.f, s3.a[1], s3.a[2] };
}

struct S4 {
	long a;
	long b;
	long c;
	long d;
};

struct S4
func_struct_large(struct S4 a, struct S4 b)
{
	return (struct S4){ a.a + b.a, a.b + b.b, a.c + b.c, a.d + b.d };
}

struct S5 {
	char a;
	char b;
	long c;
	long d;
};

struct S5
func_struct_large2(struct S5 a, struct S5 b)
{
	return (struct S5){ a.a + b.a, a.b + b.b, a.c + b.c, a.d + b.d };
}

struct S6 {
	long a;
	long b;
	char c;
	char d;
};

struct S6
func_struct_large3(struct S6 a, struct S6 b)
{
	return (struct S6){ a.a + b.a, a.b + b.b, a.c + b.c, a.d + b.d };
}

void
func_many_args(int a, int b, long c, double d, char e, int f, float g, char h,
	       int i, double j, int k, double l, char m, int n, short o, int p,
	       char q, float r, float s, double t, long u, float v, float w,
	       float x, float y)
{
}

void
func_lens(int a, long b, short c, long d)
{
}

int
func_bool(int a, int b)
{
	return !b;
}

void
func_hide(int a, int b, int c, int d, int e, int f)
{
}

struct func_hide_struct {
	int a; int b; int c; int d; int e; int f; int g; int h;
};

void
func_hide_struct(struct func_hide_struct s)
{
}

long *
func_short_enums(short values[])
{
	static long retvals[4];
	retvals[0] = values[0];
	retvals[1] = values[1];
	retvals[2] = values[2];
	retvals[3] = values[3];
	return retvals;
}

long
func_negative_enum(short a, unsigned short b, int c, unsigned d,
		   long e, unsigned long f)
{
	return -1;
}

void
func_charp_string(char *p)
{
}

struct struct_empty {};
struct struct_size1 { char a; };
struct struct_size2 { short a; };
struct struct_size4 { int a; };
struct struct_size8 { int a; int b; };

struct struct_empty
func_struct_empty(struct struct_empty e)
{
	return e;
}

struct struct_size1
func_struct_size1(struct struct_size1 e)
{
	return e;
}

struct struct_size2
func_struct_size2(struct struct_size2 e)
{
	return e;
}

struct struct_size4
func_struct_size4(struct struct_size4 e)
{
	return e;
}

struct struct_size8
func_struct_size8(struct struct_size8 e)
{
	return e;
}

void
func_printf(char *format, ...)
{
}

void
func_sprintf(char *str, char *format, ...)
{
}
