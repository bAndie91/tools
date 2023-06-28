#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>

int GetIdleTime()
{
	time_t idle_time;
	static XScreenSaverInfo *mit_info;
	Display *display;
	int screen;
	
	mit_info = XScreenSaverAllocInfo();
	if((display=XOpenDisplay(NULL)) == NULL) { return(-1); }
	screen = DefaultScreen(display);
	XScreenSaverQueryInfo(display, RootWindow(display,screen), mit_info);
	idle_time = (mit_info->idle) / 1000;
	XFree(mit_info);
	XCloseDisplay(display); 
	return idle_time;
}

int main()
{
	int idle = GetIdleTime();
	if(idle == -1) return 1;
	printf("%d", idle);
	return 0;
}

