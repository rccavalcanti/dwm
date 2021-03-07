/* Author: Rafael Cavalcanti - rafaelc.org
 * Based on dwm original configuration */

/* See LICENSE file for copyright and license details. */

/* For multimedia keys */
#include <X11/XF86keysym.h>

/* For extra mouse buttons */
#define Button6 6
#define Button7 7
#define Button8 8
#define Button9 9

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 4;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int user_bh            = 20;       /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const char *fonts[]          = { "Noto Mono:size=9" };

/* colorscheme from pywal or default */
#include "colors.h"

/* tagging */
static const char *tags[] = { " 1 ", " 2 ", " 3 ", " 4 ", " 5 ", " 6 ", " 7 ", " 8 ", " 9 "};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   issticky   isterminal  noswallow  monitor */
	/* Floating */
	{ "Gnome-calculator", NULL, NULL,     0,            1,           0,          0,          0,         -1 },
	{ "Nitrogen", NULL,       NULL,       0,            1,           0,          0,          0,         -1 },
	{ "Pavucontrol", NULL,    NULL,       0,            1,           0,          0,          0,         -1 },
	{ "Blueman-manager", NULL, NULL,      0,            1,           0,          0,          0,         -1 },
	{ "Nm-connection-editor", NULL, NULL,      0,            1,           0,          0,          0,         -1 },

	/* Floating */
	{ "mpv",      NULL,       NULL,       0,            0,           1,          0,          0,         -1 },

	/* Swallow */
	{ "Alacritty", NULL,      NULL,       0,            0,           0,          1,          0,         -1 },
	{ NULL,       NULL,       "Event Tester", 0,        0,           0,          0,          1,         -1 },

	/* Tags */
	{ "Alacritty", "sys",     NULL,       1 << 1,       0,           0,          0,          0,         -1 },
	{ "Gnome-boxes", NULL,    NULL,       1 << 5,       0,           0,          0,          0,         -1 },
	{ "Virt-manager", NULL,   NULL,       1 << 5,       0,           0,          0,          0,         -1 },
	{ "VirtualBox Manager", NULL, NULL,   1 << 5,       0,           0,          0,          0,         -1 },
	{ "TelegramDesktop", NULL, NULL,      1 << 6,       0,           0,          0,          0,         -1 },
	{ NULL, "web.whatsapp.com", NULL,     1 << 6,       0,           0,          0,          0,         -1 },
	{ "Alacritty", "cmus",    NULL,       1 << 7,       0,           0,          0,          0,         -1 },
	{ "Spotify",  NULL,       NULL,       1 << 7,       0,           0,          0,          0,         -1 },
	{ NULL, "docs.google.com__spreadsheets_d_14uu321lifzWad1456NsA6FgS8febTf64zDtL2nu29Is", NULL, 1 << 8, 0, 0, 0, 0, -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
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
static const char *dmenucmd[] = { "dmenu_run++", "-m", dmenumon, NULL };
static const char *termcmd[] = { "/bin/sh", "-c", "$TERMINAL", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "alacritty", "-t", scratchpadname, "-o", "window.dimensions.columns=105", "-o", "window.dimensions.lines=30", NULL };
static const char *volup[] = { "/bin/sh", "-c",  "pactl set-sink-volume @DEFAULT_SINK@ +5% && dwmbar", NULL };
static const char *voldown[] = { "/bin/sh", "-c",  "pactl set-sink-volume @DEFAULT_SINK@ -5% && dwmbar", NULL };
static const char *mute[] = { "/bin/sh", "-c",  "pactl set-sink-mute @DEFAULT_SINK@ toggle && dwmbar", NULL };
static const char *player_prev[] = { "/bin/sh", "-c", "playerctl previous && sleep 0.5 && dwmbar", NULL };
static const char *player_next[] = { "/bin/sh", "-c", "playerctl next && sleep 0.5 && dwmbar", NULL };
static const char *player_pause[] = { "/bin/sh", "-c", "playerctl play-pause && sleep 0.5 && dwmbar", NULL };
static const char *player_stop[] = { "/bin/sh", "-c", "playerctl stop && sleep 0.5 && dwmbar", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Return, togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_Escape, spawn,          SHCMD("slock") },
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY,                       XK_z,      view,           {0} },
	{ MODKEY,                       XK_Tab,    shiftview,      {.i = +1} },
	{ MODKEY|ShiftMask,             XK_Tab,    shiftview,      {.i = -1} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_q,      spawn,          SHCMD("xkill") },
	{ MODKEY,                       XK_comma,  cyclelayout,    {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  cyclelayout,    {.i = -1 } },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} }, // tiling
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[2]} }, // floating
	{ MODKEY,                       XK_y,      setlayout,      {.v = &layouts[1]} }, // monocle
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[3]} }, // centered
	{ MODKEY|ShiftMask,             XK_u,      setlayout,      {.v = &layouts[4]} }, // centered floating
	//{ MODKEY|ShiftMask,             XK_Return, setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_x,      togglesticky,   {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },

	/* dmenu */
	{ MODKEY,                       XK_d,      spawn,          SHCMD("dmenu_run++ -g 10 -l 15")  },
	{ MODKEY|ShiftMask,             XK_d,      spawn,          SHCMD("j4-dmenu-desktop --dmenu='dmenu -g 10 -l 15'") },
	{ MODKEY|ShiftMask,             XK_e,      spawn,          SHCMD("dmenu_yts") },
	{ MODKEY,                       XK_s,      spawn,          SHCMD("dmenu_ssh") },
	{ ControlMask|Mod1Mask,         XK_v,      spawn,          SHCMD("clipmenu") },
	{ MODKEY,                       XK_F1,     spawn,          SHCMD("dmenu_man") },
	{ Mod1Mask|ControlMask,         XK_Delete, spawn,          SHCMD("dmenu_power") },

	/* Audio and bluetooth */
	{ 0,                            XF86XK_AudioMute, spawn,   {.v = mute } },
	{ 0,                            XF86XK_AudioLowerVolume, spawn, {.v = voldown } },
	{ 0,                            XF86XK_AudioRaiseVolume, spawn, {.v = volup } },
	{ 0,                            XF86XK_AudioPrev, spawn,   {.v = player_prev } },
	{ 0,                            XF86XK_AudioNext, spawn,   {.v = player_next } },
	{ 0,                            XF86XK_AudioPlay, spawn,   {.v = player_pause } },
	{ 0,                            XF86XK_AudioStop, spawn,   {.v = player_stop } },
	{ MODKEY,                       XK_v,      spawn,          SHCMD("pavucontrol") },
	{ MODKEY,                       XK_b,      spawn,          SHCMD("blueman-manager") },

	/* Printscreen */
	{ 0,                            XK_Print,      spawn,      SHCMD("screenshot") },
	{ ShiftMask,                    XK_Print,      spawn,      SHCMD("screenshot 'Selection (clipboard)'") },

	/* Applications */
	{ 0,                            XF86XK_Calculator, spawn,  SHCMD("gnome-calculator") },
	{ MODKEY,                       XK_c,      spawn,          SHCMD("webapp whatsapp & org.telegram.desktop") },
	{ MODKEY|ShiftMask,             XK_c,      spawn,          SHCMD("killall telegram-desktop.bin & xdotool windowclose `xdotool search --name WhatsApp`") },
	{ MODKEY,                       XK_e,      spawn,          SHCMD("$TERMINAL -e vim +Files $HOME") },
	{ MODKEY,                       XK_f,      spawn,          SHCMD("$TERMINAL -e ranger") },
	{ MODKEY|ShiftMask,             XK_f,      spawn,          SHCMD("nemo") },
	{ MODKEY,                       XK_m,      spawn,          SHCMD("spotify") },
	{ MODKEY|ShiftMask,             XK_m,      spawn,          SHCMD("$TERMINAL --class cmus -e cmus") },
	{ MODKEY,                       XK_n,      spawn,          SHCMD("$TERMINAL -e notes") },
	{ MODKEY|ShiftMask,             XK_n,      spawn,          SHCMD("$TERMINAL -e notes r") },
	{ MODKEY|Mod1Mask,              XK_n,      spawn,          SHCMD("$TERMINAL -e notes f") },
	{ MODKEY|ControlMask,           XK_n,      spawn,          SHCMD("$TERMINAL -e notes j") },
	{ MODKEY,                       XK_o,      spawn,          SHCMD("webapp euinc") },
	{ MODKEY|ShiftMask,             XK_o,      spawn,          SHCMD("light-dark") },
	{ MODKEY,                       XK_p,      spawn,          SHCMD("keepassxc") },
	{ MODKEY,                       XK_r,      spawn,          SHCMD("virt-manager") },
	{ MODKEY,                       XK_w,      spawn,          SHCMD("x-www-browser") },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          SHCMD("x-www-browser --incognito") },
	{ MODKEY|Mod1Mask,              XK_w,      spawn,          SHCMD("chromium") },
	{ MODKEY,                       XK_a,      spawn,          SHCMD("$TERMINAL -e taskwarrior-tui") },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        spawn,          SHCMD("zenity --calendar") },
	{ ClkStatusText,        0,              Button2,        spawn,          SHCMD("pavucontrol") },
	{ ClkStatusText,        0,              Button9,        spawn,          {.v = player_next } },
	{ ClkStatusText,        0,              Button8,        spawn,          {.v = player_prev } },
	{ ClkStatusText,        0,              Button3,        spawn,          {.v = player_pause } },
	{ ClkStatusText,        0,              Button7,        spawn,          {.v = volup } },
	{ ClkStatusText,        0,              Button6,        spawn,          {.v = voldown } },
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

