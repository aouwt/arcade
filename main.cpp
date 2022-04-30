#include "global.hpp"

_SDL *SDL = NULL;
_Sound *Sound = NULL;
_Game *Game = NULL;
_Score *Score = NULL;

bool Exit = false;
unsigned int Tokens = 0;

int main (void) {
	Game = new _Game;
	SDL = new _SDL;
	Sound = new _Sound;
	Score = new _Score;
	
	while (Exit == false) {
		Game -> Demo ();
		Score -> Reset ();
		Game -> Run ();
	}
	delete Sound;
	delete SDL;
	delete Game;
	delete Score;
}
