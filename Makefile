all:
	gcc -o spotifyitl spotifyitl.c `pkg-config --libs --cflags libwnck-3.0`
