
#include <err.h>

#include "libarray.c"

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
	array_foreach(&arr, pprint, NULL);
	warnx("");
	
	warnx("append one");
	array_append(&arr, "one");
	array_foreach(&arr, pprint, NULL);
	warnx("");
	
	warnx("append two");
	array_append(&arr, "two");
	array_foreach(&arr, pprint, NULL);
	warnx("");
	
	warnx("insert 0 zero");
	array_insert(&arr, 0, "zero");
	array_foreach(&arr, pprint, NULL);
	warnx("");
	
	warnx("insert 2 half");
	array_insert(&arr, 2, "half");
	array_foreach(&arr, pprint, NULL);
	warnx("");
	
	warnx("insert 1000 OOB");
	array_insert(&arr, 1000, "OOB");
	array_foreach(&arr, pprint, NULL);
	warnx("");
	
	warnx("insert 4 three");
	array_insert(&arr, 4, "three");
	array_foreach(&arr, pprint, NULL);
	warnx("");
	
	warnx("insert 6 four");
	array_insert(&arr, 6, "four");
	array_foreach(&arr, pprint, NULL);
	warnx("");
	
	return 0;
}
