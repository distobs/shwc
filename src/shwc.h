#ifndef SHWC_H
#define SHWC_H

#include <stdio.h>

#define report(func, err) fprintf(stderr, "%s:%d (%s): %s\n", __FILE__, \
		__LINE__, func, err)
#define trace(func) fprintf(stderr, "%s:%d (%s)\n", __FILE__, \
		__LINE__, func)

#endif