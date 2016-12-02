#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

//quit
void quit (GtkToggleButton *tbutton, gpointer data)
{
  gtk_main_quit ();
}


//ouvrir le dialogue
void ouvrir(GtkWidget *button, GtkWidget *imgbox)
{
  char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(button));
  gtk_image_set_from_file(GTK_IMAGE(imgbox), filename);
}

GtkBuilder* get_build()
{
  static GtkBuilder* build = NULL;
  if ( !build )
  {
    build = gtk_builder_new();
  }
  return build;
}

int main (int argc, char *argv[])
{
  GtkWidget *MainWindow = NULL;
  GtkBuilder *builder;

  gtk_init (&argc, &argv);


  builder=gtk_builder_new();
  gtk_builder_add_from_file( builder, "mikatest.glade", NULL);

  gtk_builder_connect_signals (builder, NULL);

  MainWindow=GTK_WIDGET(gtk_builder_get_object(builder,"MainWindow"));
 

  //gtk_builder_connect_signals( builder, NULL );
  g_object_unref( G_OBJECT( builder ) );


  gtk_widget_show_all (MainWindow);
  gtk_main ();
  return 0;
}
