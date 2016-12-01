#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

void kaczka (GtkToggleButton *tbutton, gpointer data)
{
  gtk_main_quit ();
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

void testit(GtkToggleButton *GTKButton, gpointer data)
{
  printf("YOU DID IT !!!\n");
  gtk_main_quit();
}


int main (int argc, char *argv[])
{
  GtkWidget *win = NULL;
  GtkBuilder *builder;
  GError *error = NULL;

  gtk_init (&argc, &argv);


  builder = gtk_builder_new();
  
  if(! gtk_builder_add_from_file( builder, "test_callback.glade", &error) )
  {
    g_warning( "%s", error->message );                                      
    g_free( error );                                                        
    return( 1 );    
  }

  win=GTK_WIDGET(gtk_builder_get_object(builder,"window1"));


  gtk_builder_connect_signals( builder, NULL );
  g_object_unref( G_OBJECT( builder ) );


  gtk_widget_show (win);
  gtk_main ();
  return ( 0 );
}
