#include <glib.h>
#include <string.h>
#include <gdk/gdkx.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1
#include <libwnck/libwnck.h>

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
}

int main(int argc, char *argv[]) {

	WnckScreen *screen;

	gdk_init(&argc, &argv);

	screen = wnck_screen_get_default();
	wnck_screen_force_update(screen);

	g_list_foreach(wnck_screen_get_windows(screen),
					(GFunc)spotify_lookup_and_fix,
					gdk_screen_get_window_stack(gdk_screen_get_default()));

	wnck_shutdown();

	return 0;
}
