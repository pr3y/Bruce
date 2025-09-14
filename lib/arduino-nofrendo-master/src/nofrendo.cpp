/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.  To obtain a
** copy of the GNU Library General Public License, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** nofrendo.c
**
** Entry point of program
** Note: all architectures should call these functions
** $Id: nofrendo.c,v 1.3 2001/04/27 14:37:11 neil Exp $
*/

#include <stdio.h>
#include <stdlib.h>

#include "event.h"
#include "gui.h"
#include "log.h"
#include "nofconfig.h"
#include "nofrendo.h"
#include "noftypes.h"
#include "osd.h"
#include "vid_drv.h"

/* emulated system includes */
#include "nes/nes.h"

/* our global machine structure */
static struct {
    char *filename, *nextfilename;
    system_t type, nexttype;

    union {
        nes_t *nes;
    } machine;

    int refresh_rate;

    bool quit;
} console;

/* our happy little timer ISR */
volatile int nofrendo_ticks = 0;
static void timer_isr(void) { nofrendo_ticks++; }
static void timer_isr_end(void) {} /* code marker for djgpp */

static void shutdown_everything(void) {
    if (console.filename) {
        NOFRENDO_FREE(console.filename);
        console.filename = NULL;
    }
    if (console.nextfilename) {
        NOFRENDO_FREE(console.nextfilename);
        console.nextfilename = NULL;
    }

    config.close();
    osd_shutdown();
    gui_shutdown();
    vid_shutdown();
    nofrendo_log_shutdown();
}

/* End the current context */
void main_eject(void) {
    switch (console.type) {
        case system_nes:
            nes_poweroff();
            nes_destroy(&(console.machine.nes));
            break;

        default: break;
    }

    if (NULL != console.filename) {
        NOFRENDO_FREE(console.filename);
        console.filename = NULL;
    }
    console.type = system_unknown;
}

/* Act on the user's quit requests */
void main_quit(void) {
    console.quit = true;

    main_eject();

    /* if there's a pending filename / system, clear */
    if (NULL != console.nextfilename) {
        NOFRENDO_FREE(console.nextfilename);
        console.nextfilename = NULL;
    }
    console.nexttype = system_unknown;
}

/* brute force system autodetection */
static system_t detect_systemtype(const char *filename) {
    if (NULL == filename) return system_unknown;

    if (0 == nes_isourfile(filename)) return system_nes;

    /* can't figure out what this thing is */
    return system_unknown;
}

static int install_timer(int hertz) {
    return osd_installtimer(
        hertz,
        (void *)timer_isr,
        (int)timer_isr_end - (int)timer_isr,
        (void *)&nofrendo_ticks,
        sizeof(nofrendo_ticks)
    );
}

/* This assumes there is no current context */
static int internal_insert(const char *filename, system_t type) {
    /* autodetect system type? */
    if (system_autodetect == type) type = detect_systemtype(filename);

    console.filename = NOFRENDO_STRDUP(filename);
    console.type = type;

    /* set up the event system for this system type */
    event_set_system(type);

    switch (console.type) {
        case system_nes:
            gui_setrefresh(NES_REFRESH_RATE);

            console.machine.nes = nes_create();
            if (NULL == console.machine.nes) {
                nofrendo_log_printf("Failed to create NES instance.\n");
                return -1;
            }

            if (nes_insertcart(console.filename, console.machine.nes)) return -1;

            vid_setmode(NES_SCREEN_WIDTH, NES_SCREEN_HEIGHT);

            if (install_timer(NES_REFRESH_RATE)) return -1;

            nes_emulate();
            break;

        case system_unknown:
        default:
            nofrendo_log_printf("system type unknown, playing nofrendo NES intro.\n");
            if (NULL != console.filename) NOFRENDO_FREE(console.filename);

            /* oooh, recursion */
            return internal_insert(filename, system_nes);
    }

    return 0;
}

/* This tells main_loop to load this next image */
void main_insert(const char *filename, system_t type) {
    console.nextfilename = NOFRENDO_STRDUP(filename);
    console.nexttype = type;

    main_eject();
}

int nofrendo_main(int argc, char *argv[]) {
    /* initialize our system structure */
    console.filename = NULL;
    console.nextfilename = NULL;
    console.type = system_unknown;
    console.nexttype = system_unknown;
    console.refresh_rate = 0;
    console.quit = false;

    if (nofrendo_log_init()) return -1;

    event_init();

    return osd_main(argc, argv);
}

/* This is the final leg of main() */
int main_loop(const char *filename, system_t type) {
    vidinfo_t video;

    /* register shutdown, in case of assertions, etc. */
    atexit(shutdown_everything);

    if (config.open()) return -1;

    if (osd_init()) return -1;

    if (gui_init()) return -1;

    osd_getvideoinfo(&video);
    if (vid_init(video.default_width, video.default_height, video.driver)) return -1;

    console.nextfilename = NOFRENDO_STRDUP(filename);
    console.nexttype = type;

    while (false == console.quit) {
        if (internal_insert(console.nextfilename, console.nexttype)) return 1;
    }

    return 0;
}

