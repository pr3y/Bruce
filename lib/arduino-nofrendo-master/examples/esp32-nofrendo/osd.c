/* start rewrite from: https://github.com/espressif/esp32-nesemu.git */
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <esp_heap_caps.h>

#include <noftypes.h>

#include <event.h>
#include <gui.h>
#include <log.h>
#include <nes/nes.h>
#include <nes/nes_pal.h>
#include <nes/nesinput.h>
#include <nofconfig.h>
#include <osd.h>

#include "hw_config.h"

TimerHandle_t timer;

/* memory allocation */
extern void *mem_alloc(int size, bool prefer_fast_memory)
{
	if (prefer_fast_memory)
	{
		return heap_caps_malloc(size, MALLOC_CAP_8BIT);
	}
	else
	{
		return heap_caps_malloc_prefer(size, MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT);
	}
}

/* sound */
extern int osd_init_sound();
extern void osd_stopsound();
extern void do_audio_frame();
extern void osd_getsoundinfo(sndinfo_t *info);

/* display */
extern void display_init();
extern void display_write_frame(const uint8_t *data[]);
extern void display_clear();

//This runs on core 0.
QueueHandle_t vidQueue;
static void displayTask(void *arg)
{
	bitmap_t *bmp = NULL;
	while (1)
	{
		// xQueueReceive(vidQueue, &bmp, portMAX_DELAY); //skip one frame to drop to 30
		xQueueReceive(vidQueue, &bmp, portMAX_DELAY);
		display_write_frame((const uint8_t **)bmp->line);
	}
}

/* get info */
static char fb[1]; //dummy
bitmap_t *myBitmap;

/* initialise video */
static int init(int width, int height)
{
	return 0;
}

static void shutdown(void)
{
}

/* set a video mode */
static int set_mode(int width, int height)
{
	return 0;
}

/* copy nes palette over to hardware */
uint16 myPalette[256];
static void set_palette(rgb_t *pal)
{
	uint16 c;

	int i;

	for (i = 0; i < 256; i++)
	{
		c = (pal[i].b >> 3) + ((pal[i].g >> 2) << 5) + ((pal[i].r >> 3) << 11);
		//myPalette[i]=(c>>8)|((c&0xff)<<8);
		myPalette[i] = c;
	}
}

/* clear all frames to a particular color */
static void clear(uint8 color)
{
	// SDL_FillRect(mySurface, 0, color);
	display_clear();
}

/* acquire the directbuffer for writing */
static bitmap_t *lock_write(void)
{
	// SDL_LockSurface(mySurface);
	myBitmap = bmp_createhw((uint8 *)fb, NES_SCREEN_WIDTH, NES_SCREEN_HEIGHT, NES_SCREEN_WIDTH * 2);
	return myBitmap;
}

/* release the resource */
static void free_write(int num_dirties, rect_t *dirty_rects)
{
	bmp_destroy(&myBitmap);
}

static void custom_blit(bitmap_t *bmp, int num_dirties, rect_t *dirty_rects)
{
	xQueueSend(vidQueue, &bmp, 0);
	do_audio_frame();
}

viddriver_t sdlDriver =
	{
		"Simple DirectMedia Layer", /* name */
		init,						/* init */
		shutdown,					/* shutdown */
		set_mode,					/* set_mode */
		set_palette,				/* set_palette */
		clear,						/* clear */
		lock_write,					/* lock_write */
		free_write,					/* free_write */
		custom_blit,				/* custom_blit */
		false						/* invalidate flag */
};

void osd_getvideoinfo(vidinfo_t *info)
{
	info->default_width = NES_SCREEN_WIDTH;
	info->default_height = NES_SCREEN_HEIGHT;
	info->driver = &sdlDriver;
}

/* input */
extern void controller_init();
extern uint32_t controller_read_input();

static void osd_initinput()
{
	controller_init();
}

static void osd_freeinput(void)
{
}

void osd_getinput(void)
{
	const int ev[32] = {
		event_joypad1_up, event_joypad1_down, event_joypad1_left, event_joypad1_right,
		event_joypad1_select, event_joypad1_start, event_joypad1_a, event_joypad1_b,
		event_state_save, event_state_load, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0};
	static int oldb = 0xffff;
	uint32_t b = controller_read_input();
	uint32_t chg = b ^ oldb;
	int x;
	oldb = b;
	event_t evh;
	// nofrendo_log_printf("Input: %x\n", b);
	for (x = 0; x < 16; x++)
	{
		if (chg & 1)
		{
			evh = event_get(ev[x]);
			if (evh)
				evh((b & 1) ? INP_STATE_BREAK : INP_STATE_MAKE);
		}
		chg >>= 1;
		b >>= 1;
	}
}

void osd_getmouse(int *x, int *y, int *button)
{
}

/* init / shutdown */
static int logprint(const char *string)
{
	return printf("%s", string);
}

int osd_init()
{
	nofrendo_log_chain_logfunc(logprint);

	if (osd_init_sound())
		return -1;

	display_init();
	vidQueue = xQueueCreate(1, sizeof(bitmap_t *));
	// xTaskCreatePinnedToCore(&displayTask, "displayTask", 2048, NULL, 5, NULL, 1);
	xTaskCreatePinnedToCore(&displayTask, "displayTask", 2048, NULL, 0, NULL, 0);
	osd_initinput();
	return 0;
}

void osd_shutdown()
{
	osd_stopsound();
	osd_freeinput();
}

char configfilename[] = "na";
int osd_main(int argc, char *argv[])
{
	config.filename = configfilename;

	return main_loop(argv[0], system_autodetect);
}

//Seemingly, this will be called only once. Should call func with a freq of frequency,
int osd_installtimer(int frequency, void *func, int funcsize, void *counter, int countersize)
{
	nofrendo_log_printf("Timer install, configTICK_RATE_HZ=%d, freq=%d\n", configTICK_RATE_HZ, frequency);
	timer = xTimerCreate("nes", configTICK_RATE_HZ / frequency, pdTRUE, NULL, func);
	xTimerStart(timer, 0);
	return 0;
}

/* filename manipulation */
void osd_fullname(char *fullname, const char *shortname)
{
	strncpy(fullname, shortname, PATH_MAX);
}

/* This gives filenames for storage of saves */
char *osd_newextension(char *string, char *ext)
{
	// dirty: assume both extensions is 3 characters
	size_t l = strlen(string);
	string[l - 3] = ext[1];
	string[l - 2] = ext[2];
	string[l - 1] = ext[3];

	return string;
}

/* This gives filenames for storage of PCX snapshots */
int osd_makesnapname(char *filename, int len)
{
	return -1;
}
