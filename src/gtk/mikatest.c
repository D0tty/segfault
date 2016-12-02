#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <gtk/gtk.h>
#include "../img/img.h"

char* chemin;


//quit
void quit (GtkToggleButton *tbutton, gpointer data)
{
  gtk_main_quit ();
}


//ouvrir le dialogue
void ouvrir(GtkWidget *button, GtkWidget *imgbox)
{
  char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(button));
  chemin = filename;
  gtk_image_set_from_file(GTK_IMAGE(imgbox), filename);
}

void grey(GtkWidget *button, GtkWidget *imgbox)
{
  init_sdl();
  SDL_Surface *img = load_image(chemin);
  img = tograyscale(img);
  SDL_SaveBMP(img, "new_img_grey");
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "new_img_grey");
  remove("new_img_grey");
  free(img);
}

/*
void save (GtkWidget *button, GtkWidget *imgbox)
{
  //chooser = GTK_FILE_CHOOSER (dialog);
  
  char* path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(button));
  SDL_SaveBMP(imgbox,path);
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
