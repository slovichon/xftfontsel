#include <err.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Viewport.h>

#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrender.h>
#include <fontconfig/fontconfig.h>

void requery(Widget, XtPointer, XtPointer);
void cb_quit(Widget, XtPointer, XtPointer);
void cb_copy(Widget, XtPointer, XtPointer);

enum proptype {
	T_STR, T_INT, T_DBL, T_BOOL, T_FT_FACE, T_CHARSET
};

struct ftprops {
	char 		*prop_name;
	char		*prop_label;
	enum proptype 	 prop_type;
	Widget		 prop_widget;
	char		*prop_desc;
} props[] = {
	/* core, xlfn */
	{ XFT_ANTIALIAS,	"aa",		T_BOOL,		NULL, "Antialiasing" },
/*	{ XFT_ASPECT,		"aspect",	T_DBL,		NULL, "Horizontal stretch" }, */
/*	{ XFT_AUTOHINT,		"autohint",	T_BOOL,		NULL, "Autohinter" }, */
	{ XFT_CHAR_WIDTH,	"charwidth",	T_INT,		NULL, "Character width"},
	{ XFT_CHAR_HEIGHT,	"charheight",	T_INT,		NULL, "Character height"},
/*	{ XFT_CHARSET,		"charset",	T_CHARSET,	NULL, "Character set" }, */
	{ XFT_DPI,		"dpi",		T_DBL,		NULL, "Dots per inch" },
/*	{ XFT_ENCODING,		"encoding",	T_DBL,		NULL, "???" }, */
	{ XFT_FAMILY,		"family",	T_STR,		NULL, "Font family name" },
	{ XFT_FILE,		"file",		T_STR,		NULL, "System filename" },
	{ XFT_FOUNDRY,		"foundry",	T_STR,		NULL, "Font foundry name" },
/*	{ XFT_FTFACE,		"ftface",	T_FT_FACE,	NULL, "FreeType face" }, */
/*	{ XFT_GLOBAL_ADVANCE,	"globaladv",	T_BOOL,		NULL, "Global advance data" }, */
/*	{ XFT_HINTING,		"hint",		T_BOOL,		NULL, "Hinting" }, */
	{ XFT_INDEX,		"index",	T_INT,		NULL, "Index of font from resource" },
/*	{ XFT_LANG,		"lang",		T_STR,		NULL, "Languages" }, */
	{ XFT_MATRIX,		"matrix",	T_BOOL,		NULL, "Modification matrix" },
	{ XFT_MINSPACE,		"minspace",	T_BOOL,		NULL, "Line space leading" },
	{ XFT_OUTLINE,		"outline",	T_BOOL,		NULL, "Outlines" },
	{ XFT_PIXEL_SIZE,	"pixelsize",	T_DBL,		NULL, "Pixel size" },
	{ XFT_RASTERIZER,	"rasterizer",	T_STR,		NULL, "Rasterizer" },
	{ XFT_RGBA,		"rgba",		T_INT,		NULL, "Subpixel geometry" },
	{ XFT_SCALABLE,		"scalable",	T_BOOL,		NULL, "Scalibility" },
	{ XFT_SCALE,		"scale",	T_DBL,		NULL, "Point to pixel scale conversion" },
	{ XFT_SIZE,		"size",		T_DBL,		NULL, "Point size" },
	{ XFT_SLANT,		"slant",	T_INT,		NULL, "Italics/roman/oblique" },
	{ XFT_SPACING,		"spacing",	T_INT,		NULL, "Proprotional/monospace/charcell" },
	{ XFT_STYLE,		"style",	T_STR,		NULL, "Font style (overrides weight/slant)" },
/*	{ XFT_VERTICAL_LAYOUT,	"vertlayout",	T_BOOL,		NULL, "Vertical layout" }, */
	{ XFT_WEIGHT,		"weight",	T_INT,		NULL, "Light/medium/bold" },
};
#define NUM_PROPS (sizeof(props) / sizeof(props[0]))

XtActionsRec actions[] = {
	{ "Quit", (void *)exit }
};

struct {
	Cursor cursor;
} appres;

Atom wm_delete_window;
XtAppContext appctx;
Widget 	quit_button, copy_button, count_label, pane,
	sample_text, top, command_box, field_box, view;

int
main(int argc, char *argv[])
{
	int i, screen;
	XftDraw *draw;
	Display *display;
	XRenderColor renclr;
	XftColor xftclr;
	Colormap cmap;

	top = XtOpenApplication(&appctx, "XftFontSel", NULL, 0, &argc, argv, NULL,
				sessionShellWidgetClass, NULL, 0);
	XtAppAddActions(appctx, actions, XtNumber(actions));

	display = XtDisplay(top);
	screen = DefaultScreen(display);
	draw = XftDrawCreate(display, XtWindow(top),
			     DefaultVisual(display, screen),
			     DefaultColormap(display, screen));

	renclr.red   = 0;
	renclr.green = 0;
	renclr.blue  = 0;
	renclr.alpha = 0xffff;
	cmap = XCreateColormap(display, XtWindow(top),
			       DefaultVisual(display, screen), 1);
	XftColorAllocValue(display, DefaultVisual(display, screen),
			   cmap, &renclr, &xftclr);
	XftDrawRect(draw, &xftclr, 5, 5, 200, 300);

	pane = XtCreateManagedWidget("pane", panedWidgetClass, top, NULL, 0);
	command_box 	= XtCreateManagedWidget("command_box", formWidgetClass, pane, NULL, 0);
	field_box 	= XtCreateManagedWidget("field_box", boxWidgetClass, pane, NULL, 0);

	for (i = 0; i < NUM_PROPS; i++) {
		props[i].prop_widget = XtCreateManagedWidget(props[i].prop_name, menuButtonWidgetClass, field_box, NULL, ZERO);
		XtAddCallback(props[i].prop_widget, XtNcallback, requery, &props[i]);
	}

	quit_button	= XtCreateManagedWidget("Quit", commandWidgetClass, command_box, NULL, 0);
	copy_button	= XtCreateManagedWidget("Copy To Clipboard", commandWidgetClass, command_box, NULL, 0);
	count_label	= XtCreateManagedWidget("count_label", labelWidgetClass, command_box, NULL, 0);

	XtAddCallback(quit_button, XtNcallback, cb_quit, NULL);
	XtAddCallback(copy_button, XtNcallback, cb_copy, NULL);

	view 		= XtCreateManagedWidget("view", viewportWidgetClass, pane, NULL, 0);
	sample_text	= XtCreateManagedWidget("sample_text", asciiTextWidgetClass, view, NULL, 0);

	XtRealizeWidget(top);
	XDefineCursor(XtDisplay(top), XtWindow(top), appres.cursor);
	wm_delete_window = XInternAtom(XtDisplay(top), "WM_DELETE_WINDOW", False);
	(void) XSetWMProtocols(XtDisplay(top), XtWindow(top), &wm_delete_window, 1);
	XtAppMainLoop(appctx);

	return 0;
}

void
cb_quit(Widget w, XtPointer closure, XtPointer callData)
{
	exit(0);
}


void
cb_copy(Widget w, XtPointer closure, XtPointer callData)
{
}

void
requery(Widget w, XtPointer closure, XtPointer callData)
{
	printf("requiried\n");
}
