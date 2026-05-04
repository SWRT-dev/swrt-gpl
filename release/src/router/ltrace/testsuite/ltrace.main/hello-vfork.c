/* Copyright (C) 2008, Red Hat, Inc.
 * Written by Denys Vlasenko */
#include <stdio.h>
#include <unistd.h>

int main() {
        int r = vfork();
        fprintf(stdout, "vfork():%d\n", r);
        _exit(0);
}

