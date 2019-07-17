/*
Kero Platform is designed to be used for single-window applications and has a simple API design in mind. Currently only supports software rendering but it is planned to add OpenGL contexts in the future.

Kero Platform gives you window events, a keyboard state, a software rendering context and software framerate limiting.
*/

#ifndef KERO_PLATFORM_H

#ifdef __cplusplus
extern "C"{
#endif
    
    //------------------------------------------------------------
    
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
    
    struct{
        unsigned int w, h;
    } kp_window;
    typedef struct{
        unsigned int w, h;
        uint32_t* pixels;
    } kp_frame_buffer_t;
    static kp_frame_buffer_t kp_frame_buffer;
    float kp_delta = 0.f;
    bool kp_reset_keyboard_on_focus_out = true;
    bool kp_fullscreen = false;
    int kp_windowed_width = 0, kp_windowed_height = 0;
    int kp_windowed_x = 0, kp_windowed_y = 0;
    struct{
        int x, y;
        uint32_t buttons;
    } kp_mouse;
    static unsigned long target_frame_time = 0; // Linux: In nano seconds. Other platforms: In milliseconds.
    typedef enum {
        KPEVENT_KEY_PRESS, KPEVENT_KEY_RELEASE, KPEVENT_QUIT, KPEVENT_RESIZE, KPEVENT_FOCUS_OUT, KPEVENT_FOCUS_IN, KPEVENT_MOUSE_BUTTON_PRESS, KPEVENT_MOUSE_BUTTON_RELEASE, KPEVENT_NONE
    } kp_event_type_t;
    typedef struct {
        kp_event_type_t type;
        union {
            uint8_t key;
            struct {
                uint8_t button;
                uint16_t x, y;
            };
            struct {
                uint16_t width, height;
            };
        };
    } kp_event_t;
    
    //------------------------------------------------------------
    
    /*
     Usage
     
    Include this file. Currently this single header contains the entire Kero_Platform library. On Linux link against X11 (-lX11). On Windows/Mac link against SDL2 (-lSDL2)
    */
    
    
    
    void KPInit(const unsigned int width, const unsigned int height, const char* const title);
    /*
Initialize Kero Platform
width and height are the internal size of the frame, not the total size of the window. The actual frame may be smaller if it cannot fit on the screen.
    This sets a target framerate of 60fps and disables key repeat.
    */
    
    void KPFlip();
    /*
    Send frame buffer to screen.
    Sleep until time since last flip = target frame time (1/fps).
    Sets kp_delta variable to the number of seconds from the end of the last frame to the end of this one, including sleep time. DO NOT MANUALLY CHANGE THE VALUE OF kp_delta!
    */
    
    int KPEventsQueued();
    /*
    Returns number of events queued.
    Updates mouse position and button state.
    */
    
    kp_event_t* KPNextEvent();
    /*
    Returns pointer to next event in queue and removes that event from the queue.
    The event may have type KPEVENT_NONE in which case it should be ignored.
    */
    
    void KPFreeEvent(kp_event_t* e);
    /*
    Frees the event. Should be called for every event retrieved by KPNextEvent to avoid a memory leak.
    */
    
    /*
    Example event handling code:
    
    while(KPEventsQueued()) {
        kpEvent* e = KPNextEvent();
        switch(e->type) {
            case KPEVENT_KEY_PRESS:{
                switch(e->key) {
                    case KEY_ESCAPE:{
                        game_running = false;
                    }break;
                }
            }break;
            case KPEVENT_QUIT:{
                game_running = false;
            }break;
        }
        KPFreeEvent(e);
    }
    
    See enum kp_event_type_t for all event types.
    */
    
    void KPSetTargetFramerate(const unsigned int fps);
    /*
    Target number of frames per second. When calling KPFlip() the process will sleep until 1/fps seconds have elapsed since the last flip.
    Call this with fps of 0 to disable frame limit.
    */
    
    void KPUpdateMouse();
    /*
    Updates the mouse position and buttons.
    Mouse x/y are kp_mouse.x/y
    Buttons are kp_mouse.buttons & MOUSE_LEFT/MOUSE_RIGHT/MOUSE_OTHER
    */
    
    void KPSetWindowTitle(const char* const title);
    /*
     Very slow on Linux. Don't call every frame.
    */
    
    /*
    The keyboard is stored in a boolean array called kp_keyboard. Updated inside KPNextEvent() whenever a keyboard event is received.
Key repeats (holding key down) are ignored automatically.
    See Key defines for all the keys/
    
    if(kp_keyboard[KEY_ESCAPE]) {
        game_running = false;
    }
    */
    
    void KPShowCursor(const bool show);
    /*
    True = draw the cursor over the window
    False = hide cursor
    */
    
    double KPClock();
    /*
    Returns current wall-clock time in milliseconds.
    */
    
    void KPSetCursorPos(int x, int y, int* dx, int* dy);
    /*
    Sets mouse cursor to x,y which are offsets from the top-left corner of the window.
    dx and dy are set to the distance the cursor was moved.
    dx/dy can be NULL.
    */
    
    void KPFullscreen(bool full);
    /*
    True makes the window fullscreen.
    False returns to a windowed view.
    */
    
    void KPSleep(unsigned long nanoseconds);
    /*
    Pause thread for that many nanoseconds.
    */
    
    void KPOpenURL(const char* const url);
    /*
    Open a URL in the default web browser.
    */
    
    //------------------------------------------------------------
    
#define KEY_ENTER KEY_RETURN
    
#if defined(__linux__)
    
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>
#include <stdlib.h>
#include <string.h>
#include <X11/XKBlib.h>
#include <time.h>
    typedef struct timespec timespec;
    
    // Mouse buttons
#define MOUSE_LEFT (0b1)
#define MOUSE_RIGHT (0b10)
#define MOUSE_OTHER (0b100)
    
    // Key consts
#define KEY_UP ((uint8_t)XK_Up)
#define KEY_DOWN ((uint8_t)XK_Down)
#define KEY_LEFT ((uint8_t)XK_Left)
#define KEY_RIGHT ((uint8_t)XK_Right)
#define KEY_SPACE ((uint8_t)XK_space)
#define KEY_RETURN ((uint8_t)XK_Return)
#define KEY_ESCAPE ((uint8_t)XK_Escape)
#define KEY_LSHIFT ((uint8_t)XK_Shift_L)
#define KEY_RSHIFT ((uint8_t)XK_Shift_R)
#define KEY_LALT ((uint8_t)XK_Alt_L)
#define KEY_RALT ((uint8_t)XK_Alt_R)
#define KEY_Q ((uint8_t)XK_q)
#define KEY_W ((uint8_t)XK_w)
#define KEY_E ((uint8_t)XK_e)
#define KEY_R ((uint8_t)XK_r)
#define KEY_T ((uint8_t)XK_t)
#define KEY_Y ((uint8_t)XK_y)
#define KEY_U ((uint8_t)XK_u)
#define KEY_I ((uint8_t)XK_i)
#define KEY_O ((uint8_t)XK_o)
#define KEY_P ((uint8_t)XK_p)
#define KEY_A ((uint8_t)XK_a)
#define KEY_S ((uint8_t)XK_s)
#define KEY_D ((uint8_t)XK_d)
#define KEY_F ((uint8_t)XK_f)
#define KEY_G ((uint8_t)XK_g)
#define KEY_H ((uint8_t)XK_h)
#define KEY_J ((uint8_t)XK_j)
#define KEY_K ((uint8_t)XK_k)
#define KEY_L ((uint8_t)XK_l)
#define KEY_Z ((uint8_t)XK_z)
#define KEY_X ((uint8_t)XK_x)
#define KEY_C ((uint8_t)XK_c)
#define KEY_V ((uint8_t)XK_v)
#define KEY_B ((uint8_t)XK_b)
#define KEY_N ((uint8_t)XK_n)
#define KEY_M ((uint8_t)XK_m)
#define KEY_EQUAL ((uint8_t)XK_equal)
#define KEY_MINUS ((uint8_t)XK_minus)
#define KEY_0 ((uint8_t)XK_0)
#define KEY_1 ((uint8_t)XK_1)
#define KEY_2 ((uint8_t)XK_2)
#define KEY_3 ((uint8_t)XK_3)
#define KEY_4 ((uint8_t)XK_4)
#define KEY_5 ((uint8_t)XK_5)
#define KEY_6 ((uint8_t)XK_6)
#define KEY_7 ((uint8_t)XK_7)
#define KEY_8 ((uint8_t)XK_8)
#define KEY_9 ((uint8_t)XK_9)
    
#define _NET_WM_STATE_TOGGLE    2
    Atom _NET_WM_STATE_ATOM;
    
    static Display* display;
    static unsigned long root_window;
    static int screen;
    static XVisualInfo visual_info;
    static XSetWindowAttributes window_attributes;
    static Window xwindow;
    static XImage* ximage;
    static Atom WM_DELETE_WINDOW;
    static GC graphics_context;
    static timespec frame_start;
    static timespec frame_finish;
    bool kp_keyboard[256];
    
    // KPSetWindowTitle on Linux is very slow and inconsistent. Don't call every frame.
    void KPSetWindowTitle(const char* const title) {
        XStoreName(display, xwindow, title);
    }
    
    // fps = 0 to disable frame limiting
    void KPSetTargetFramerate(const unsigned int fps) {
        if(fps) {
            target_frame_time = 1000000000/fps;
        }
        else{
            target_frame_time = 0;
        }
    }
    
    void KPInit(const unsigned int width, const unsigned int height, const char* const title) {
        kp_windowed_width = width;
        kp_windowed_height = height;
        kp_window.w = width;
        kp_window.h = height;
        display = XOpenDisplay(0);
        root_window = XDefaultRootWindow(display);
        screen = XDefaultScreen(display);
        XMatchVisualInfo(display, screen, 24, TrueColor, &visual_info);
        window_attributes.background_pixel = 0;
        window_attributes.colormap = XCreateColormap(display, root_window, visual_info.visual, AllocNone);
        window_attributes.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | FocusChangeMask | ButtonPressMask | ButtonReleaseMask;
        xwindow = XCreateWindow(display, root_window, 0, 0, kp_window.w, kp_window.h, 0, visual_info.depth, 0, visual_info.visual, CWBackPixel | CWColormap | CWEventMask, &window_attributes);
        XMapWindow(display, xwindow);
        XFlush(display);
        WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, xwindow, &WM_DELETE_WINDOW, 1);
        XkbSetDetectableAutoRepeat(display, True, 0);
        KPSetWindowTitle(title);
        kp_frame_buffer.pixels = (uint32_t*)malloc(sizeof(uint32_t)*kp_window.w*kp_window.h);
        kp_frame_buffer.w = kp_window.w;
        kp_frame_buffer.h = kp_window.h;
        ximage = XCreateImage(display, visual_info.visual, visual_info.depth, ZPixmap, 0, (char*)kp_frame_buffer.pixels, kp_window.w, kp_window.h, 32, 0);
        graphics_context = DefaultGC(display, screen);
        KPSetTargetFramerate(60);
        _NET_WM_STATE_ATOM = XInternAtom(display, "_NET_WM_STATE", False);
        clock_gettime(CLOCK_REALTIME, &frame_start);
    }
    
