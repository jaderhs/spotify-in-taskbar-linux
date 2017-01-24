#include <glib.h>
#include <string.h>
#include <gdk/gdkx.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1
#include <libwnck/libwnck.h>

GMainLoop *main_loop = NULL;

gint match_spotify_gdk_window(GdkWindow *window, gulong *xid) {

	if( GDK_WINDOW_XID(window) == *xid )
		return 0;
	return -1;
}

void spotify_lookup_and_fix(WnckWindow *window, GList *gdk_window_list) {

	gulong spotify_xid;
	GdkWindow *gdkWindow;
	GList *spotify_window_entry;

	if( wnck_window_has_name(window) == FALSE )
		return;

	if( strcmp(wnck_window_get_name(window), "Spotify") != 0 )
		return;

	spotify_xid = wnck_window_get_xid(window);

	spotify_window_entry = g_list_find_custom(gdk_window_list, &spotify_xid,
										(GCompareFunc)match_spotify_gdk_window);

	if( spotify_window_entry == NULL )
		return;

	gdkWindow = spotify_window_entry->data;
	gdk_window_hide(gdkWindow);
	gdk_flush();
	gdk_window_set_type_hint(gdkWindow, GDK_WINDOW_TYPE_HINT_NORMAL);
	gdk_window_show(gdkWindow);
	gdk_flush();

	if(main_loop != NULL) {
		g_main_loop_quit(main_loop);
		g_main_loop_unref(main_loop);
		main_loop = NULL;
	}
}

gboolean iter_windows(gpointer userdata) {

	GList *gdk_window_stack;
	WnckScreen *screen = wnck_screen_get_default();
	GdkScreen *gdkScreen = gdk_screen_get_default();

	gdk_window_stack = gdk_screen_get_window_stack(gdkScreen);

	if(userdata != NULL)
		wnck_screen_force_update(screen);

	g_list_foreach(wnck_screen_get_windows(screen),
				(GFunc)spotify_lookup_and_fix,
						gdk_window_stack);

	g_list_free_full(gdk_window_stack, g_object_unref);
	return TRUE;
}

int main(int argc, char *argv[]) {

	gdk_init(&argc, &argv);

	while(argc > 1) {
		if(strcmp(argv[--argc], "-w") == 0 && main_loop == NULL) {
			main_loop = g_main_loop_new(NULL, FALSE);
		}
	}

	iter_windows(&argc);
	if(main_loop != NULL) {

		g_timeout_add_seconds(2, iter_windows, NULL);
		g_main_loop_run(main_loop);
	} else {

		iter_windows(&argc);
	}

	wnck_shutdown();

	return 0;
}
