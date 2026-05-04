#pragma once

#include <stdio.h>
#include <complex.h>

// ltrace doesn't know about these yet, so turn off the tests
#define UNIONTEST_DISABLE
#define COMPLEXTEST_DISABLE

struct list
{
    int x;
    struct list* next;
};
void linkedlisttest( struct list* l );

struct tree
{
    int x;
    struct tree* left;
    struct tree* right;
};
struct tree treetest(struct tree* t);

struct loop_a;
struct loop_b;
typedef struct loop_a { struct loop_b*   b; int x;} loop_a_t;
        struct loop_b {        loop_a_t* a; int x;};
void looptest( loop_a_t* a );

enum E { A,B,C };
typedef enum E E_t;
int enumtest( enum E a, E_t b );

#ifndef UNIONTEST_DISABLE
union U { int a, b; };
int uniontest( union U u1, union U u2 );
#endif

struct witharray
{
    double x[5];
};
double arraytest( struct witharray* s );

#ifndef COMPLEXTEST_DISABLE
double complex complextest(const float complex* x);
#endif

int stringtest(char* str);

void FILEtest( FILE* a, const FILE* b );

void voidfunc(void);

void printtree( const struct tree* tree, int indent );