/*
** $Log: nofrendo.c,v $
** Revision 1.3  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.2  2001/04/27 11:10:08  neil
** compile
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.48  2000/11/27 12:47:08  matt
** free them strings
**
** Revision 1.47  2000/11/25 20:26:05  matt
** removed fds "system"
**
** Revision 1.46  2000/11/25 01:51:53  matt
** bool stinks sometimes
**
** Revision 1.45  2000/11/20 13:22:12  matt
** standardized timer ISR, added nofrendo_ticks
**
** Revision 1.44  2000/11/05 22:53:13  matt
** only one video driver per system, please
**
** Revision 1.43  2000/11/01 14:15:35  matt
** multi-system event system, or whatever
**
** Revision 1.42  2000/10/28 15:16:24  matt
** removed nsf_init
**
** Revision 1.41  2000/10/27 12:58:44  matt
** gui_init can now fail
**
** Revision 1.40  2000/10/26 22:48:57  matt
** prelim NSF support
**
** Revision 1.39  2000/10/25 13:42:02  matt
** strdup - giddyap!
**
** Revision 1.38  2000/10/25 01:23:08  matt
** basic system autodetection
**
** Revision 1.37  2000/10/23 17:50:47  matt
** adding fds support
**
** Revision 1.36  2000/10/23 15:52:04  matt
** better system handling
**
** Revision 1.35  2000/10/21 19:25:59  matt
** many more cleanups
**
** Revision 1.34  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.33  2000/10/10 13:03:54  matt
** Mr. Clean makes a guest appearance
**
** Revision 1.32  2000/09/15 04:58:06  matt
** simplifying and optimizing APU core
**
** Revision 1.31  2000/09/10 23:19:14  matt
** i'm a sloppy coder
**
** Revision 1.30  2000/09/07 01:30:57  matt
** nes6502_init deprecated
**
** Revision 1.29  2000/08/16 03:17:49  matt
** bpb
**
** Revision 1.28  2000/08/16 02:58:19  matt
** changed video interface a wee bit
**
** Revision 1.27  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.26  2000/07/30 04:31:26  matt
** automagic loading of the nofrendo intro
**
** Revision 1.25  2000/07/27 01:16:36  matt
** sorted out the video problems
**
** Revision 1.24  2000/07/26 21:54:53  neil
** eject has to clear the nextfilename and nextsystem
**
** Revision 1.23  2000/07/26 21:36:13  neil
** Big honkin' change -- see the mailing list
**
** Revision 1.22  2000/07/25 02:21:36  matt
** safer xxx_destroy calls
**
** Revision 1.21  2000/07/23 16:46:47  matt
** fixed crash in win32 by reodering shutdown calls
**
** Revision 1.20  2000/07/23 15:18:23  matt
** removed unistd.h from includes
**
** Revision 1.19  2000/07/23 00:48:15  neil
** Win32 SDL
**
** Revision 1.18  2000/07/21 13:42:06  neil
** get_options removed, as it should be handled by osd_main
**
** Revision 1.17  2000/07/21 04:53:48  matt
** moved palette calls out of nofrendo.c and into ppu_create
**
** Revision 1.16  2000/07/21 02:40:43  neil
** more main fixes
**
** Revision 1.15  2000/07/21 02:09:07  neil
** new main structure?
**
** Revision 1.14  2000/07/20 17:05:12  neil
** Moved osd_init before setup_video
**
** Revision 1.13  2000/07/11 15:01:05  matt
** moved config.close() into registered atexit() routine
**
** Revision 1.12  2000/07/11 13:35:38  bsittler
** Changed the config API, implemented config file "nofrendo.cfg". The
** GGI drivers use the group [GGI]. Visual= and Mode= keys are understood.
**
** Revision 1.11  2000/07/11 04:32:21  matt
** less magic number nastiness for screen dimensions
**
** Revision 1.10  2000/07/10 03:04:15  matt
** removed scanlines, backbuffer from custom blit
**
** Revision 1.9  2000/07/07 04:39:54  matt
** removed garbage dpp shite
**
** Revision 1.8  2000/07/06 16:48:25  matt
** new video driver
**
** Revision 1.7  2000/07/05 17:26:16  neil
** Moved the externs in nofrendo.c to osd.h
**
** Revision 1.6  2000/06/26 04:55:44  matt
** cleaned up main()
**
** Revision 1.5  2000/06/20 20:41:21  matt
** moved <stdlib.h> include to top (duh)
**
** Revision 1.4  2000/06/09 15:12:25  matt
** initial revision
**
*/
