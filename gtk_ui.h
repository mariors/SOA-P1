#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct row_struct_t {
    int id_int;
    char *id_char;
    GtkWidget *id;
    GtkWidget *progress;
    GtkWidget *status;
    GtkWidget *result;
} row_struct_t;

typedef struct state {

    int id;
    int finish;
    double percent;
    double result;
    int active;

} state;

int totalProcess;

enum { NO_EXPROPIATIVO, EXPROPIATIVO };

int numberOfDigits (int);
void update_row_active (char *, char *);
void update_row_work (char *, float, double, int, int);
void update_row_status(int id, float percent, float result, int active, int finish);
void add_row (int, char *);
void init_table ();
void create_UI (int, int);
void show_ui ();
int activeTimer ();
void stopTimer (int);
