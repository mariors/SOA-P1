#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "gtk_ui.h"
#include "hashmap.h"

static float percent = 0.0;
map_t mymap;
static GtkWidget *window;
GtkWidget *table;
extern int totalProcess;
state *states;
extern char **ids;
GtkWidget *label_info_value3;
int threadCompletes = 0;
int *flags;

int
numberOfDigits (int number) {

    int c = 0;
    while (number > 0) {
	number = number / 10;
	c++;
    }
    return c;
}

void
update_row_active (char *id_char, char *state) {

    row_struct_t *thread;
    int error = hashmap_get (mymap, id_char, (void **) (&thread));

    /* Make sure the value was both found and the correct number */
    assert (error == MAP_OK);

    //gdk_threads_enter();
    gtk_label_set_text (GTK_LABEL (thread->status), state);
    //gdk_threads_leave();
}

void
update_row_work (char *id_char, float percent, double result, int finish,
		 int id) {

    row_struct_t *thread;
    int error = hashmap_get (mymap, id_char, (void **) (&thread));

    /* Make sure the value was both found and the correct number */
    assert (error == MAP_OK);

    char *st;
    if (finish == 1 && !flags[id]) {

	st = "COMPLETED";
	threadCompletes++;
	int nDigits = numberOfDigits (threadCompletes);
	char *completedString = malloc (sizeof (char) * (nDigits + 1));
	sprintf (completedString, "%d", threadCompletes);
	gtk_label_set_text (GTK_LABEL (label_info_value3), completedString);
	flags[id] = 1;

    }

    gdk_threads_enter ();
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (thread->progress),
				   percent);
    char c[3];
    sprintf (c, "%d%%", (int) (percent * 100));
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (thread->progress), c);

    char d[60];
    sprintf (d, "%.50f", result);
    gtk_label_set_text (GTK_LABEL (thread->result), d);
    gdk_threads_leave ();
}

void update_row_status(int id, float percent, float result, int active, int finish){

	if (!finish) {
	    if (active) {
		update_row_active (ids[id], "ACTIVE");
	    }
	    else {
		update_row_active (ids[id], "INACTIVE");
	    }
	}
	else {
	    update_row_active (ids[id], "COMPLETED");
	}
	update_row_work (ids[id], percent, result,
			 finish, id);

}

double currentp = 0.0;

static gboolean
progress_timeout (gpointer data) {

    int k;
    int allFinished = 1;

    for (k = 0; k < totalProcess; k++) {

	if (!states[k].finish) {
	    allFinished = 0;
	    if (states[k].active) {
		update_row_active (ids[k], "ACTIVE");
	    }
	    else {
		update_row_active (ids[k], "INACTIVE");
	    }
	}
	else {
	    update_row_active (ids[k], "COMPLETED");
	}
	update_row_work (ids[k], states[k].percent, states[k].result,
			 states[k].finish, k);
    }
    if (allFinished) {
	return FALSE;
    }
    return TRUE;
}

int
activeTimer () {
    int timer = g_timeout_add (10, progress_timeout, NULL);
    return timer;
}


void
stopTimer (int timer) {
    g_source_remove (timer);
}


void
add_row (int id_int, char *id_char) {

    int bar_width = 30;
    int bar_length = 450;
    int result_length = 200;
    char c[10]; //< ID
    sprintf (c, "%d", id_int);
    GtkWidget *id = gtk_label_new (c);

    /* Progress bar */
    GtkWidget *progress = gtk_progress_bar_new ();

    float percent = 0;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress), percent);
    char d[3];
    sprintf (d, "%d%%", (int) (percent * 100));
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progress), d);
    gtk_widget_set_size_request (progress, bar_length, bar_width);

    /* Status and Result */
    GtkWidget *status = gtk_label_new ("INACTIVE");
    GtkWidget *result = gtk_label_new ("0");

    /* Creating scroll panel for result */
    GtkWidget *halign = gtk_alignment_new (0, 0, 0, 1);
    gtk_container_add (GTK_CONTAINER (halign), result);
    GtkWidget *scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_border_width (GTK_CONTAINER (scroll), 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
				    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll),
					   halign);
    gtk_widget_set_size_request (scroll, result_length, bar_width);

    /* Attaching row values to the table */
    gtk_table_attach (GTK_TABLE (table), id, 0, 1, id_int, id_int + 1,
		      GTK_FILL, GTK_FILL, 10, 10);
    gtk_table_attach (GTK_TABLE (table), progress, 1, 2, id_int, id_int + 1,
		      GTK_FILL, GTK_FILL, 0, 10);
    gtk_table_attach (GTK_TABLE (table), status, 2, 3, id_int, id_int + 1,
		      GTK_FILL, GTK_FILL, 0, 10);
    gtk_table_attach (GTK_TABLE (table), scroll, 3, 4, id_int, id_int + 1,
		      GTK_FILL, GTK_FILL, 0, 10);

    row_struct_t *thread;
    thread = malloc (sizeof (row_struct_t));
    thread->id_int = id_int;
    thread->id_char = id_char;
    thread->id = id;
    thread->progress = progress;
    thread->status = status;
    thread->result = result;

    int error = hashmap_put (mymap, id_char, thread);
    assert (error == MAP_OK);
}

