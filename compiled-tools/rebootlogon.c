
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	execvp("reboot", (char*[]){ "reboot", NULL });
	exit(127);
}
