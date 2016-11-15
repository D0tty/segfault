#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

void kaczka (GtkToggleButton *tbutton, gpointer data)
{
  gtk_main_quit ();
}

void testit(GtkToggleButton *GTKButton, gpointer data)
{
  printf("YOU DID IT !!!\n");
  gtk_main_quit();
}


int main (int argc, char *argv[])
{
  GtkWidget *win = NULL;
  GtkBuilder *builder;

  gtk_init (&argc, &argv);


  builder=gtk_builder_new();
  gtk_builder_add_from_file( builder, "test_callback.glade", NULL);

  win=GTK_WIDGET(gtk_builder_get_object(builder,"window1"));


  gtk_builder_connect_signals( builder, NULL );
  //g_object_unref( G_OBJECT( builder ) );


  gtk_widget_show_all (win);
  gtk_main ();
  return 0;
}