/*******************************
ADDING ROWS DYNAMICALLY FUNCTION
********************************/
void
init_table () {

    /* Setting spacings for columns and rows */
    gtk_table_set_col_spacings (GTK_TABLE (table), 10);
    gtk_table_set_row_spacings (GTK_TABLE (table), 0);

    /* Creating table headers */
    GtkWidget *id_header = gtk_label_new ("Thread ID");
    GtkWidget *progress_header = gtk_label_new ("Progress");
    GtkWidget *status_header = gtk_label_new ("Status");
    GtkWidget *result_header = gtk_label_new ("Result");

    PangoFontDescription *df;
    df = pango_font_description_from_string ("Monospace");
    pango_font_description_set_size (df, 20 * PANGO_SCALE);

    gtk_widget_modify_font (id_header, df);
    gtk_widget_modify_font (progress_header, df);
    gtk_widget_modify_font (status_header, df);
    gtk_widget_modify_font (result_header, df);

    gtk_table_attach (GTK_TABLE (table), id_header, 0, 1, 0, 1, GTK_FILL,
		      GTK_FILL, 10, 10);
    gtk_table_attach (GTK_TABLE (table), progress_header, 1, 2, 0, 1,
		      GTK_FILL, GTK_FILL, 0, 10);
    gtk_table_attach (GTK_TABLE (table), status_header, 2, 3, 0, 1, GTK_FILL,
		      GTK_FILL, 0, 10);
    gtk_table_attach (GTK_TABLE (table), result_header, 3, 4, 0, 1, GTK_FILL,
		      GTK_FILL, 0, 10);

}

void
create_UI (int n_threads, int mode) {

    mymap = hashmap_new ();
    window = gtk_dialog_new ();
    table = gtk_table_new (n_threads, 4, FALSE);
    gtk_window_set_title (GTK_WINDOW (window), "Lottery Scheduler");
    gtk_window_set_default_size (GTK_WINDOW (window), 1000, 605);
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);

    flags = (int *) malloc (sizeof (int) * n_threads);
    int k;
    for (k = 0; k < n_threads; k++) {
	flags[k] = 0;
    }

    /* Creating main app title */
    GtkWidget *app_title = gtk_label_new ("Thread Status Monitor");
    PangoFontDescription *df;
    df = pango_font_description_from_string ("Monospace");
    pango_font_description_set_size (df, 30 * PANGO_SCALE);
    gtk_widget_modify_font (app_title, df);

    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), app_title, FALSE,
			FALSE, 15);

    /* Creating info table */
    GtkWidget *table_info = gtk_table_new (3, 2, TRUE);
    gtk_table_set_col_spacings (GTK_TABLE (table_info), 5);
    gtk_table_set_row_spacings (GTK_TABLE (table_info), 5);

    GtkWidget* label_info_name1 = gtk_label_new ("Operation Mode:");
    GtkWidget *label_info_name2 = gtk_label_new ("Threads Submitted:");
//    GtkWidget* label_info_name2 = gtk_label_new ("Threads Running:");
    GtkWidget *label_info_name3 = gtk_label_new ("Threads Completed:");

    int nDigits = numberOfDigits (n_threads);
    char *submittedThreads = malloc (sizeof (char) * (nDigits + 1));
    sprintf (submittedThreads, "%d", n_threads);

