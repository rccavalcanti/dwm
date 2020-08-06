/* Modifications by Rafael Cavalcanti - rafaelc.org
 * Based on dwm original configuration */

/* See LICENSE file for copyright and license details. */

/* For multimedia keys */
#include <X11/XF86keysym.h>

/* patches */
static const unsigned int gappx     = 6;        /* gap pixel between windows */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 4;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "Fira Mono:size=9" };
static const char dmenufont[]       = "Fira Mono:size=9";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#994f00";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { " WWW ", " DEV ", "  3  ", "  4  ", "  5  ", " VMs ", " CHT ", " MUS ", " NOT " };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Gnome-calculator", NULL, NULL,     0,            1,           -1 },
	{ "Nitrogen", NULL,       NULL,       0,            1,           -1 },
	{ "Code",     NULL,       NULL,       1 << 1,       0,           -1 },
	{ "Gnome-boxes", NULL,    NULL,       1 << 5,       0,           -1 },
	{ "TelegramDesktop", NULL, NULL,      1 << 6,       0,           -1 },
	{ "Google-chrome", "crx_hnpfjngllnobngcgfapefoaidbinmjnm", NULL, 1 << 6, 0, -1 },
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
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_g,     ACTION##stack, {.i = 0 } }, \
	//{ MOD, XK_a,     ACTION##stack, {.i = 1 } }, \
	//{ MOD, XK_z,     ACTION##stack, {.i = 2 } }, \
	//{ MOD, XK_x,     ACTION##stack, {.i = -1 } },
	//{ MOD, XK_q,     ACTION##stack, {.i = 0 } },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* [> component of dmenucmd, manipulated in spawn() <] */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, "-b", NULL };
static const char *termcmd[] = { "/bin/sh", "-c", "$TERMINAL", NULL };
static const char *volup[] = { "/bin/sh", "-c",  "pactl set-sink-volume @DEFAULT_SINK@ +5% && dwmbar", NULL };
static const char *voldown[] = { "/bin/sh", "-c",  "pactl set-sink-volume @DEFAULT_SINK@ -5% && dwmbar", NULL };
static const char *mute[] = { "/bin/sh", "-c",  "pactl set-sink-mute @DEFAULT_SINK@ toggle && dwmbar", NULL };
static const char *player_prev[] = { "/bin/sh", "-c", "playerctl --player=%any,chromium previous && dwmbar", NULL };
static const char *player_next[] = { "/bin/sh", "-c", "playerctl --player=%any,chromium next && dwmbar", NULL };
static const char *player_pause[] = { "/bin/sh", "-c", "playerctl --player=%any,chromium play-pause && dwmbar", NULL };
static const char *player_stop[] = { "/bin/sh", "-c", "playerctl --player=%any,chromium stop && dwmbar", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_Escape, spawn,          SHCMD("slock") },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_equal,  incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_minus,  incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY,                       XK_z,      view,           {0} },
	{ MODKEY,                       XK_Tab,    shiftview,      {.i = +1} },
	{ MODKEY|ShiftMask,             XK_Tab,    shiftview,      {.i = -1} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_comma, cyclelayout,    {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma, cyclelayout,    {.i = -1 } },
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
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_d,      spawn,          SHCMD("j4-dmenu-desktop") },
	{ MODKEY,                       XK_s,      spawn,          SHCMD("dmenu_ssh") },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          SHCMD("dmenu_ssh --vnc") },
	{ MODKEY,                       XK_r,      spawn,          SHCMD("dmenu_config") },
	{ ControlMask|Mod1Mask,         XK_v,      spawn,          SHCMD("clipmenu") },

	/* Audio */
	{ 0,                            XF86XK_AudioMute, spawn,   {.v = mute } },
	{ 0,                            XF86XK_AudioLowerVolume, spawn, {.v = voldown } },
	{ 0,                            XF86XK_AudioRaiseVolume, spawn, {.v = volup } },
	/* Command any player but select Chromium last */
	{ 0,                            XF86XK_AudioPrev, spawn,   {.v = player_prev } },
	{ 0,                            XF86XK_AudioNext, spawn,   {.v = player_next } },
	{ 0,                            XF86XK_AudioPlay, spawn,   {.v = player_pause } },
	{ 0,                            XF86XK_AudioStop, spawn,   {.v = player_stop } },
	{ MODKEY|ShiftMask,             XK_a,      spawn,          SHCMD("pavucontrol") },

	/* Printscreen */
	{ 0,                            XK_Print,      spawn,      SHCMD("screenshot fullfile") },
	{ ShiftMask,                    XK_Print,      spawn,      SHCMD("screenshot fullcopy") },
	{ Mod1Mask,                     XK_Print,      spawn,      SHCMD("screenshot winfile") },
	{ Mod1Mask|ShiftMask,           XK_Print,      spawn,      SHCMD("screenshot wincopy") },
	{ ControlMask,                  XK_Print,      spawn,      SHCMD("screenshot selfile") },
	{ ControlMask|ShiftMask,        XK_Print,      spawn,      SHCMD("screenshot selcopy") },

	/* Applications */
	{ 0,                            XF86XK_Calculator, spawn,  SHCMD("gnome-calculator") },
	{ MODKEY,                       XK_a,      spawn,          SHCMD("audacity") },
	{ MODKEY,                       XK_c,      spawn,          SHCMD("org.telegram.desktop") },
	{ MODKEY|ShiftMask,             XK_c,      spawn,          SHCMD("chrome-whatsapp") },
	{ MODKEY,                       XK_e,      spawn,          SHCMD("$TERMINAL_RUN zsh -c 'source /usr/share/autojump/autojump.zsh && ranger'") },
	{ MODKEY|ShiftMask,             XK_e,      spawn,          SHCMD("nautilus") },
	{ MODKEY,                       XK_m,      spawn,          SHCMD("chrome-myfitnesspal") },
	{ MODKEY|ShiftMask,             XK_m,      spawn,          SHCMD("$TERMINAL_RUN cmus") },
	{ MODKEY,                       XK_n,      spawn,          SHCMD("$TERMINAL_RUN quick-note") },
	{ MODKEY|ShiftMask,             XK_n,      spawn,          SHCMD("joplin") },
	{ MODKEY,                       XK_p,      spawn,          SHCMD("chrome-projetos") },
	{ MODKEY,                       XK_w,      spawn,          SHCMD("x-www-browser") },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          SHCMD("x-www-browser --incognito") },

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
	{ ClkStatusText,        0,              Button1,        spawn,          {.v = player_next } },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = player_prev } },
	{ ClkStatusText,        0,              Button3,        spawn,          {.v = player_pause } },
	{ ClkStatusText,        0,              Button4,        spawn,          {.v = volup } },
	{ ClkStatusText,        0,              Button5,        spawn,          {.v = voldown } },
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

