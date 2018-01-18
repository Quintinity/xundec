#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <X11/Xlib.h>

#define _MOTIF_WM_HINTS_SIZE 5
#define _NET_WM_STATE_REMOVE 0
#define _NET_WM_STATE_ADD 1
#define SOURCE_INDICATOR_NORMAL 1

struct hints_t {
	unsigned long flags;
	unsigned long functions;
	unsigned long decorations;
	long input_mode;
	unsigned long status;
};

Window get_active_window(Display *display) {
	Window root = RootWindow(display, DefaultScreen(display));
	unsigned char *data;
	Atom _NET_ACTIVE_WINDOW = XInternAtom(display, "_NET_ACTIVE_WINDOW", false);
	
	Atom value;
	int format;
	unsigned long extra, n;

	XGetWindowProperty(
			display, 
			root, 
			_NET_ACTIVE_WINDOW, 
			0, 
			~0, 
			false, 
			AnyPropertyType, 
			&value, &format, &n, &extra,
			&data);

	return * (Window *) data;
}

void maximize_window(Display *display, Window window) {
    Window root = RootWindow(display, DefaultScreen(display));
    
    Atom _NET_WM_STATE = XInternAtom(display, "_NET_WM_STATE", false);
    Atom _NET_WM_STATE_MAX_H = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
    Atom _NET_WM_STATE_MAX_V = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", false);

    XClientMessageEvent event;
    event.type = ClientMessage;
    event.display = display;
    event.window = window;
    event.format = 32;
	event.message_type = _NET_WM_STATE;
    event.data.l[0] = _NET_WM_STATE_ADD;
    event.data.l[1] = _NET_WM_STATE_MAX_H;
    event.data.l[2] = _NET_WM_STATE_MAX_V;
    event.data.l[3] = SOURCE_INDICATOR_NORMAL;
	
    XSendEvent(display, root, false, SubstructureRedirectMask | SubstructureNotifyMask, (XEvent *) &event);

}

void set_decorations(Display *display, Window window, int decorated) {
	Atom _MOTIF_WM_HINTS = XInternAtom(display, "_MOTIF_WM_HINTS", false);
	
	struct hints_t hints;
	hints.functions = 0;
	hints.flags = 2;
	hints.decorations = decorated;
	hints.input_mode = 0;
	hints.status = 0;

	XChangeProperty(
			display,
			window,
			_MOTIF_WM_HINTS,
			_MOTIF_WM_HINTS,
			32,
			PropModeReplace,
			(unsigned char *) &hints,
			_MOTIF_WM_HINTS_SIZE);
}

int main(int argc, char **argv) {
	Display *display = XOpenDisplay(NULL);
	if (display == NULL) {
		printf("Failed to open default display\n");
		return 1;
	}

	int decorations = 0;
	if (argc == 2) {
		decorations = atoi(argv[1]);
		if (decorations != 1 && decorations != 0) {
			printf("Invalid decoration setting: %d\n", decorations);
			return 1;
		}
	}

	Window active_window = get_active_window(display);
	set_decorations(display, active_window, decorations);
    maximize_window(display, active_window);

	XFlush(display);

	return 0;
}

