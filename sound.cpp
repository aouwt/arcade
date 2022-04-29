#include <SDL.h>

#include "game.hpp"


void callback (void *p, Uint8 *stream, int len) {
	_Sound *_this = (_Sound *) p;
	if (_this -> Current.len == 0)
			return;
		
	for (int i = 0; i < len; i ++) {
		if (_this -> Current.len > 0) {
			_this -> Current.len --;
			_this -> Current.buf ++;
		} else
			break;
		
		stream [i] = *_this -> Current.buf;
	}
}


#define LEN(sec)	(AudFmt.freq * (sec))
_Sound::_Sound (void) {
	SDL_AudioSpec request = {
		.freq = 44100,
		.format = AUDIO_U8,
		.samples = 64,
		.callback = &callback,
		.userdata = (void *) this
	};
	
	_ (SDL_OpenAudio (&request, &AudFmt), "Could not initialize sound!");
	SDL_PauseAudio (0);
	
	// Build sounds
	Effect.Damage = mk_sqr (440, 64, LEN (0.5), NULL);
	Effect.Token = mk_sqr (783.990871963, 127, LEN (0.25), NULL);
}

_Sound::~_Sound (void) {
	SDL_CloseAudio ();
	delete [] Effect.Damage.buf;
	delete [] Effect.Token.buf;
}

void _Sound::PlaySound (_Sound::Audio *effect) {
	Current = *effect;
}

_Sound::Audio _Sound::mk_sqr (float freq, unsigned char amp, size_t len, Uint8 *buf) {
	if (buf == NULL)
		buf = new Uint8 [len];
	
	for (size_t i = 0; i != len; i ++)
		buf [i] = ((i % int (AudFmt.freq / (freq / 2))) > (AudFmt.freq / freq)) * amp;
	
	return {
		.len = len,
		.buf = buf
	};
}
