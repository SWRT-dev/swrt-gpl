#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dwarflib.h"

void printtree( const struct tree* tree, int indent )
{
    if( tree == NULL )
    {
        printf("%*sNULL\n", indent*4, "");
    }
    else
    {
        printf("%*s%d\n", indent*4, "", tree->x);
        printtree( tree->left,  indent+1 );
        printtree( tree->right, indent+1 );
    }
}


void linkedlisttest( struct list* l )
{
}

struct tree treetest(struct tree* t)
{
    if(t->left  != NULL) treetest(t->left);
    if(t->right != NULL) treetest(t->right);
    t->x++;

    return *t;
}

void looptest( loop_a_t* a )
{
    a->x++;
    a->b->x++;
    ftell(stdin);
}

int enumtest( enum E a, E_t b )
{
    return a == b;
}

#ifndef UNIONTEST_DISABLE
int uniontest( union U u1, union U u2 )
{
    u1.a = u2.b;
    return u1.a;
}
#endif

double arraytest( struct witharray* s )
{
    return s->x[0];
}

#ifndef COMPLEXTEST_DISABLE
double complex complextest(const float complex* x)
{
    double complex y = (double complex)(*x);
    y *= 2;
    return y;
}
#endif

int stringtest(char* str)
{
    if(str == NULL) return -1;
    return strlen(str);
}

void FILEtest( FILE* a, const FILE* b )
{
}

void voidfunc(void)
{
}
