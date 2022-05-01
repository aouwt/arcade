#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_thread.h>
#include <math.h>
#include <pthread.h>

#include "config.h"

#define _(stage, func, ...)	\
	fprintf (stderr, "\t" stage "..."); \
	if (func) {	\
		fprintf (stderr, __VA_ARGS__); \
		fprintf (stderr, " (%i)\n", __LINE__); \
		exit (1); \
	} \
	fprintf (stderr, "Done\n");
#define rnd(range)	((rand () / float (RAND_MAX)) * range)
#define rndf	(rand () / float (RAND_MAX))

#define MASK	0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000


#define CIRC_RADIUS_DEF	(SCR_WIDTH * 0.25)
#define CIRC_RADIUS_MIN	100
#define CIRC_RADIUS_MAX	((SCR_HEIGHT * 0.5) - 100)
#define CENTER_SCREEN	{ SCR_WIDTH / 2, SCR_HEIGHT / 2 }

#define RAD_MAX	(2 * M_PI)

template <typename scalar>
	struct Coord {
		scalar x = 0;
		scalar y = 0;
	};

template <typename scalar>
	struct CCoord {
		scalar d = 0;
		scalar r = 0;
	};

class Sprite {
	public:
		Coord <short> origin;
		SDL_Surface *surface = NULL;
		Coord <short> pos;
		CCoord <float> cpos;
		
		~Sprite (void);
		
		void Place (void);
		void DefaultOrigin (void);
};

class Beam {
	public:
		unsigned short diameter;
		
		Beam (void);
		~Beam (void);
		
		void Draw (void);
		void Frame (void);
		void Restart (void);
		
		bool Collision (void);
	
	private:
		Sprite sprite;
		CCoord <float> vel;
		unsigned int breaktime;
		
		void reinit (void);
};	



class _Score {
	public:
		SDL_Surface *surface = NULL;
		long current = 0;
		long long total = 0;
		long maximum = 0;
		bool changed = true;
		
		_Score (void);
		~_Score (void);
		
		void Inc (unsigned int amt);
		void Dec (unsigned int amt);
		void Reset (void);
};

class _Sound {
	public:
		struct Audio {
			size_t len = 0;
			Uint8 *buf = NULL;
		};
		
		struct {
			Audio Damage;
			Audio Token;
		} Effect;
		
		Audio Current;
		
		_Sound (void);
		~_Sound (void);
		
		void PlaySound (Audio *effect);
		
	private:
		SDL_AudioSpec AudFmt;
		
		Audio mk_sqr (float freq, unsigned char amp, size_t len, Uint8 *buf);
};

class _SDL {
	public:
		SDL_Surface *Screen = NULL;
		SDL_Surface *Bkg = NULL;
		
		_SDL (void);
		~_SDL (void);
		
		void NextFrame (void);
		void PollEvents (void);
		void Frame (void);
		void GameFrame (void);
	
	private:
		SDL_Joystick *Stick;
		TTF_Font *Font;
};

class _Game {
	public:
		struct {
			Sprite s;
			Coord <float> vel;
			Coord <signed short> dir;
			unsigned int flash = 0;
		} Player;
		
		_Game (void);
		~_Game (void);
		
		long Run (void);
		void Demo (void);
		void ProcessEvent (SDL_JoyAxisEvent axis);
		void ProcessEvent (SDL_JoyButtonEvent btn);
		
		pthread_cond_t Thread_Cond = PTHREAD_COND_INITIALIZER;
		pthread_mutex_t Thread_Mutex = PTHREAD_MUTEX_INITIALIZER;
		SDL_Thread *Thread_Thread;
		
		Beam beam [MAX_BEAMS];
		size_t Beams = 1;
		bool DemoOn;
};

extern _Score *Score;
extern _Sound *Sound;
extern _SDL *SDL;
extern _Game *Game;

extern bool Exit;
extern unsigned int Tokens;
