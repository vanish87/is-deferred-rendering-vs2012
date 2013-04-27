#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_

#pragma once
#include <vector>
#include "PreDec.h"
#include "TypeAndFormat.h"

#include "GameState.h"

namespace MocapGE
{
	class StateManager
	{
	public:
		StateManager(void);
		~StateManager(void);

		void Update();
		void ChangeState(GameState* game_state, StateOP op);
		GameState* CurrentState();

	private:
		std::vector<GameState*> states_;
		GameState* current_state_;

	};

}
#endif
