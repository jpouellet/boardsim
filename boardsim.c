#include <err.h>
#include <vpi_user.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//////// BOARD I/O DECLARATIONS ////////

#define lin_scale(start, stop, count, idx) (((stop)-(start))*(idx)/((count)-1)+(start))

#define UP   (1)
#define DOWN (0)
struct board_button {
	SDL_Keycode sym;
	int vecidx;
	int x;
	int y;
	int state;
};

#define ON   (1)
#define OFF  (0)
struct board_switch {
	int vecidx;
	int x;
	int y;
	int state;
};

struct board_led {
	int vecidx;
	int x;
	int y;
	int state;
};

struct board_button buttons[] = {
#define btn_x(idx) lin_scale(624, 791, 4, idx)
	{SDLK_q, 3, btn_x(0), 606, UP},
	{SDLK_w, 2, btn_x(1), 606, UP},
	{SDLK_e, 1, btn_x(2), 606, UP},
	{SDLK_r, 0, btn_x(3), 606, UP},
#undef btn_x
};
#define N_BTN (sizeof(buttons) / sizeof(buttons[0]))

struct board_switch switches[] = {
#define sw_x(idx) lin_scale(84, 567, 18, idx)
	{17, sw_x( 0), 604, OFF},
	{16, sw_x( 1), 604, OFF},
	{15, sw_x( 2), 604, OFF},
	{14, sw_x( 3), 604, OFF},
	{13, sw_x( 4), 604, OFF},
	{12, sw_x( 5), 604, OFF},
	{11, sw_x( 6), 604, OFF},
	{10, sw_x( 7), 604, OFF},
	{ 9, sw_x( 8), 604, OFF},
	{ 8, sw_x( 9), 604, OFF},
	{ 7, sw_x(10), 604, OFF},
	{ 6, sw_x(11), 604, OFF},
	{ 5, sw_x(12), 604, OFF},
	{ 4, sw_x(13), 604, OFF},
	{ 3, sw_x(14), 604, OFF},
	{ 2, sw_x(15), 604, OFF},
	{ 1, sw_x(16), 604, OFF},
	{ 0, sw_x(17), 604, OFF},
#undef sw_x
};
#define N_SW (sizeof(switches) / sizeof(switches[0]))

struct board_led red_leds[] = {
#define led_x(idx) lin_scale(88, 567, 18, idx)
	{17, led_x( 0), 561, OFF},
	{16, led_x( 1), 561, OFF},
	{15, led_x( 2), 561, OFF},
	{14, led_x( 3), 561, OFF},
	{13, led_x( 4), 561, OFF},
	{12, led_x( 5), 561, OFF},
	{11, led_x( 6), 561, OFF},
	{10, led_x( 7), 561, OFF},
	{ 9, led_x( 8), 561, OFF},
	{ 8, led_x( 9), 561, OFF},
	{ 7, led_x(10), 561, OFF},
	{ 6, led_x(11), 561, OFF},
	{ 5, led_x(12), 561, OFF},
	{ 4, led_x(13), 561, OFF},
	{ 3, led_x(14), 561, OFF},
	{ 2, led_x(15), 561, OFF},
	{ 1, led_x(16), 561, OFF},
	{ 0, led_x(17), 561, OFF},
#undef led_x
};
#define N_LED (sizeof(red_leds) / sizeof(red_leds[0]))

//////// LEDS ////////

PLI_INT32
DE2_leds_compiletf(PLI_BYTE8 *user_data)
{
	vpiHandle systf, iter, arg;

	systf = vpi_handle(vpiSysTfCall, NULL);
	iter = vpi_iterate(vpiArgument, systf);
	if (iter == NULL) // must have args
		goto fail;

	arg = vpi_scan(iter);
	if (arg == NULL) // should not be reachable... but just in case
		goto fail;

	if (vpi_get(vpiType, arg) != vpiNet) // arg must be vec[]
		goto fail;

	if (vpi_get(vpiSize, arg) != N_LED) // vec must be N_LED bits wide
		goto fail;

	if (vpi_scan(iter) != NULL) // must not have >1 arg
		goto fail;

	return 0;

	// We are leaking an iter in some cases, but vpi_free_object(iter) double-frees if used after vpi_scan returns NULL and frees non-alloc'd if used before scanning, so it's significantly more readable (and non-problematic since in _complie) to just leak. See handbook section 4.5.3 "When to use vpi_free_object() on iterator handles" (2nd edition page 108)
fail:
	vpi_printf("ERROR: $DE2_leds() requires exactly one %lu-bit wide vector argument\n", N_LED);
	vpi_control(vpiFinish, 1);
	return 0;
}

