#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <time.h>
#include <pthread.h>
#include <glib.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "liblinklist.h"



guint act_id = 0;
static gint STATE = 0;
gint interval;
LIBLIST * list = NULL;
GAsyncQueue * q = NULL;
pthread_t T;

gboolean create_t = FALSE;
gboolean create_q = FALSE;
gboolean create_l = FALSE;
gint thread_end = 0;

enum STATE
{
  Stop,
  Pause = 0,
  Slow, 
  Play, 
  Speed
};  


gpointer get_random (gpointer data)
{ 
  if(!create_q)
    {
      q = g_async_queue_new ();
      create_q = TRUE; 
    }
   
  while (1)
    {       
      if ((gint)data)
        { break; }  

      if (g_async_queue_length (q) < 1)
        {
          gint rnd = rand() % 100 + 1;
          g_async_queue_push (q, (gpointer)rnd);
        }
    }

  if ((gint)data)  //stop
    {   
      while(g_async_queue_length (q) > 0)
        { g_async_queue_try_pop (q); }

      g_async_queue_unref (q); 
      
    }
  
  pthread_exit(NULL);
 
}



gboolean get_data (GtkButton * button) 
{        
  GtkWidget * drawingarea = lookup_widget(GTK_WIDGET(button),"drawingarea1");   
  GdkDrawable * drawable = drawingarea->window;
  GdkGC * gcf = gdk_gc_new(drawable);
  GdkColor fcolor;
  fcolor.red = 0;
  fcolor.green = 56000;
  fcolor.blue = 0;
  gdk_gc_set_rgb_fg_color(gcf, &fcolor); // set front color
  
  gint width = gdk_window_get_width(drawable);
  gint height = gdk_window_get_height(drawable);
  
  gboolean list_has_next (LIBLIST * list)
  {
    if (list != lib_linklist_next(list)) 
      { return TRUE; }
    else 
      { return FALSE; }
  }

  if (STATE)   // if not stop
    {
      list = lib_linklist_append (list, g_async_queue_pop(q));
      create_l = TRUE; 
      
      gdk_window_clear(drawable);
      gint delta_x = (width/9);
      gint x1,y1,x2,y2;
      LIBLIST * L;
      
      if (lib_linklist_length(list) > 1)
        {        
          while (lib_linklist_length(list) > 10)
            { list = list_remove_last(list); }     
          
          L = lib_linklist_first(list);
          x1 = 0;
          y1 = lib_linklist_data_get(L)*height/100;

          while (list_has_next(L))
            {
              x2 = x1 + delta_x;
              y2 = lib_linklist_data_get(lib_linklist_next(L))*height/100;
         
              printf("\t(%d,%d)->(%d,%d)\n",x1,y1,x2,y2);         
              gdk_draw_line(drawable, gcf, x1, y1, x2, y2);
              
              x1= x2;
              y1 = y2;
              L = lib_linklist_next(L);       
            }
          printf("========================\n");  

        }
      return TRUE;
    }
  else 
    {     
      gdk_window_clear(drawable);      
      return FALSE;
    } 
  
}



//== window1 =======================================================


void
on_window1_destroy                     (GtkObject       *object,
                                        gpointer         user_data)
{  
  
  STATE = 0;
  if (act_id)
    {
      g_source_remove(act_id);
      printf("free timeout\n");
      act_id = 0; 
    }

  if (create_l)
    { 
      lib_linklist_all_free(list); 
      list=NULL;
      create_l = FALSE;
      printf("free linklist\n");
    }    

  if (create_t)
    {  
      thread_end = 1; 
      create_t = FALSE;
      printf("exit pthread\n"); 
    }
  
  gtk_main_quit();
  
}




//== drawing area =====================================================

void
on_drawingarea1_expose_event           (GtkWidget       *widget,
                                        gpointer         user_data)
{
  GdkGC * gcb;
  GdkColor bcolor;
  GdkColormap * colormap;
  GdkDrawable * drawable;
  drawable = widget->window;
  colormap = gdk_rgb_get_colormap();
  gdk_color_parse("black", &bcolor);
  gdk_color_alloc(colormap, &bcolor);
  gdk_window_set_background(drawable, &bcolor);

}


//== buttons ======================================================

void
on_but_play_clicked           (GtkButton       *button,     
                               gpointer         user_data) 
{
  
  srand((unsigned)time(NULL));  
  printf("***PLAY****");  
  
  if (act_id)  
    { g_source_remove(act_id); }
  
  if (STATE == 0 && act_id == 0)
    {
      thread_end = 0;
      pthread_create(&T, NULL, &get_random, (gpointer)thread_end);
      create_t = TRUE;
    }
 
    STATE = Play;
    interval = 900;
    printf("\t interval = %d\n", interval);
    act_id = g_timeout_add(interval, (GSourceFunc )get_data, button);  
}



void
on_but_slow_down_clicked               (GtkButton       *button,   
                                        gpointer         user_data)
{
  printf("***Slow Down****"); 
  if (act_id)
    { g_source_remove(act_id); }

  if (STATE)  
    { 
      srand((unsigned)time(NULL)); 
      if (STATE == 1 && interval < 1700) 
        { interval = interval + 200; }
      else if (STATE != 1) 
        { interval = 1100; }

      printf("\t interval = %d\n", interval);
      STATE = Slow;
      act_id = g_timeout_add(interval, (GSourceFunc )get_data, button);
    }
  else
    { printf("Invalid usage; Use \'Slow down\' only when ECG is running.\n"); }     
  
}


void
on_but_speed_up_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("***Speed Up****"); 
  if (act_id) 
    { g_source_remove(act_id); }
  
  if (STATE)
    {
      srand((unsigned)time(NULL)); 
      if (STATE == 3 && interval > 100) 
        { interval = interval - 200; }
      else if (STATE != 3) 
        { interval = 700;}

      printf("\t interval = %d\n", interval);
      STATE = Speed;
      act_id = g_timeout_add(interval, (GSourceFunc ) get_data, button);
    }
  else   // STATE =0 ->pause or stop
    { printf("Invalid usage; Use \'Speed up\' only when ECG is running.\n"); }
}


void
on_but_pause_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{ 
  printf("***PAUSE****\n");
  
  if (STATE) // play/slow/speed ->pause
    { STATE = Pause; }
  else   //stop->pause 
    { printf("Invalid usage; Use \'Pause\' only when ECG is running.\n"); }

  if (act_id) 
    { g_source_remove(act_id); }
   
}


void
on_but_stop_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget * drawingarea = lookup_widget((GtkWidget *) button,"drawingarea1");
  GdkDrawable * drawable;
  drawable = drawingarea->window;  

  printf("***STOP****\n"); 
  
  gdk_window_clear(drawable);
  printf("clear drawable\n");
 
  STATE = Stop;
  if (act_id) 
    {
      g_source_remove(act_id);
      printf("free timeout\n");
      act_id = 0; 
    }
       
  if (create_l) 
    {
      lib_linklist_all_free(list); 
      list=NULL;
      create_l = FALSE;
      printf("free linklist\n");
    }     
    
  if (create_t)
    { 
      thread_end = 1; 
      create_t = FALSE;
      printf("exit pthread\n"); 
    }

}







