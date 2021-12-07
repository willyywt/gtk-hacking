#include "framework.h"
#include "sap84data.h"

struct MyStatus {
	const char *data_file_name;
	SAP84Data *data;
}global_status;

static void do_drawing(cairo_t *cr) {
	SAP84Data *g_data = global_status.data;
	cairo_set_source_rgb(cr, 0,0,0);
	cairo_set_line_width(cr, 1.2);

	cairo_set_font_size(cr, 14);

	for(size_t i=0; i < g_data->planes.size(); i++) {
		Plane plane = g_data->planes[i];
		Joint jointa = g_data->joints[plane.a - 1];
		Joint jointb = g_data->joints[plane.b - 1];
		Joint jointc = g_data->joints[plane.c - 1];
		Joint jointd = g_data->joints[plane.d - 1];
		Joint_CCL(&jointa, &jointb, &jointc, &jointd);

		Joint offset = { -180.0, -70.0 };
		jointa = Joint_add(jointa, offset);
		jointb = Joint_add(jointb, offset);
		jointc = Joint_add(jointc, offset);
		jointd = Joint_add(jointd, offset);
		double coef = 1.8;
		jointa = Joint_multiply(jointa, coef);
		jointb = Joint_multiply(jointb, coef);
		jointc = Joint_multiply(jointc, coef);
		jointd = Joint_multiply(jointd, coef);

		cairo_move_to(cr, jointa.x, jointa.y);
		cairo_line_to(cr, jointb.x, jointb.y);
		cairo_line_to(cr, jointc.x, jointc.y);
		cairo_line_to(cr, jointd.x, jointd.y);
		cairo_line_to(cr, jointa.x, jointa.y);
		char buf[100];
		cairo_text_extents_t extents;
		Joint center = Joint_average4(jointa, jointb, jointc, jointd);
		g_snprintf(buf, 100, "%ld", i + 1);
		cairo_text_extents(cr, buf, &extents);

		cairo_move_to(cr, center.x - extents.width / 2, center.y + extents.height/2);
		cairo_show_text(cr, buf);  
	}
	cairo_stroke(cr);    
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
	do_drawing(cr);
	return FALSE;
}
static void activate(GtkApplication *app) {
	GtkWidget *window;
	GtkWidget *darea;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	darea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), darea);

	g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
	gtk_window_set_title(GTK_WINDOW(window), "Lines");

	gtk_application_add_window(app, GTK_WINDOW(window));

	gtk_widget_show_all(window);
}

static void shutdown(GtkApplication *app) {
	SAP84_delete(global_status.data);
}

static void print_usage() {
	g_printerr("Usage: hw1 --data <file>\nSee 'hw1 --help' for more options\n");
}

static int
local_options (GApplication *app, GVariantDict *options,
		gpointer      user_data) {
	g_variant_dict_lookup (options, "data", "&s", &global_status.data_file_name);
	if (!global_status.data_file_name) {
		print_usage();
		return 1;
	}
	FILE* file = fopen(global_status.data_file_name, "r");
	if(!file) {
		perror("fopen");
		return 1;
	}
	SAP84Data_error *error = NULL;
	SAP84Data *data = SAP84_read(file, &error);
	if(!data) {
		printf("Error on line %d, char %d: %s\n", error->line_count, error->char_count, error->message); 
		return 1;
	}
	global_status.data = data;
	return -1;
}

int main(int argc, char *argv[]) {
	GtkApplication *app = gtk_application_new (NULL, G_APPLICATION_NON_UNIQUE | G_APPLICATION_FLAGS_NONE);
	g_application_add_main_option (G_APPLICATION (app), "data", 0, G_OPTION_FLAG_NONE, G_OPTION_ARG_STRING, "Path to data file", "./SAP84Data-unix.txt");
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	g_signal_connect (app, "handle-local-options", G_CALLBACK (local_options), NULL);
	g_signal_connect (app, "shutdown", G_CALLBACK (shutdown), NULL);
	return g_application_run (G_APPLICATION (app), argc, argv);
}