PLI_INT32
DE2_leds_calltf(PLI_BYTE8 *user_data)
{
	vpiHandle systf, iter, vec;
	s_vpi_value val;
	PLI_INT32 aval, bval;

	systf = vpi_handle(vpiSysTfCall, NULL);
	iter = vpi_iterate(vpiArgument, systf);
	vec = vpi_scan(iter);
	vpi_free_object(iter);

	// For vector represenation, see "The Verilog PLI Handbook" section 5.2.7 "Reading Verilog 4-state logic vectors as encoded aval/bval pairs" (1st edition page 150, 2nd edition page 163)

	val.format = vpiVectorVal;
	vpi_get_value(vec, &val);
	aval = val.value.vector[0].aval;
	bval = val.value.vector[0].bval;

	size_t i;
	for (i = 0; i < sizeof(red_leds) / sizeof(red_leds[0]); i++) {
		PLI_INT32 mask = (1 << (N_LED - 1)) >> i;
		red_leds[i].state = ((aval & mask) && !(bval & mask)) ? ON : OFF;
	}

	return 0;
}

void
DE2_leds_register(void)
{
	s_vpi_systf_data tf_data;

	tf_data.type = vpiSysTask;
	tf_data.sysfunctype = 0;
	tf_data.tfname = "$DE2_leds";
	tf_data.calltf = DE2_leds_calltf;
	tf_data.compiletf = DE2_leds_compiletf;
	tf_data.sizetf = NULL;
	tf_data.user_data = NULL;

	vpi_register_systf(&tf_data);
}

//////// SWITCHES ////////

PLI_INT32
DE2_switches_calltf(PLI_BYTE8 *user_data)
{
	vpiHandle systf;
	s_vpi_value val;
	s_vpi_vecval vec0;

	systf = vpi_handle(vpiSysTfCall, NULL);

	val.format = vpiVectorVal;
	val.value.vector = &vec0;
	vec0.aval = 0;
	vec0.bval = 0;

	size_t i;
	for (i = 0; i < N_SW; i++)
		if (switches[i].state)
			vec0.aval |= (1 << (N_SW - 1)) >> i;

	vpi_put_value(systf, &val, NULL, vpiNoDelay);

	return 0;
}

PLI_INT32
DE2_switches_sizetf(PLI_BYTE8 *user_data)
{
	return (N_SW);
}

void
DE2_switches_register(void)
{
	s_vpi_systf_data tf_data;

	tf_data.type = vpiSysFunc;
	tf_data.sysfunctype = vpiSizedFunc;
	tf_data.tfname = "$DE2_switches";
	tf_data.calltf = DE2_switches_calltf;
	tf_data.compiletf = NULL;
	tf_data.sizetf = DE2_switches_sizetf;
	tf_data.user_data = NULL;

	vpi_register_systf(&tf_data);
}

//////// BUTTONS ////////

PLI_INT32
DE2_buttons_calltf(PLI_BYTE8 *user_data)
{
	vpiHandle systf;
	s_vpi_value val;
	s_vpi_vecval vec0;

	systf = vpi_handle(vpiSysTfCall, NULL);

	val.format = vpiVectorVal;
	val.value.vector = &vec0;
	vec0.aval = 0;
	vec0.bval = 0;

	size_t i;
	for (i = 0; i < N_BTN; i++)
		if (buttons[i].state)
			vec0.aval |= (1 << (N_BTN - 1)) >> i;

	vpi_put_value(systf, &val, NULL, vpiNoDelay);

	return 0;
}

PLI_INT32
DE2_buttons_sizetf(PLI_BYTE8 *user_data)
{
	return (N_BTN);
}

void
DE2_buttons_register(void)
{
	s_vpi_systf_data tf_data;

	tf_data.type = vpiSysFunc;
	tf_data.sysfunctype = vpiSizedFunc;
	tf_data.tfname = "$DE2_buttons";
	tf_data.calltf = DE2_buttons_calltf;
	tf_data.compiletf = NULL;
	tf_data.sizetf = DE2_buttons_sizetf;
	tf_data.user_data = NULL;

	vpi_register_systf(&tf_data);
}

//////// GUI ////////

SDL_Window *window;
SDL_Surface *screen;
SDL_Surface *board_texture;
SDL_Surface *led_texture;
SDL_Surface *one_texture;
SDL_Surface *zero_texture;

static void
handle_button(struct board_button *b, int updown)
{
	b->state = updown;
}

static void
handle_switch(struct board_switch *sw)
{
	sw->state = !sw->state;
}

static struct board_switch *
find_switch(int x, int y)
{
	size_t i;
	for (i = 0; i < sizeof(switches) / sizeof(switches[0]); i++)
		if (abs(switches[i].x - x) < 10 &&
		    abs(switches[i].y - y) < 20)
			return &switches[i];
	return NULL;
}

