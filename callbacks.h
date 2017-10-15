#include <gtk/gtk.h>
#include <glib.h>



gpointer
get_random();


gboolean
get_data();


void
on_window1_destroy                     (GtkObject       *object,
                                        gpointer         user_data);

void
on_but_play_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_but_slow_down_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_but_speed_up_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_but_pause_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_but_stop_clicked                    (GtkButton       *button,
                                        gpointer         user_data);


void
on_drawingarea1_expose_event           (GtkWidget       *widget,
                                        gpointer         user_data);


