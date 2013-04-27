#pragma once

#include "PreDec.h"
#include "GameState.h"

class StartMenu :public MocapGE::GameState
{
public:
	StartMenu(void);
	~StartMenu(void);

	virtual void Update();

};

