#include <gtk/gtk.h>

#include "parameter.h"


gint
CloseAppWindow (GtkWidget * widget, gpointer * data)
{
	gtk_main_quit ();

	return (FALSE);
}

void
button_event (GtkWidget * widget, gpointer * data)
{
	g_print ("Button event: %s\n", data);
}

int
d1main (int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *button;

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title (GTK_WINDOW (window), "Push Button");

	gtk_signal_connect (GTK_OBJECT (window), "delete_event",
			    GTK_SIGNAL_FUNC (CloseAppWindow), NULL);

	gtk_container_border_width (GTK_CONTAINER (window), 50);

	button = gtk_button_new_with_label ("Button");

	gtk_widget_show (button);

	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			    GTK_SIGNAL_FUNC (button_event), "pressed");
	gtk_signal_connect (GTK_OBJECT (button), "released",
			    GTK_SIGNAL_FUNC (button_event), "released");
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (button_event), "clicked");
	gtk_signal_connect (GTK_OBJECT (button), "enter",
			    GTK_SIGNAL_FUNC (button_event), "enter");
	gtk_signal_connect (GTK_OBJECT (button), "leave",
			    GTK_SIGNAL_FUNC (button_event), "leave");

	gtk_container_add (GTK_CONTAINER (window), button);

	gtk_widget_show (window);

	gtk_main ();
	return (0);
}

float *E;
int patternSize;

void
loadData (char *filename)
{
	char buffer[MAXSIZE];
	char key[MAXSIZE];
	int i, s;

	getValue (buffer, "pattern.size", filename);
	patternSize = atoi (buffer);

	E = (float *) malloc (sizeof (float) * patternSize * 2);

	g_print ("Pattern size:%d\n", patternSize);

	for (s = 0; s < patternSize; s++)
	{
		g_print (".");

		for (i = 0; i < 2; i++)
		{
			sprintf (key, "pattern.in.%d.%d", s, i);
			getQuickValue (buffer, key, filename);
			E[s * 2 + i] = atof (buffer);
		}
	}

	g_print ("Data read...\n");

}

void
quit ()
{
	gtk_exit (0);
}

int
getx (int s)
{
	return (E[s * 2 + 0] + 1) * 250;

}

int
gety (int s)
{
	return ((E[s * 2 + 1] + 1) * 250);
}


gint
Repaint (gpointer data)
{
	GtkWidget *drawing_area = (GtkWidget *) data;
	GdkDrawable *drawable;
	int midx, midy;
	int patternSize;
	char buffer[MAXSIZE];
	int x, y, s;
	int xx, yy;
	int pos;
	int division;

	drawable = drawing_area->window;

	midx = drawing_area->allocation.width / 2;
	midy = drawing_area->allocation.height / 2;


	getValue (buffer, "pattern.size", "kohonen.weight.out");
	patternSize = atoi (buffer);

	division = (int) sqrt ((double) patternSize);

	for (s = 0; s < patternSize; s++)
	{
		gdk_draw_point (drawable, drawing_area->style->black_gc,
				getx (s), gety (s));
	}

	for (s = 1; s < patternSize; s++)
	{

		if ((s % division) != 0)
			gdk_draw_line (drawable,
				       drawing_area->style->black_gc,
				       getx (s - 1), gety (s - 1), getx (s),
				       gety (s));

		if (((s / division) != 0) && (s >= division))
			gdk_draw_line (drawable,
				       drawing_area->style->black_gc,
				       getx (s - division),
				       gety (s - division), getx (s),
				       gety (s));
	}

	gdk_draw_line (drawable, drawing_area->style->black_gc, midx, 0, midx,
		       drawing_area->allocation.height);
	gdk_draw_line (drawable, drawing_area->style->black_gc, 0, midy,
		       drawing_area->allocation.width, midy);

}

int
main (int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *drawing_area;
	GtkWidget *vbox;

	// inicializar GTK
	gtk_init (&argc, &argv);

	if (argc < 2) {
		printf("Especifique el archivo con los patrones....\n");
	}

	loadData (argv[1]);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	vbox = gtk_hbox_new (FALSE, 0);

	gtk_container_add (GTK_CONTAINER (window), vbox);

	gtk_widget_show (vbox);

	gtk_signal_connect (GTK_OBJECT (window), "destroy",
			    GTK_SIGNAL_FUNC (quit), NULL);

	drawing_area = gtk_drawing_area_new ();

	gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area), 500, 500);

	gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);

	gtk_widget_show (drawing_area);

	gtk_widget_show (window);

	gtk_timeout_add (100, Repaint, (gpointer) drawing_area);

	gtk_main ();

	return (0);
}