#ifdef KERO_PLATFORM_GL
#include <GL/glx.h>
#include <GL/glu.h>
    static GLXContext gl_context;
    static GLint gl_attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    void KPGLInit(const unsigned int width, const unsigned int height,
                  const char* const title) {
        kp_windowed_width = width;
        kp_windowed_height = height;
        kp_window.w = width;
        kp_window.h = height;
        display = XOpenDisplay(0);
        root_window = XDefaultRootWindow(display);
        screen = XDefaultScreen(display);
        visual_info = *glXChooseVisual(display, 0, gl_attributes);
        window_attributes.colormap = XCreateColormap(display, root_window, visual_info.visual, AllocNone);
        window_attributes.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | FocusChangeMask | ButtonPressMask | ButtonReleaseMask;
        xwindow = XCreateWindow(display, root_window, 0, 0, kp_window.w, kp_window.h, 0, visual_info.depth, 0, visual_info.visual, CWColormap | CWEventMask, &window_attributes);
        XMapWindow(display, xwindow);
        XFlush(display);
        WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, xwindow, &WM_DELETE_WINDOW, 1);
        XkbSetDetectableAutoRepeat(display, True, 0);
        KPSetWindowTitle(title);
        gl_context = glXCreateContext(display, &visual_info, NULL, GL_TRUE);
        glXMakeCurrent(display, xwindow, gl_context);
        //KPGLSetTargetFramerate(60);
        _NET_WM_STATE_ATOM = XInternAtom(display, "_NET_WM_STATE", False);
        clock_gettime(CLOCK_REALTIME, &frame_start);
    }
    
    void KPGLFlip() {
        glXSwapBuffers(display, xwindow);
    }
