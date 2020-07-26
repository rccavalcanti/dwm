/* Modifications by Rafael Cavalcanti - rafaelc.org
 * Based on dwm original configuration */

/* See LICENSE file for copyright and license details. */

/* For multimedia keys */
#include <X11/XF86keysym.h>

/* patches */
static const unsigned int gappx     = 6;        /* gap pixel between windows */

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Fira Sans:size=10" };
static const char dmenufont[]       = "Fira Sans:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Gnome-calculator", NULL, NULL,     0,            1,           -1 },
	{ "Code",     NULL,       NULL,       1 << 1,       0,           -1 },
	{ "Gnome-boxes", NULL,    NULL,       1 << 6,       0,           -1 },
	{ "Alacritty", "cmus",    NULL,       1 << 7,       0,           -1 },
	{ "Spotify",  NULL,       NULL,       1 << 7,       0,           -1 },
	{ "Joplin",   NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#include "fibonacci.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "[D]",      deck },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* [> component of dmenucmd, manipulated in spawn() <] */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[] = { "/bin/sh", "-c", "$TERMINAL", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_Escape, spawn,          SHCMD("slock") },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_equal,  incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_minus,  incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ Mod1Mask,                     XK_Tab,    shiftview,      {.i = +1} },
	{ Mod1Mask|ShiftMask,           XK_Tab,    shiftview,      {.i = -1} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,            		        XK_period, cyclelayout,    {.i = +1 } },
	{ MODKEY|ShiftMask,  		        XK_period, cyclelayout,    {.i = -1 } },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} }, // tiling
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[1]} }, // floating
	{ MODKEY,                       XK_y,      setlayout,      {.v = &layouts[2]} }, // monocle
	{ MODKEY|ShiftMask,             XK_y,      setlayout,      {.v = &layouts[5]} }, // deck
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[3]} }, // centered
	{ MODKEY|ShiftMask,             XK_u,      setlayout,      {.v = &layouts[4]} }, // centered floating
	{ MODKEY,                       XK_i,      setlayout,      {.v = &layouts[6]} }, // spiral
	{ MODKEY|ShiftMask,             XK_i,      setlayout,      {.v = &layouts[7]} }, // dwindle
	{ MODKEY|ShiftMask,             XK_Return, setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_x,      togglesticky,   {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },

	/* dmenu */
	{ MODKEY,                       XK_d,      spawn,          SHCMD("dmenu_run_i") },
	{ MODKEY|ShiftMask,             XK_d,      spawn,          SHCMD("j4-dmenu-desktop") },
	{ MODKEY,                       XK_s,      spawn,          SHCMD("dmenu_ssh") },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          SHCMD("dmenu_ssh --vnc") },
	{ MODKEY,                       XK_r,      spawn,          SHCMD("dmenu_config") },
	{ ControlMask|Mod1Mask,         XK_v,      spawn,          SHCMD("clipmenu") },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          SHCMD("dmenu_websearch") },

	/* Audio */
	{ 0,                            XF86XK_AudioMute, spawn,   SHCMD("pactl set-sink-volume @DEFAULT_SINK@ toggle")  },
	{ 0,                            XF86XK_AudioLowerVolume, spawn, SHCMD("pactl set-sink-volume @DEFAULT_SINK@ -5%") },
	{ 0,                            XF86XK_AudioRaiseVolume, spawn, SHCMD("pactl set-sink-volume @DEFAULT_SINK@ +5%") },
	{ MODKEY|ShiftMask,             XK_a,      spawn,          SHCMD("pavucontrol") },

	/* Printscreen */
	{ 0,                            XK_Print,      spawn,      SHCMD("maim ~/Pictures/screenshot_$(date +%s).png") },
	{ ShiftMask,                    XK_Print,      spawn,      SHCMD("maim | xclip -selection clipboard -t image/png") },
	{ Mod1Mask,                     XK_Print,      spawn,      SHCMD("maim -i $(xdotool getactivewindow) ~/Pictures/screenshot_$(date +%s).png") },
	{ Mod1Mask|ShiftMask,           XK_Print,      spawn,      SHCMD("maim -i $(xdotool getactivewindow) | xclip -selection clipboard -t image/png") },
	{ ControlMask,                  XK_Print,      spawn,      SHCMD("maim -s ~/Pictures/screenshot_$(date +%s).png") },
	{ ControlMask|ShiftMask,        XK_Print,      spawn,      SHCMD("maim -s | xclip -selection clipboard -t image/png") },

	/* Applications */
	{ 0,                            XF86XK_Calculator, spawn,  SHCMD("gnome-calculator") },
	{ MODKEY,                       XK_a,      spawn,          SHCMD("audacity") },
	{ MODKEY,                       XK_c,      spawn,          SHCMD("telegram-desktop") },
	{ MODKEY|ShiftMask,             XK_c,      spawn,          SHCMD("chrome-whatsapp") },
	{ MODKEY,                       XK_e,      spawn,          SHCMD("$TERMINAL_RUN ranger") },
	{ MODKEY|ShiftMask,             XK_e,      spawn,          SHCMD("nautilus") },
	{ MODKEY,                       XK_m,      spawn,          SHCMD("$TERMINAL_RUN cmus") },
	{ MODKEY,                       XK_n,      spawn,          SHCMD("$TERMINAL_RUN quick-note") },
	{ MODKEY|ShiftMask,             XK_n,      spawn,          SHCMD("joplin") },
	{ MODKEY,                       XK_w,      spawn,          SHCMD("x-www-browser") },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ShiftMask,             XK_r,      quit,           {1} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkTagBar,            0,              Button4,        shiftview,      {.i = -1} },
	{ ClkTagBar,            0,              Button5,        shiftview,      {.i = +1} },
};

