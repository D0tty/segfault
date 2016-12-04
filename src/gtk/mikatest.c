#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <gtk/gtk.h>
#include "../img/img.h"

char* chemin;
double degre=0;

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

void tobin(GtkWidget *button, GtkWidget *imgbox)                 
{                                                                               
  init_sdl();                                                                   
  SDL_Surface *img = load_image(chemin);                                        
  img = tograyscale(img);                                                       
  img = tobinary(img);
  SDL_SaveBMP(img, "new_img_grey");                                             
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "new_img_grey");                   
  remove("new_img_grey");                                                       
  free(img);                                                                    
}    

void result (GtkWidget *button, GtkLabel *label)
{
  char *text;
  text = "coucou je fonctionne";
  gtk_label_set_label(label, text);
}

void l_rot(GtkWidget *button, GtkWidget *imgbox)      
{                                                                              
  init_sdl();                                                                   
  SDL_Surface *img = load_image(chemin);                                        
  
  img = left_rotation(img);
  SDL_SaveBMP(img, "new_img_grey");                                             
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "new_img_grey");                   
  remove("new_img_grey");                                                       
  free(img);                                                                    
}  


void r_rot(GtkWidget *button, GtkWidget *imgbox)                                
{                                                                               
  init_sdl();                                                                   
  SDL_Surface *img = load_image(chemin);                                        
  img = right_rotation(img);                         
  SDL_SaveBMP(img, "new_img_bw");                                             
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "new_img_bw");                   
  remove("new_img_bw");                                                       
  free(img);                                                                    
}

void rot(GtkWidget *button, GtkWidget *imgbox)                                
{                                                                               
  init_sdl();                                                                   
  SDL_Surface *img = load_image(chemin); 
  img = rotation(img,degre);
  SDL_SaveBMP(img, "new_img_grey");                             
  gtk_image_set_from_file(GTK_IMAGE(imgbox), "new_img_grey");                   
  remove("new_img_grey");                                                       
  free(img);                                                                    
}     

void more(GtkWidget *button, GtkWidget *label)
{
  degre += 1;
  /*char* text;
  sprintf(text,"%f",degre);*/
  gtk_label_set_label(label,"%f"degre);
}

void less(GtkWidget *button, GtkWidget *label)                                  
{                                                                               
  degre -= 1;                                                                   
  //char* text = degre;                                                           
  //gtk_label_set_label(label,text);                                              
}

/*
void set_degre(GtkWidget *button, GtkWidget *entry)
{
  degre = (double)gtk_entry_get_text(entry);
  //degre = (double)angle;
}
*/
/*
void save (GtkWidget *button, GtkWidget *imgbox)
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
