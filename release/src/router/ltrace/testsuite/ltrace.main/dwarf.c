#include "dwarflib.h"
#include "dwarflib_cxx.hh"
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

void main(void)
{
    struct list l = { .x = 5, .next = NULL };
    linkedlisttest( &l );

    struct tree d = {.x = 4};
    struct tree c = {.x = 3, .right = &d};
    struct tree b = {.x = 2};
    struct tree a = {.x = 1, .left = &b, .right = &c};
    treetest( &a );

    struct loop_a la = {.x = 5};
    struct loop_b lb = {.x = 6};
    la.b = &lb;
    lb.a = &la;
    looptest(&la);

    enum E ea = A, eb = B;
    enumtest( ea, eb );

#ifndef UNIONTEST_DISABLE
    union U u1 = {.a = 5};
    union U u2 = {.a = 6};
    uniontest( u1, u2 );
#endif

    struct witharray s = {.x = {1.0,2.0,1.0,2.0,1.0}};
    arraytest( &s );

#ifndef COMPLEXTEST_DISABLE
    float complex x = 3 + 4*I;
    complextest(&x);
#endif

    enum_cxx_test();

    stringtest("test");
    stringtest(NULL);

    FILEtest( stdout, stderr );

    voidfunc();

    // tests aliased symbols. On my amd64 Debian/sid box this generates a
    // reference to nanosleep, while the debug symbols have __nanosleep and
    // __GI___nanosleep
	usleep(33);
	nanosleep(&(struct timespec){.tv_nsec = 44}, NULL);
}