#endif // KERO_PLATFORM_GL
    
    void KPSleep(unsigned long nanoseconds) {
        timespec sleep_time = { 0, nanoseconds };
        nanosleep(&sleep_time, 0);
    }
    
    void KPFlip() {
        XPutImage(display, xwindow, graphics_context, ximage, 0, 0, 0, 0, kp_window.w, kp_window.h);
        if(target_frame_time) {
            // Delay until we take up the full frame time
            clock_gettime(CLOCK_REALTIME, &frame_finish);
            long unsigned frame_time = (frame_finish.tv_sec - frame_start.tv_sec)*1000000000 + (frame_finish.tv_nsec - frame_start.tv_nsec);
            timespec sleep_time;
            sleep_time.tv_sec = 0;
            sleep_time.tv_nsec = target_frame_time - frame_time;
            nanosleep(&sleep_time, 0);
        }
        clock_gettime(CLOCK_REALTIME, &frame_finish);
        kp_delta = (frame_finish.tv_sec - frame_start.tv_sec) +(frame_finish.tv_nsec - frame_start.tv_nsec)/1000000000.f;
        frame_start = frame_finish;
    }
    
    void KPUpdateMouse() {
        Window window_returned;
        int display_x, display_y;
        unsigned int mask_return;
        if(XQueryPointer(display, xwindow, &window_returned,
                         &window_returned, &display_x, &display_y, &kp_mouse.x, &kp_mouse.y, 
                         &mask_return) == True) {
        }
    }
    
    int KPEventsQueued() {
        KPUpdateMouse();
        return XEventsQueued(display, QueuedAfterFlush);
    }
    
    kp_event_t* KPNextEvent() {
        kp_event_t* event = (kp_event_t*)malloc(sizeof(kp_event_t));
        event->type = KPEVENT_NONE;
        XEvent e;
        XNextEvent(display, &e);
        switch(e.type) {
            case KeyPress:{
                event->type = KPEVENT_KEY_PRESS;
                uint8_t symbol = (uint8_t)XLookupKeysym(&e.xkey, 0);
                kp_keyboard[symbol] = true;
                event->key = symbol;
            }break;
            case KeyRelease:{
                event->type = KPEVENT_KEY_RELEASE;
                uint8_t symbol = (uint8_t)XLookupKeysym(&e.xkey, 0);
                kp_keyboard[symbol] = false;
                event->key = symbol;
            }break;
            case ButtonPress:{
                event->type = KPEVENT_MOUSE_BUTTON_PRESS;
                event->x = e.xbutton.x;
                event->y = e.xbutton.y;
                event->button = MOUSE_OTHER;
                switch(e.xbutton.button) {
                    case Button1:{
                        event->button = MOUSE_LEFT;
                        kp_mouse.buttons |= MOUSE_LEFT;
                    }break;
                    case Button3:{
                        event->button = MOUSE_RIGHT;
                        kp_mouse.buttons |= MOUSE_RIGHT;
                    }break;
                }
            }break;
            case ButtonRelease:{
                event->type = KPEVENT_MOUSE_BUTTON_RELEASE;
                event->x = e.xbutton.x;
                event->y = e.xbutton.y;
                event->button = MOUSE_OTHER;
                switch(e.xbutton.button) {
                    case Button1:{
                        event->button = MOUSE_LEFT;
                        kp_mouse.buttons &= ~MOUSE_LEFT;
                    }break;
                    case Button3:{
                        event->button = MOUSE_RIGHT;
                        kp_mouse.buttons &= ~MOUSE_RIGHT;
                    }break;
                }
            }break;
            case ClientMessage:{
                XClientMessageEvent* ev = (XClientMessageEvent*)&e;
                if((Atom)ev->data.l[0] == WM_DELETE_WINDOW) {
                    event->type = KPEVENT_QUIT;
                }
            }break;
            case ConfigureNotify:{
                XConfigureEvent* ev = (XConfigureEvent*)&e;
                if(ev->width != kp_window.w || ev->height != kp_window.h) {
                    event->type = KPEVENT_RESIZE;
                    if(!kp_fullscreen) {
                        kp_windowed_width = ev->width;
                        kp_windowed_height = ev->height;
                    }
                    kp_window.w = ev->width;
                    kp_window.h = ev->height;
                    event->width = kp_window.w;
                    event->height = kp_window.h;
                    XDestroyImage(ximage);
                    kp_frame_buffer.pixels = (uint32_t*)malloc(sizeof(uint32_t)*kp_window.w*kp_window.h);
                    kp_frame_buffer.w = kp_window.w;
                    kp_frame_buffer.h = kp_window.h;
                    ximage = XCreateImage(display, visual_info.visual, visual_info.depth, ZPixmap, 0, (char*)kp_frame_buffer.pixels, kp_window.w, kp_window.h, 32, 0);
                }
            }break;
            case DestroyNotify:{
                event->type = KPEVENT_QUIT;
            }break;
            case FocusOut:{
                if(kp_reset_keyboard_on_focus_out) {
                    memset(kp_keyboard, false, sizeof(kp_keyboard));
                }
                kp_mouse.buttons = 0;
                event->type = KPEVENT_FOCUS_OUT;
            }break;
            case FocusIn:{
                event->type = KPEVENT_FOCUS_IN;
            }break;
        }
        return event;
    }
    
    void KPFreeEvent(kp_event_t* e) {
        free(e);
    }
    
    void KPShowCursor(const bool show) {
        if(show) {
            XUndefineCursor(display, xwindow);
        }
        else{
            XColor color;
            const char data[1] = {0};
            Pixmap pixmap = XCreateBitmapFromData(display, xwindow, data, 1, 1);
            Cursor cursor = XCreatePixmapCursor(display, pixmap, pixmap, &color, &color, 0, 0);
            XFreePixmap(display, pixmap);
            XDefineCursor(display, xwindow, cursor);
            XFreeCursor(display, cursor);
        }
    }
    
    double KPClock() {
        timespec clock_time;
        clock_gettime(CLOCK_REALTIME, &clock_time);
        return clock_time.tv_sec*1000.0 + clock_time.tv_nsec/1000000.0;
    }
    
    void KPSetCursorPos(int x, int y, int* dx, int* dy) {
        if(dx) *dx = x - kp_mouse.x;
        if(dy) *dy = y - kp_mouse.y;
        XWarpPointer(display, xwindow, xwindow, 0, 0, 0, 0, x, y);
        XFlush(display);
    }
    
    void KPFullscreen(bool full) {
        kp_fullscreen = full;
        XEvent xev = {0};
        xev.type = ClientMessage;
        xev.xclient.window = xwindow;
        xev.xclient.message_type = _NET_WM_STATE_ATOM;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = _NET_WM_STATE_TOGGLE;
        xev.xclient.data.l[1] = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);;
        xev.xclient.data.l[2] = 0;  /* no second property to toggle */
        xev.xclient.data.l[3] = 1;  /* source indication: application */
        xev.xclient.data.l[4] = 0;  /* unused */
        XSendEvent(display, root_window, 0, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    }
    
    void KPOpenURL(const char* const url) {
        char command[1024];
        sprintf(command, "xdg-open %s", url);
        system(command);
    }
    
    // End of __linux__
    
    //------------------------------------------------------------
    