static struct board_button *
find_button(SDL_Keycode k)
{
	size_t i;
	for (i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
		if (buttons[i].sym == k)
			return &buttons[i];
	return NULL;
}

static void
handle_input(void)
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_MOUSEBUTTONDOWN: {
			struct board_switch *sw = find_switch(e.button.x, e.button.y);
			if (sw != NULL)
				handle_switch(sw);
			break;
		}

		case SDL_KEYDOWN:
		case SDL_KEYUP: {
			struct board_button *b = find_button(e.key.keysym.sym);
			if (b != NULL)
				handle_button(b, e.type == SDL_KEYDOWN ? DOWN : UP);
			break;
		}

		case SDL_QUIT:
			exit(0);
			break;
		}
	}
}

static SDL_Surface *
xload_img(const char *path)
{
	SDL_Surface *s;
	s = IMG_Load(path);
	if (s == NULL)
		err(1, "IMG_Load: %s", IMG_GetError());
	return s;
}

void
gui_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		err(1, "SDL_Init: %s", SDL_GetError());

	int flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) != flags)
		err(1, "IMG_Init: %s", IMG_GetError());

	board_texture = xload_img("DE2.png");
	led_texture = xload_img("led.png");
	zero_texture = xload_img("0.png");
	one_texture = xload_img("1.png");

	window = SDL_CreateWindow("board", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, board_texture->w, board_texture->h, SDL_WINDOW_SHOWN);
	if (window == NULL)
		err(1, "SDL_CreateWindow: %s", SDL_GetError());

	screen = SDL_GetWindowSurface(window);
	if (screen == NULL)
		err(1, "SDL_GetWindowSurface: %s", SDL_GetError());
}

static void
blit_centered(SDL_Surface *texture, int x, int y)
{
	SDL_Rect dr;
	dr.w = texture->w;
	dr.h = texture->h;

	dr.x = x - dr.w/2;
	dr.y = y - dr.h/2;

	if (SDL_BlitSurface(texture, NULL, screen, &dr) < 0)
		err(1, "SDL_BlitSurface(texture): %s", SDL_GetError());
}

static void
draw_leds(void)
{
	size_t i;
	for (i = 0; i < sizeof(red_leds) / sizeof(red_leds[0]); i++)
		if (red_leds[i].state) // XXX TODO: variable opacity?
			blit_centered(led_texture, red_leds[i].x, red_leds[i].y);
}

static void
draw_input_states(void)
{
	size_t i;

	for (i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
		blit_centered(buttons[i].state ? one_texture : zero_texture, buttons[i].x, buttons[i].y);

	for (i = 0; i < sizeof(switches) / sizeof(switches[0]); i++)
		blit_centered(switches[i].state ? one_texture : zero_texture, switches[i].x, switches[i].y);
}

void
render(void)
{
	if (SDL_BlitSurface(board_texture, NULL, screen, NULL) < 0)
		err(1, "SDL_BlitSurface(board_texture): %s", SDL_GetError());

	draw_leds();
	draw_input_states();

	if (SDL_UpdateWindowSurface(window) < 0)
		err(1, "SDL_UpdateWindowSurface: %s", SDL_GetError());
}

//////// SDL<->VPI GUI SHIMS ////////

PLI_INT32
DE2_render(PLI_BYTE8 *user_data)
{
	render();
	return 0;
}

void
DE2_render_register(void)
{
	s_vpi_systf_data tf_data;

	tf_data.type = vpiSysTask;
	tf_data.sysfunctype = 0;
	tf_data.tfname = "$DE2_render";
	tf_data.calltf = DE2_render;
	tf_data.compiletf = NULL;
	tf_data.sizetf = NULL;
	tf_data.user_data = NULL;

	vpi_register_systf(&tf_data);
}

PLI_INT32
DE2_handle_input(PLI_BYTE8 *user_data)
{
	handle_input();
	return 0;
}

void
DE2_handle_input_register(void)
{
	s_vpi_systf_data tf_data;

	tf_data.type = vpiSysTask;
	tf_data.sysfunctype = 0;
	tf_data.tfname = "$DE2_handle_input";
	tf_data.calltf = DE2_handle_input;
	tf_data.compiletf = NULL;
	tf_data.sizetf = NULL;
	tf_data.user_data = NULL;

	vpi_register_systf(&tf_data);
}

//////// VPI REGISTRATION ////////

void (*vlog_startup_routines[])() = {
	gui_init,
	DE2_leds_register,
	DE2_switches_register,
	DE2_buttons_register,
	DE2_render_register,
	DE2_handle_input_register,
	0
};
