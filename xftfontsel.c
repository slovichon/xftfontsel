#include <err.h>

#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Cardinals.h>

#include <X11/StringDefs.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrender.h>
#include <fontconfig/fontconfig.h>

XtAppContext appctx;
Widget 	quit_button, copy_button, count_label, pane,
	sample_text, top, command_box, field_box, view;

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
	{ "family",		"family",	T_STR,		NULL, "Font family name" },
	{ "style",		"style",	T_STR,		NULL, "Font style (overrides weight/slant)" },
	{ "slant",		"slant",	T_INT,		NULL, "Italics/roman/oblique" },
	{ "weight",		"weight",	T_INT,		NULL, "Light/medium/bold" },
	{ "size",		"size",		T_DBL,		NULL, "Point size" },
	{ "aspect",		"aspect",	T_DBL,		NULL, "Horizontal stretch" },
	{ "pixelsize",		"pixelsize",	T_DBL,		NULL, "Pixel size" },
	{ "spacing",		"spacing",	T_INT,		NULL, "Proprotional/monospace/charcell" },
	{ "foundry",		"foundry",	T_STR,		NULL, "Font foundry name" },
	{ "antialias",		"aa",		T_BOOL,		NULL, "Antialiasing" },
	{ "hinting",		"hint",		T_BOOL,		NULL, "Hinting" },
	{ "verticallayout",	"vertlayout",	T_BOOL,		NULL, "Vertical layout" },
	{ "autohint",		"autohint",	T_BOOL,		NULL, "Autohinter" },
	{ "globaladvance",	"globaladv",	T_BOOL,		NULL, "Global advance data" },
	{ "file",		"file",		T_STR,		NULL, "System filename" },
	{ "index",		"index",	T_INT,		NULL, "Index of font from resource" },
	{ "ftface",		"ftface",	T_FT_FACE,	NULL, "FreeType face" },
	{ "rasterizer",		"rasterizer",	T_STR,		NULL, "Rasterizer" },
	{ "outline",		"outline",	T_BOOL,		NULL, "Outlines" },
	{ "scalable",		"scalable",	T_BOOL,		NULL, "Scalibility" },
	{ "scale",		"scale",	T_DBL,		NULL, "Point to pixel scale conversion" },
	{ "dpi",		"dpi",		T_DBL,		NULL, "Dots per inch" },
	{ "rgba",		"rgba",		T_INT,		NULL, "Subpixel geometry" },
	{ "minspace",		"minspace",	T_BOOL,		NULL, "Line space leading" },
	{ "charset",		"charset",	T_CHARSET,	NULL, "Character set" },
	{ "lang",		"lang",		T_STR,		NULL, "Languages" },
};

XtActionsRec actions[] = {
	{ "Quit", (void *)exit }
};

Atom wm_delete_window;

struct {
	Cursor cursor;
} appres;

#define NUM_PROPS (sizeof(props) / sizeof(props[0]))

void requery(Widget, XtPointer, XtPointer);
void cb_quit(Widget, XtPointer, XtPointer);
void cb_copy(Widget, XtPointer, XtPointer);

int
main(int argc, char *argv[])
{
	int i;

	XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL);
	top = XtAppInitialize(&appctx, "XftFontSel", NULL, 0, &argc, argv, NULL, NULL, ZERO);
	XtAppAddActions(appctx, actions, XtNumber(actions));
    
	pane 		= XtCreateManagedWidget("pane", panedWidgetClass, top, NULL, 0);
	command_box 	= XtCreateManagedWidget("command_box", formWidgetClass, pane, NULL, 0);
	field_box 	= XtCreateManagedWidget("field_box", boxWidgetClass, pane, NULL, 0);
	
	for (i = 0; i < NUM_PROPS; i++) {
		props[i].prop_widget = XtCreateManagedWidget(props[i].prop_name, labelWidgetClass, field_box, NULL, ZERO);
		XtAddCallback(props[i].prop_widget, XtNcallback, requery, NULL);
	}

	quit_button	= XtCreateManagedWidget("quit_button", commandWidgetClass, command_box, NULL, 0);
	copy_button	= XtCreateManagedWidget("copy_button", commandWidgetClass, command_box, NULL, 0);
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
}