//      GtkWidget* label_info_value2 = gtk_label_new ("1");
    GtkWidget* label_info_value1;
    if (mode == EXPROPIATIVO){
	label_info_value1 = gtk_label_new ("Expropiativo");
    }else{
	label_info_value1 = gtk_label_new ("No expropiativo");
    }
    GtkWidget *label_info_value2 = gtk_label_new (submittedThreads);
    label_info_value3 = gtk_label_new ("0");

    PangoFontDescription *df_info;
    df_info = pango_font_description_from_string ("Monospace");
    pango_font_description_set_size (df_info, 15 * PANGO_SCALE);

    gtk_widget_modify_font (label_info_name1, df_info);
    gtk_widget_modify_font(label_info_name2, df_info);
    gtk_widget_modify_font (label_info_name3, df_info);
    gtk_widget_modify_font (label_info_value1, df_info);
    gtk_widget_modify_font(label_info_value2, df_info);
    gtk_widget_modify_font (label_info_value3, df_info);

    GtkWidget *halign_info_name1 = gtk_alignment_new (0, 0, 0, 1);
    GtkWidget *halign_info_name2 = gtk_alignment_new(0, 0, 0, 1);
    GtkWidget *halign_info_name3 = gtk_alignment_new (0, 0, 0, 1);
    GtkWidget *halign_info_value1 = gtk_alignment_new (0, 0, 0, 1);
    GtkWidget *halign_info_value2 = gtk_alignment_new(0, 0, 0, 1);
    GtkWidget *halign_info_value3 = gtk_alignment_new (0, 0, 0, 1);

    gtk_container_add (GTK_CONTAINER (halign_info_name1), label_info_name1);
    gtk_container_add (GTK_CONTAINER(halign_info_name2), label_info_name2);
    gtk_container_add (GTK_CONTAINER (halign_info_name3), label_info_name3);
    gtk_container_add (GTK_CONTAINER (halign_info_value1), label_info_value1);
    gtk_container_add (GTK_CONTAINER(halign_info_value2), label_info_value2);
    gtk_container_add (GTK_CONTAINER (halign_info_value3), label_info_value3);

    gtk_table_attach_defaults (GTK_TABLE (table_info), halign_info_name1, 0,
			       1, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE(table_info), halign_info_name2, 0, 1, 1, 2 );
    gtk_table_attach_defaults (GTK_TABLE (table_info), halign_info_name3, 0,
			       1, 2, 3);

    gtk_table_attach_defaults (GTK_TABLE (table_info), halign_info_value1, 1,
			       2, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE(table_info), halign_info_value2, 1, 2, 1, 2 );
    gtk_table_attach_defaults (GTK_TABLE (table_info), halign_info_value3, 1,
			       2, 2, 3);

    /* Attaching info table to main pannel */
    GtkWidget *hbox_table_info = gtk_hbox_new (TRUE, 1);
    gtk_box_pack_start (GTK_BOX (hbox_table_info), table_info, FALSE, FALSE,10);
    GtkWidget *halign_table_info = gtk_alignment_new (0, 0, 0, 0);
    gtk_container_add (GTK_CONTAINER (halign_table_info), hbox_table_info);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox),
			halign_table_info, FALSE, FALSE, 15);

    /* Creating scroll panel for progress table */
    GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_border_width (GTK_CONTAINER (scrolled_window), 10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
				    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), scrolled_window,
			TRUE, TRUE, 0);

    /* Attaching progress table to scroll panel */
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW
					   (scrolled_window), table);

    /* init the table */
    init_table ();

    /* Creating Close Button */
    GtkWidget *button = gtk_button_new_with_label ("Close");
    g_signal_connect_swapped (button, "clicked",
			      G_CALLBACK (gtk_widget_destroy), window);
    GtkWidget *hbox_button = gtk_hbox_new (TRUE, 1);
    gtk_box_pack_start (GTK_BOX (hbox_button), button, FALSE, FALSE, 10);
    GtkWidget *halign_button = gtk_alignment_new (1, 0, 0, 0);
    gtk_container_add (GTK_CONTAINER (halign_button), hbox_button);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->vbox), halign_button,
			FALSE, FALSE, 0);

    /* Adding quit buttom event */
    g_signal_connect (G_OBJECT (window), "destroy",
		      G_CALLBACK (gtk_main_quit), NULL);
}

void
show_ui () {
    gtk_widget_show_all (window);
}
