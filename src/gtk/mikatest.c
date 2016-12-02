#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

//quit
void quit (GtkToggleButton *tbutton, gpointer data)
{
  gtk_main_quit ();
}

/*trouve le chemin*/
/*gchar* load(gchar* path)
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new("Load image", NULL,
  GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,
  GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    path = gtk_file_chooser_get_preview_filename (
    GTK_FILE_CHOOSER(dialog));
    GdkPixbuf *pix = gdk_pixbuf_new_from_file (path, NULL);
    pix = gdk_pixbuf_scale_simple (pix, 700, 850, GDK_INTERP_NEAREST);
    GtkWidget *image = gtk_image_new_from_pixbuf(pix);
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), path);
    gtk_container_add(GTK_CONTAINER(win), image);
    gtk_window_move(GTK_WINDOW(win), 500, 0);
    gtk_widget_show_all(win);
  }
  gtk_widget_destroy (dialog);
  return path;
}*/

//ouvrir le dialogue
void ouvrir(GtkWidget *button, GtkWidget *imgbox)
{
  char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(button));
  gtk_image_set_from_file(GTK_IMAGE(imgbox), filename);
}
/*
void charger (GtkWidget *dialog, GTK_IMAGE *img)
{


}*/

//choisir l'image
/*void choose(GtkWidget *button ,GtkWidget *dial)
{
  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint res;

  dialog = dial;
  dialog = gtk_file_chooser_dialog_new ("Open File",NULL,
      action,("_Cancel"),GTK_RESPONSE_CANCEL,("_Open"),
      GTK_RESPONSE_ACCEPT,NULL);

  res = gtk_dialog_run (GTK_DIALOG (dialog));
  if (res == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
    //path = gtk_file_chooser_get_filename (chooser);
    //load(path);
    gtk_image_set_from_file (GTK_IMAGE(image) , filename);
    g_free (filename);
  }
  gtk_widget_destroy (dialog);
}*/

GtkBuilder* get_build()
{
  static GtkBuilder* build = NULL;
  if ( !build )
  {
    build = gtk_builder_new();
  }
  return build;
}
/*
void testit(GtkToggleButton *GTKButton, gpointer data)
{
  printf("YOU DID IT !!!\n");
  gtk_main_quit();
}*/

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
