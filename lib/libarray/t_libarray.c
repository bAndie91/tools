
#include <err.h>

#include "libarray.h"

array_loop_control
pprint(size_t index, char * item, void * x)
{
	warnx("[%d]=%s", index, item);
	return ARRAY_LOOP_CONTINUE;
}

int main(int argc, char* argv[])
{
	Array* arr;
	
	size_t x;
	x=-1;
	warnx("x=%d=%u x%s0", x, x, x>0?">":"<=");
	
	warnx("init");
	array_init(&arr, 0);
	array_foreach(&arr, 0, pprint, NULL);
	warnx("");
	
	// test append function

	warnx("append venus");
	array_append(&arr, "venus");
	array_foreach(&arr, 0, pprint, NULL);
	warnx("");
	
	warnx("append mars");
	array_append(&arr, "mars");
	array_foreach(&arr, 0, pprint, NULL);
	warnx("");
	
	// test insert function

	warnx("insert 0 mercury");
	array_insert(&arr, 0, "mercury");
	array_foreach(&arr, 0, pprint, NULL);
	warnx("");
	
	warnx("insert 2 earth");
	array_insert(&arr, 2, "earth");
	array_foreach(&arr, 0, pprint, NULL);
	warnx("");
	
	warnx("insert 50 oort");
	array_insert(&arr, 50, "oort");
	array_foreach(&arr, 0, pprint, NULL);
	warnx("");
	
	warnx("insert 4 jupiter");
	array_insert(&arr, 4, "jupiter");
	array_foreach(&arr, 0, pprint, NULL);
	warnx("");
	
	warnx("insert 6 uranus");
	array_insert(&arr, 6, "uranus");
	array_foreach(&arr, 0, pprint, NULL);
	warnx("");

	// TODO: test grow/halfen functions

	return 0;
}
