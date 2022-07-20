#include <gtk/gtk.h>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <Windows.h>
using namespace std;

gboolean draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data);
static void draw_page (GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data);
static void do_print (GtkWidget* wid, GtkWidget* data);

GtkBuilder *builder = nullptr;
GObject *objeto = nullptr;
GtkPrintSettings *settings = nullptr;

int main( int argc, char *argv[] ){
    gtk_init( &argc, &argv );

    builder = gtk_builder_new_from_file( "impresion.glade" );

    objeto = gtk_builder_get_object( builder, "VentanaPrincipal" );
    gtk_widget_show( GTK_WIDGET( objeto ) );
    g_signal_connect( objeto, "destroy", G_CALLBACK( gtk_main_quit ), nullptr );

    objeto = gtk_builder_get_object( builder, "AreaImpresion" );
    gtk_widget_set_size_request( GTK_WIDGET( objeto ), 400, 600 );
    g_signal_connect( objeto, "draw", G_CALLBACK( draw_callback ), nullptr );

    objeto = gtk_builder_get_object( builder, "ImprimirBoton" );
    g_signal_connect ( objeto, "clicked", G_CALLBACK( do_print ), nullptr );

    gtk_main();

    return 0;
}

string letra = "SOY EL PINCHE PERRO MAS CAPORONGA\nParo todas. Hasta las brgas. Ptm gomez.";

gboolean draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data){
    gdouble width, height;
    GdkRGBA color;
    GtkStyleContext *context;

    GtkWidget *impresion = GTK_WIDGET( gtk_builder_get_object( builder, "AreaImpresion" ) );
    
    // Obtiene las dimensiones del area de dibujo
    height = gtk_widget_get_allocated_height ( impresion );
    width = gtk_widget_get_allocated_width ( impresion );
    

    context = gtk_widget_get_style_context (widget);
    
    gtk_render_background( context, cr, 0, 0, width, height );

    // Dibuja la página
    cairo_set_source_rgba(cr, 255, 255, 255, 255 );
    cairo_rectangle( cr, 0, 0, width, height );
    cairo_fill (cr);

    // Renderiza el texto
    cairo_set_source_rgba( cr, 0, 0, 0, 255 );
    /*cairo_set_font_size( cr, 12 );

    cairo_text_extents_t te;
    cairo_text_extents (cr, "Hola hijo de tu puta madre :)", &te);
    cairo_move_to (cr, 10, 10 );
    cairo_show_text (cr, "Hola hijo de tu puta madre :)" );

    //cairo_arc( cr, 10, 10, 10, 0, 2 * G_PI );
    cairo_fill (cr);*/
    PangoFontDescription *desc;
    PangoLayout* layout = gtk_widget_create_pango_layout(widget, nullptr );
    desc = pango_font_description_from_string( "sans 18" );
    pango_layout_set_font_description( layout, desc );
    pango_font_description_free( desc );
    cairo_move_to (cr, 20, 20);
    pango_cairo_layout_path( cr, layout );
    cairo_arc( cr, 100, 200, 100, 0, 2 * G_PI );
    cairo_fill (cr);
    

    return FALSE;
}

static void do_print(GtkWidget* wid, GtkWidget* data) {
    printf("DEBUG> do_print\n");
    bool loadedFromFile = true;
    GtkPrintOperation *print = nullptr;
    GtkPrintOperationResult res;
    GError *error;

    print = gtk_print_operation_new();
    if( print == nullptr ){
        throw runtime_error( "Ah ocurrido un error al crear la configuración de la impresora." );
    }

    settings = gtk_print_settings_new_from_file( "conf-imp.dat", &error );
    if (settings == NULL){
        loadedFromFile = false;
        settings = gtk_print_settings_new();
        if( settings == nullptr ){
            throw runtime_error( "Ocurrió un error al crear la configuración de impresión." );
        }
        gtk_print_operation_set_print_settings (print, settings);
    }

    gtk_print_operation_set_n_pages(print, 1);

    g_signal_connect (print, "draw_page", G_CALLBACK (draw_page), data);

    double width = gtk_print_settings_get_paper_width( settings, GTK_UNIT_POINTS );
    double height = gtk_print_settings_get_paper_height( settings, GTK_UNIT_POINTS );

    printf( "Ancho: %lf, Alto: %lf\n", width, height );

    if( !loadedFromFile )
        res = gtk_print_operation_run( print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW( gtk_builder_get_object( builder, "VentanaPrincipal" ) ), NULL);
    else
        res = gtk_print_operation_run( print, GTK_PRINT_OPERATION_ACTION_PRINT, GTK_WINDOW( gtk_builder_get_object( builder, "VentanaPrincipal" ) ), NULL);

    if (res == GTK_PRINT_OPERATION_RESULT_APPLY){
        if (settings != NULL){
            gtk_print_settings_to_file( settings, "conf-imp.dat", &error );
            g_object_unref (settings);
        }

        settings = GTK_PRINT_SETTINGS(g_object_ref (gtk_print_operation_get_print_settings (print)));
    }

    g_object_unref (print);
}

