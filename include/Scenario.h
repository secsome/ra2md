#pragma once

#include <Always.h>

class ScenarioClass
{
public:
	static ScenarioClass Scenario;

	explicit ScenarioClass() noexcept;

	CLASS_NOCOPY(ScenarioClass);
	CLASS_CANMOVE(ScenarioClass);

	virtual ~ScenarioClass();

	int GetScenarioID();

private:
	int ScenarioID;
};