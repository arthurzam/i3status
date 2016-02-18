// vim:ts=4:sw=4:expandtab
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <yajl/yajl_gen.h>
#include <yajl/yajl_version.h>

#include <X11/Xlib.h>

#include "i3status.h"

void print_language(yajl_gen json_gen, char *buffer) {

    char *outwalk = buffer;
    static Display *dpy = NULL;
    XKeyboardState values;
    const char* lan_str = NULL;

    if(dpy == NULL) {
        dpy = XOpenDisplay(NULL);
        if (dpy == NULL) {
            fprintf(stderr, "language:  unable to open display\n");
            *buffer = '\0';
            OUTPUT_FULL_TEXT(buffer);
        }
    }

    XGetKeyboardControl(dpy, &values);
    int lan = values.led_mask;

    if((lan & 2) == 0) {
        START_COLOR("color_degraded");
    }

    if(lan >= 1000) {
        lan_str = ((lan & 1) == 0) ? "Hebrew" : "HEBREW";
    } else {
        lan_str = ((lan & 1) == 0) ? "English" : "ENGLISH";
    }

    strcpy(buffer, lan_str);
    outwalk += strlen(lan_str);

    if((lan & 2) == 0) {
        END_COLOR;
    }
    OUTPUT_FULL_TEXT(buffer);
}
