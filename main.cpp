#include "global.hpp"

_SDL *SDL = NULL;
_Sound *Sound = NULL;
_Game *Game = NULL;
_Score *Score = NULL;

int main (void) {
	Game = new _Game;
	SDL = new _SDL;
	Sound = new _Sound;
	Score = new _Score;
	
	for (;;) {
		while (Game -> Tokens == 0) {
			SDL -> Frame ();
			if (SDL -> Exit == true)
				goto end;
		}
		Game -> Run ();
	}
end:
	delete Sound;
	delete SDL;
	delete Game;
	delete Score;
}
