#include "arcade.h"

#include <dlfcn.h>
#include <stdlib.h>
#include <stdbool.h>


struct GameContext *Games = NULL;



bool load_game (const char *fn, struct GameContext *game) {
	game -> DlHdl = dlopen (fn, RTLD_NOW | RTLD_LOCAL);
	
	if (game -> DlHdl == NULL) {
		fprintf (stderr, "[ERROR] dlopen: %s\n", dlerror ());
		return true;
	}
	
	game -> Info = (struct GameInfo *) dlsym (game -> DlHdl, "__gameinfo__");
	
	if (game -> Info == NULL) {
		fprintf (stderr, "[ERROR] Missing symbol \"__gameinfo__\"!\n");
		return true;
	}
	
	if (game -> Info -> New == NULL) {
		fprintf (stderr, "[ERROR] No New () function!\n");
		return true;
	}
	
	{	int status = (game -> Info -> New) (game);
		if (status > 0) {
			fprintf (stderr, "[WARN] New () returned non-zero positive exit status: %i\n", status);
		} else
		if (status < 0) {
			fprintf (stderr, "[ERROR] New () returned non-zero negative exit status: %i\n", status);
			return true;
		}
	}
	
	return false;
}





int main (int argc, char **argv) {
	Games = (struct GameContext *) calloc (argc, sizeof (struct GameContext));
	
	
	for (int i = 1; i != argc; i ++) {
		fprintf (stderr, "Loading %s...\n", argv [i]);
		
		if (load_game (argv [i], &Games [i])) {
			fprintf (stderr, "Unloading...\n");
			if (Games [i].DlHdl != NULL)
				dlclose (Games [i].DlHdl);
			Games [i].DlHdl = NULL;
		}
	}
	
	
}
