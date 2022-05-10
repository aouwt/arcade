#ifndef GAMES_HPP
	#define GAMES_HPP
	#include <system.hpp>
	#include <mains.hpp>
	
	typedef struct _GameInfo {
		GameFunc main = NULL;
		const char *name = NULL;
		unsigned int tokens = 1;
	} GameInfo;
	
	const GameInfo Games [] = {
		{	.main = &BallGame_Main,
			.name = "Ball game"
		}
	};
	
#endif
