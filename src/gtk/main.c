#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <gtk/gtk.h>
#include "../img/img.h"
#include "../v2/sdl.h"
#include "../v2/world.h"

char* chemin;
double degre=0;
SDL_Surface* g_img = NULL;

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


void save(GtkWidget *button, GtkLabel *label)
{
  const char* text;
  text = gtk_label_get_label(label);
  char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(button));
  FILE* fichier = NULL;
  fichier = fopen(strcat(filename,"text.txt"),"w");
  if (fichier != NULL)
  {
    fputs(text, fichier);
    fclose(fichier);
  }
}


void grey(GtkWidget *button, GtkWidget *imgbox)
{
  init_sdl();
  SDL_Surface *img;
  if (g_img == NULL)
  {
    g_img = load_image(chemin);
    img = g_img;
  }
  else
  {
    img = g_img;
  }
  tograyscale(img);
  SDL_SaveBMP(img, "new_img_grey");
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "new_img_grey");
  remove("new_img_grey");
}

void tobin(GtkWidget *button, GtkWidget *imgbox)
{
  init_sdl();
  SDL_Surface *img;
  if (g_img == NULL)
  {
    g_img = load_image(chemin);
    img = g_img;
  }
  else
  {
    img = g_img;
  }
  tograyscale(img);
  tobinary(img,140);
  SDL_SaveBMP(img, "new_img_grey");
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "new_img_grey");
  remove("new_img_grey");
}

GtkLabel *g_lab;

void result (GtkWidget *button, GtkLabel *label)
{
  g_lab = label;
}

void result2 (GtkWidget *button, GtkWidget *imgbox)
{
  SDL_Surface* img;
  size_t text_length;
  wchar_t* text;
  the_world("all3.network", "all.charcodes", chemin, &img, &text, &text_length);
  char* txt2  = malloc ( text_length * 2 * sizeof(char) );
  wcstombs(txt2, text, text_length * 2);
  gtk_label_set_label(g_lab, txt2);
  SDL_SaveBMP(img, "result");
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "result");
  remove("result");
}

void l_rot(GtkWidget *button, GtkWidget *imgbox)
{
  if (strcmp(chemin,"") ==0)
  {
    return;
  }
  init_sdl();
  SDL_Surface *img;
  if (g_img == NULL)
  {
    g_img = load_image(chemin);
    img = g_img;
  }
  else
  {
    img = g_img;
  }
  img = left_rotation(img);
  SDL_SaveBMP(img, "new_img_grey");
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "new_img_grey");
  remove("new_img_grey");
  g_img = img;
}


void r_rot(GtkWidget *button, GtkWidget *imgbox)
{
  init_sdl();
  SDL_Surface *img;
  if (g_img == NULL)
  {
    g_img = load_image(chemin);
    img = g_img;
  }
  else
  {
    img = g_img;
  }
  img = right_rotation(img);
  SDL_SaveBMP(img, "new_img_bw");
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "new_img_bw");
  remove("new_img_bw");
  g_img = img;
}

void rot(GtkWidget *button, GtkWidget *imgbox)
{
  init_sdl();
  SDL_Surface *img;
  if (g_img == NULL)
  {
    g_img = load_image(chemin);
    img = g_img;
  }
  else
  {
    img = g_img;
  }
  img = rotation(img,degre);
  SDL_SaveBMP(img, "new_img_grey");
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "new_img_grey");
  g_img = load_image("new_img_grey");
  remove("new_img_grey");
}

void more(GtkWidget *button, GtkLabel *label)
{
  degre += 1;
  char text[5];
  snprintf(text,5,"%f",degre);
  gtk_label_set_label(label,text);
}

void less(GtkWidget *button, GtkLabel *label)
{
  degre -= 1;
  char text[5];
  snprintf(text,5,"%f",degre);
  gtk_label_set_label(label,text);
}


/*void save_lab(GtkWidget *button, GtkWidget *imgbox)
{
//chooser = GTK_FILE_CHOOSER (dialog);

char* path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(button));
SDL_SaveBMP(imgbox,path);
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
}

GtkBuilder* get_build()
{
  static GtkBuilder* build = NULL;
  if ( !build )
  {
    build = gtk_builder_new();
  }
  return build;
}*/

int main (int argc, char *argv[])
{
  GtkWidget *MainWindow = NULL;
  GtkBuilder *builder;

  gtk_init (&argc, &argv);


  builder=gtk_builder_new();
  gtk_builder_add_from_file( builder, "main.glade", NULL);

  gtk_builder_connect_signals (builder, NULL);

  MainWindow=GTK_WIDGET(gtk_builder_get_object(builder,"MainWindow"));


  //gtk_builder_connect_signals( builder, NULL );
  g_object_unref( G_OBJECT( builder ) );


  gtk_widget_show_all (MainWindow);
  gtk_main ();
  return 0;
}
