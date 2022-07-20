#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

GtkWidget *main_window = NULL;

static void draw_page (GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data) {
    printf("DEBUG> draw_page\n");
    std::string* nadpisOdkaz = reinterpret_cast<std::string*>(g_object_get_data(G_OBJECT(data), "nadpisOdkaz"));
    std::vector <std::string>* vektorTextuOdkaz = reinterpret_cast<std::vector <std::string>*>(g_object_get_data(G_OBJECT(data), "vektorTextuOdkaz"));

    std::string teloReportu;
    for (int i = 0; i < vektorTextuOdkaz->size(); i++) {
        teloReportu = teloReportu + vektorTextuOdkaz->at(i).c_str() + "\n";
    }

    PangoFontDescription *desc;
    cairo_t *cr = gtk_print_context_get_cairo_context (context);
    //drawing some image in head
    cairo_surface_t *surf1 = cairo_image_surface_create_from_png("image.png");
    cairo_set_source_surface(cr, surf1, 10, 0);
    cairo_paint(cr);

    //some bigger text in head
    PangoLayout *nadpisLayout = gtk_print_context_create_pango_layout(context);
    pango_layout_set_text (nadpisLayout, nadpisOdkaz->c_str(), -1);
    desc = pango_font_description_from_string ("sans 28");
    pango_layout_set_font_description (nadpisLayout, desc);
    pango_font_description_free (desc);
    //setting position of printing text in head
    cairo_move_to (cr, 150, 30);
    pango_cairo_layout_path (cr, nadpisLayout);
    //filling with black
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_fill (cr);

    //printing rest of text
    PangoLayout* layout =  gtk_print_context_create_pango_layout(context);
    pango_layout_set_text (layout, teloReportu.c_str(), -1);
    desc = pango_font_description_from_string ("sans 18");
    pango_layout_set_font_description (layout, desc);
    pango_font_description_free (desc);
    cairo_move_to (cr, 20, 130);
    pango_cairo_layout_path (cr, layout);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_fill (cr);

    //draw line under head
    cairo_move_to (cr, 10, 120);
    cairo_line_to (cr, 550, 120);
    cairo_stroke (cr);

    g_object_unref (nadpisLayout);
    g_object_unref (layout);
    cairo_surface_destroy(surf1);
}

static GtkPrintSettings *settings = NULL;

static void do_print (GtkWidget* wid, GtkWidget* data) {
    printf("DEBUG> do_print\n");

  GtkPrintOperation *print;
  GtkPrintOperationResult res;

  print = gtk_print_operation_new ();

  if (settings != NULL)
    gtk_print_operation_set_print_settings (print, settings);

    gtk_print_operation_set_n_pages(print, 2);

  g_signal_connect (print, "draw_page", G_CALLBACK (draw_page), data);

  res = gtk_print_operation_run (print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW (main_window), NULL);

  if (res == GTK_PRINT_OPERATION_RESULT_APPLY)
    {
      if (settings != NULL)
        g_object_unref (settings);

      settings = GTK_PRINT_SETTINGS(g_object_ref (gtk_print_operation_get_print_settings (print)));
    }

  g_object_unref (print);
}


static GtkPageSetup *page_setup = NULL;

std::string Int2String (int cislo) {
    std::stringstream ss;
    ss << cislo;
    return ss.str();
}

int main (int argc, char *argv[]) {
    //some data to print
    std::string nadpis = "jmeno kategorie";
    std::vector<std::string> vektorTextu;

    for (int i = 0; i < 50; i++) {
        vektorTextu.push_back(Int2String(i));
    }


  GtkWidget *button = NULL;
  GtkWidget *vbox = NULL;

  // Initialize GTK+
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
  gtk_init (&argc, &argv);
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

  // Create the main window
  main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (main_window), 8);
  gtk_window_set_title (GTK_WINDOW (main_window), "Hello World");
  gtk_window_set_position (GTK_WINDOW (main_window), GTK_WIN_POS_CENTER);
  gtk_widget_realize (main_window);
  g_signal_connect (main_window, "destroy", gtk_main_quit, NULL);

  // Create a vertical box with buttons
  vbox = gtk_vbox_new (TRUE, 6);
  gtk_container_add (GTK_CONTAINER (main_window), vbox);

  button = gtk_button_new_from_stock (GTK_STOCK_DIALOG_INFO);

  g_object_set_data(G_OBJECT (main_window), "nadpisOdkaz", (gpointer)&nadpis);
  g_object_set_data(G_OBJECT (main_window), "vektorTextuOdkaz", (gpointer)&vektorTextu);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (do_print), (gpointer) main_window);
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  g_signal_connect (button, "clicked", gtk_main_quit, NULL);
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

  // Enter the main loop
  gtk_widget_show_all (main_window);
  gtk_main ();

  return 0;
}