static void draw_page (GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data) {
    cout << "publico_imprimir" << endl;
    
    // Ancho y alto de la página
    gdouble ancho, alto;
    GdkRGBA color;
    int a, b, c;

    // Obtiene el contexto de impresión
    cairo_t *cr = gtk_print_context_get_cairo_context( context );
    
    // Obtiene las dimensiones de la página
    ancho = gtk_print_context_get_width( context );
    alto = gtk_print_context_get_height( context );

    printf( "Tamaño: ancho %lf, alto %lf\n", ancho, alto );


    cairo_move_to (cr, 0, 0);
    cairo_set_source_rgb( cr, 0, 0, 0 );

    // Crea el formato pango y establece las opciones generales
    PangoLayout* layout = gtk_print_context_create_pango_layout(context);
    PangoFontDescription *fuente = pango_font_description_from_string( "sans 8" );
    pango_layout_set_font_description( layout, fuente );
    pango_font_description_free( fuente );
    pango_layout_set_single_paragraph_mode( layout, FALSE );
    pango_layout_set_width( layout, ancho * PANGO_SCALE );
    pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
    
    string texto = "Basculas BISA S.A. de C.V.\n"
                   "Acueducto 5065, Artesanos 45598, Tlaquepaque, Jalisco, México\n";

    // 
    pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
    pango_layout_set_text( layout, texto.c_str(), -1);
    pango_layout_get_pixel_size( layout, &a, &b );

    pango_cairo_layout_path( cr, layout );
    cairo_move_to (cr, 0, b );

    pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
    pango_layout_set_text( layout, "Folio: 1", -1);
    pango_cairo_layout_path( cr, layout );
    pango_layout_get_pixel_size( layout, &a, &c );
    cairo_move_to( cr, 0, b );

    pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
    pango_layout_set_text( layout, "Fecha: 11-03-1999\n", -1);
    pango_cairo_layout_path( cr, layout );
    pango_layout_get_pixel_size( layout, &a, &c );
    b = b + c;
    cairo_move_to( cr, 0, b );

    // Derecha
    texto = "Producto: Jícama\n\n"
           "Conductor: Ismael Salas López\n";
    
    pango_layout_set_text( layout, texto.c_str(), -1);
    pango_cairo_layout_path( cr, layout );
    pango_layout_get_pixel_size( layout, &a, &c );
    b = b + c;
    cairo_move_to( cr, 0, b );

    texto = "Peso bruto: \n"
            "Peso tara: \n\n"
            "Peso neto: \n";

    pango_layout_set_text( layout, texto.c_str(), -1);
    pango_cairo_layout_path( cr, layout );
    cairo_move_to( cr, 0, b );

    texto = "1000000 Kg\n"
            "1000000 Kg\n\n"
            "0000000 Kg\n";

    pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
    pango_layout_set_text( layout, texto.c_str(), -1);
    pango_cairo_layout_path( cr, layout );
    b = b + c;
    cairo_move_to( cr, 0, b );

    texto = "\n\n\n_________________________\n"
            "Autoriza\n\n"
            "Realizado con Sistema de Control de Báscula\n";
    pango_layout_set_alignment( layout, PANGO_ALIGN_CENTER );
    pango_layout_set_text( layout, texto.c_str(), -1);
    pango_cairo_layout_path( cr, layout );
    b = b + c;
    cairo_move_to( cr, 0, b );

    //
    cairo_fill (cr);
}