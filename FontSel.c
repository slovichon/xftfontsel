#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include "FontSel.h"

static XtResource res[] = {
};

static void initclass(void);
static void init(Widget, Widget, ArgList, Cardinal *);
static void destroy(Widget);
static void realize(Widget, XtValueMask *, XSetWindowAttributes *);
static void resize(Widget);
static void redisplay(Widget, XEvent *, Region);
static Boolean setvals(Widget, Widget, Widget, ArgList, Cardinal *);

FontSelClassRec fontSelClassRec = {
	{
	/* core fields			*/
	/* superclass			*/	(WidgetClass) &simpleClassRec,
	/* class_name			*/	"FontSel",
	/* widget_size			*/	sizeof(FontSelRec),
	/* class_initialize		*/	initclass,
	/* class_part_initialize	*/	NULL,
	/* class_inited			*/	FALSE,
	/* initialize			*/	init,
	/* initialize_hook		*/	NULL,
	/* realize			*/	realize,
	/* actions			*/	NULL,
	/* num_actions			*/	0,
	/* resources			*/	res,
	/* resource_count		*/	XtNumber(res),
	/* xrm_class			*/	NULLQUARK,
	/* compress_motion		*/	TRUE,
	/* compress_exposure		*/	TRUE,
	/* compress_enterleave		*/	TRUE,
	/* visible_interest		*/	FALSE,
	/* destroy			*/	destroy,
	/* resize			*/	resize,
	/* expose			*/	redisplay,
	/* set_values			*/	setvals,
	/* set_values_hook		*/	NULL,
	/* set_values_almost		*/	XtInheritSetValuesAlmost,
	/* get_values_hook		*/	NULL,
	/* accept_focus			*/	NULL,
	/* version			*/	XtVersion,
	/* callback_private		*/	NULL,
	/* tm_table			*/	NULL,
	/* query_geometry		*/	XtInheritQueryGeometry,
	/* display_accelerator		*/	XtInheritDisplayAccelerator,
	/* extension			*/	NULL
	},
	{ /* simple fields */
	/* change_sensitive         */      XtInheritChangeSensitive
	},
	{ /* logo fields */
	/* ignore                   */      0
	}
};

WidgetClass fontSelWidgetClass = (WidgetClass)&fontSelClassRec;

static void
RenderPrepare(FontSelWidget *w)
{
	Display *d = XtDisplay(w);
	if (w->fontsel.draw == NULL)
		w->fontsel.draw = XftDrawCreate(d, XtWindow(w),
						DefaultVisual(d, DefaultScreen(d)),
						w->core.colormap);
}

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
