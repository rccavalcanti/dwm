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
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Ubuntu:size=10" };
static const char dmenufont[]       = "Ubuntu:size=10";
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
	{ "Code",     NULL,       NULL,       1 << 1,       0,           -1 },
	{ "Alacritty", "cmus",    NULL,       1 << 7,       0,           -1 },
	{ "Spotify",  NULL,       NULL,       1 << 7,       0,           -1 },
	{ "Joplin",   NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
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
//static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *dmenucmd[] = { "rofi", "-show", "run", "-modi", "run,drun", "-theme", "dmenu", "-font", "Ubuntu Mono 10" };
static const char *sshcmd[] = { "rofi", "-show", "ssh", "-modi", "ssh", "-theme", "dmenu", "-font", "Ubuntu Mono 10" };
static const char *vnccmd[] = { "rofi", "-show", "vnc", "-modi", "vnc:/home/rafael/.config/rofi/scripts/vnc.sh", "-theme", "dmenu", "-font", "Ubuntu Mono 10" };
static const char *termcmd[] = { "alacritty", NULL };
static const char *lockcmd[] = { "slock", NULL };
static const char *pavucmd[] = { "pavucontrol", NULL };
static const char *upvol[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%", NULL };
static const char *downvol[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%", NULL };
static const char *mutevol[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };
static const char *audacitycmd[] = { "audacity", NULL };
static const char *browsercmd[] = { "x-www-browser", NULL };
static const char *cmuscmd[] = { "alacritty", "--class", "cmus", "-e", "cmus" };
static const char *joplincmd[] = { "joplin", NULL };
static const char *notecmd[] = { "alacritty", "-e", "vim", "+", "/home/rafael/Sync/quick_note.md" };
static const char *telegramcmd[] = { "telegram-desktop", NULL };
static const char *vimcmd[] = { "alacritty", "-e", "vim" };
static const char *whatsappcmd[] = { "whatsapp", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_r,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_s,      spawn,          {.v = sshcmd } },
	{ MODKEY,                       XK_v,      spawn,          {.v = vnccmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_Escape, spawn,          {.v = lockcmd } },
	{ MODKEY,                       XK_a,      spawn,          {.v = pavucmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_Return, setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },

	/* Audio */
	{ 0,                            XF86XK_AudioLowerVolume, spawn, {.v = downvol } },
	{ 0,                            XF86XK_AudioMute, spawn, {.v = mutevol } },
	{ 0,                            XF86XK_AudioRaiseVolume, spawn, {.v = upvol } },
	/* Applications */
	{ MODKEY|Mod1Mask,              XK_a,      spawn,          {.v = audacitycmd } },
	{ MODKEY|Mod1Mask,              XK_h,      spawn,          {.v = whatsappcmd } },
	{ MODKEY|Mod1Mask,              XK_j,      spawn,          {.v = joplincmd } },
	{ MODKEY|Mod1Mask,              XK_m,      spawn,          {.v = cmuscmd } },
	{ MODKEY|Mod1Mask,              XK_n,      spawn,          {.v = notecmd } },
	{ MODKEY|Mod1Mask,              XK_t,      spawn,          {.v = telegramcmd } },
	{ MODKEY|Mod1Mask,              XK_v,      spawn,          {.v = vimcmd } },
	{ MODKEY|Mod1Mask,              XK_w,      spawn,          {.v = browsercmd } },

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
};

