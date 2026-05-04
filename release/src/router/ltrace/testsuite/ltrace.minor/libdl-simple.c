#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

int main(int argc, char **argv) {
	void *handle;
	int (*test)(int);
	char *error;

	handle = dlopen ("liblibdl-simple.so", RTLD_LAZY);
	if (!handle) {
		fputs (dlerror(), stderr);
		exit(1);
	}

	test = dlsym(handle, "test_libdl");
	if ((error = dlerror()) != NULL)  {
		fputs(error, stderr);
		exit(1);
	}

	printf("%d\n", test(5));
	dlclose(handle);
	return 0;
}