#else
    
#include <SDL2/SDL.h>
    
    // Mouse buttons
#define MOUSE_LEFT SDL_BUTTON_LEFT
#define MOUSE_RIGHT SDL_BUTTON_RIGHT
#define MOUSE_OTHER SDL_BUTTON_MIDDLE
    
    // Keyboard keys
#define KEY_UP (SDL_SCANCODE_UP)
#define KEY_DOWN (SDL_SCANCODE_DOWN)
#define KEY_LEFT (SDL_SCANCODE_LEFT)
#define KEY_RIGHT (SDL_SCANCODE_RIGHT)
#define KEY_SPACE (SDL_SCANCODE_SPACE)
#define KEY_RETURN (SDL_SCANCODE_RETURN)
#define KEY_LALT (SDL_SCANCODE_LALT)
#define KEY_RALT (SDL_SCANCODE_LALT)
#define KEY_ESCAPE (SDL_SCANCODE_ESCAPE)
#define KEY_Q (SDL_SCANCODE_Q)
#define KEY_W (SDL_SCANCODE_W)
#define KEY_E (SDL_SCANCODE_E)
#define KEY_R (SDL_SCANCODE_R)
#define KEY_T (SDL_SCANCODE_T)
#define KEY_Y (SDL_SCANCODE_Y)
#define KEY_U (SDL_SCANCODE_U)
#define KEY_I (SDL_SCANCODE_I)
#define KEY_O (SDL_SCANCODE_O)
#define KEY_P (SDL_SCANCODE_P)
#define KEY_A (SDL_SCANCODE_A)
#define KEY_S (SDL_SCANCODE_S)
#define KEY_D (SDL_SCANCODE_D)
#define KEY_F (SDL_SCANCODE_F)
#define KEY_G (SDL_SCANCODE_G)
#define KEY_H (SDL_SCANCODE_H)
#define KEY_J (SDL_SCANCODE_J)
#define KEY_K (SDL_SCANCODE_K)
#define KEY_L (SDL_SCANCODE_L)
#define KEY_Z (SDL_SCANCODE_Z)
#define KEY_X (SDL_SCANCODE_X)
#define KEY_C (SDL_SCANCODE_C)
#define KEY_V (SDL_SCANCODE_V)
#define KEY_B (SDL_SCANCODE_B)
#define KEY_N (SDL_SCANCODE_N)
#define KEY_M (SDL_SCANCODE_M)
#define KEY_EQUAL (SDL_SCANCODE_EQUALS)
#define KEY_MINUS (SDL_SCANCODE_MINUS)
#define KEY_LSHIFT (SDL_SCANCODE_LSHIFT)
#define KEY_RSHIFT (SDL_SCANCODE_RSHIFT)
#define KEY_0 (SDL_SCANCODE_0)
#define KEY_1 (SDL_SCANCODE_1)
#define KEY_2 (SDL_SCANCODE_2)
#define KEY_3 (SDL_SCANCODE_3)
#define KEY_4 (SDL_SCANCODE_4)
#define KEY_5 (SDL_SCANCODE_5)
#define KEY_6 (SDL_SCANCODE_6)
#define KEY_7 (SDL_SCANCODE_7)
#define KEY_8 (SDL_SCANCODE_8)
#define KEY_9 (SDL_SCANCODE_9)
    
    static SDL_Window* sdlwindow;
    static SDL_Surface* canvas;
    uint32_t frame_start;
    uint32_t frame_finish;
    
    const uint8_t* kp_keyboard;
    
    void KPSetWindowTitle(const char* const title) {
        SDL_SetWindowTitle(sdlwindow, title);
    }
    
    // fps = 0 to disable frame limiting
    void KPSetTargetFramerate(const unsigned int fps) {
        if(fps) {
            target_frame_time = 1000/fps;
        }
        else{
            target_frame_time = 0;
        }
    }
    
    void KPInit(const unsigned int width, const unsigned int height, const char* const title) {
        SDL_Init(SDL_INIT_VIDEO);
        kp_windowed_width = width;
        kp_windowed_height = height;
        kp_window.w = width;
        kp_window.h = height;
        sdlwindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        canvas = SDL_GetWindowSurface(sdlwindow);
        kp_frame_buffer.w = kp_window.w;
        kp_frame_buffer.h = kp_window.h;
        kp_frame_buffer.pixels = (uint32_t*)canvas->pixels;
        kp_keyboard = SDL_GetKeyboardState(NULL);
        KPSetTargetFramerate(60);
        frame_start = SDL_GetTicks();
    }
    
    void KPSleep(unsigned long nanoseconds) {
        SDL_Delay(nanoseconds / 1000000);
    }
    
    void KPFlip() {
        SDL_UpdateWindowSurface(sdlwindow);
        if(target_frame_time) {
            // Delay until we take up the full frame time
            frame_finish = SDL_GetTicks();
            int32_t sleep_time = target_frame_time - (frame_finish - frame_start);
            sleep_time = sleep_time > 0 ? sleep_time : 0;
            SDL_Delay(sleep_time);
        }
        frame_finish = SDL_GetTicks();
        kp_delta = (frame_finish - frame_start) / 1000.f;
        frame_start = frame_finish;
    }
    
    void KPUpdateMouse() {
        kp_mouse.buttons = SDL_GetMouseState(&kp_mouse.x, &kp_mouse.y);
    }
    
    int KPEventsQueued() {
        KPUpdateMouse();
        return SDL_PollEvent(0);
    }
    
    kp_event_t* KPNextEvent() {
        kp_event_t* event = (kp_event_t*)malloc(sizeof(kp_event_t));
        event->type = KPEVENT_NONE;
        SDL_Event e;
        if(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_KEYDOWN:{
                    event->type = KPEVENT_KEY_PRESS;
                    int unsigned symbol = SDL_GetScancodeFromKey(e.key.keysym.sym);
                    event->key = symbol;
                }break;
                case SDL_KEYUP:{
                    if(!e.key.repeat) {
                        event->type = KPEVENT_KEY_RELEASE;
                        int unsigned symbol = SDL_GetScancodeFromKey(e.key.keysym.sym);
                        event->key = symbol;
                    }
                }break;
                case SDL_MOUSEBUTTONDOWN:{
                    event->type = KPEVENT_MOUSE_BUTTON_PRESS;
                    event->x = e.button.x;
                    event->y = e.button.y;
                    event->button = MOUSE_OTHER;
                    switch(e.button.button) {
                        case SDL_BUTTON_LEFT:{
                            event->button = MOUSE_LEFT;
                            kp_mouse.buttons |= MOUSE_LEFT;
                        }break;
                        case SDL_BUTTON_RIGHT:{
                            event->button = MOUSE_RIGHT;
                            kp_mouse.buttons |= MOUSE_RIGHT;
                        }break;
                    }
                }break;
                case SDL_MOUSEBUTTONUP:{
                    event->type = KPEVENT_MOUSE_BUTTON_RELEASE;
                    event->x = e.button.x;
                    event->y = e.button.y;
                    event->button = MOUSE_OTHER;
                    switch(e.button.button) {
                        case SDL_BUTTON_LEFT:{
                            event->button = MOUSE_LEFT;
                            kp_mouse.buttons &= ~MOUSE_LEFT;
                        }break;
                        case SDL_BUTTON_RIGHT:{
                            event->button = MOUSE_RIGHT;
                            kp_mouse.buttons &= ~MOUSE_RIGHT;
                        }break;
                    }
                }break;
                case SDL_QUIT:{
                    event->type = KPEVENT_QUIT;
                }break;
                case SDL_WINDOWEVENT:{
                    switch(e.window.event) {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:{
                            event->type = KPEVENT_RESIZE;
                            if(!kp_fullscreen) {
                                kp_windowed_width = e.window.data1;
                                kp_windowed_height = e.window.data2;
                            }
                            kp_window.w = e.window.data1;
                            kp_window.h = e.window.data2;
                            event->width = kp_window.w;
                            event->height = kp_window.h;
                            SDL_FreeSurface(canvas);
                            canvas = SDL_GetWindowSurface(sdlwindow);
                            kp_frame_buffer.pixels = canvas->pixels;
                            kp_frame_buffer.w = kp_window.w;
                            kp_frame_buffer.h = kp_window.h;
                        }break;
                        case SDL_WINDOWEVENT_FOCUS_LOST:{
                            event->type = KPEVENT_FOCUS_OUT;
                        }break;
                        case SDL_WINDOWEVENT_FOCUS_GAINED:{
                            event->type = KPEVENT_FOCUS_IN;
                        }break;
                    }
                }break;
            }
        }
        
        return event;
    }
    
    void KPFreeEvent(kp_event_t* e) {
        free(e);
    }
    
    void KPShowCursor(const bool show) {
        SDL_ShowCursor(show);
    }
    
    double KPClock() {
        return SDL_GetTicks();
    }
    
    void KPSetCursorPos(int x, int y, int* dx, int* dy) {
        if(dx) *dx = x - kp_mouse.x;
        if(dy) *dy = y - kp_mouse.y;
        SDL_WarpMouseInWindow(sdlwindow, x, y);
    }
    
    void KPFullscreen(bool full) {
        kp_fullscreen = full;
        if(full) {
            SDL_GetWindowSize(sdlwindow, &kp_windowed_width, &kp_windowed_height);
            SDL_GetWindowPosition(sdlwindow, &kp_windowed_x, &kp_windowed_y);
            SDL_SetWindowFullscreen(sdlwindow, SDL_WINDOW_FULLSCREEN);
        }
        else {
            SDL_SetWindowFullscreen(sdlwindow, 0);
            SDL_SetWindowPosition(sdlwindow, kp_windowed_x, kp_windowed_y);
        }
    }
    
#if _WIN32
    void KPOpenURL(const char* const url) {
        char command[1024];
        sprintf(command, "start %s", url);
        system(command);
    }
#else
    void KPOpenURL(const char* const url) {
        char command[1024];
        sprintf(command, "open %s", url);
        system(command);
    }
#endif
    
#endif // Not Linux
    
    //------------------------------------------------------------
    
#ifdef __cplusplus
}
#endif

#define KERO_PLATFORM_H
#endif
