#include <Scenario.h>

ScenarioClass ScenarioClass::Scenario;

ScenarioClass::ScenarioClass() noexcept
	: ScenarioID{ 10000 }
{
}

ScenarioClass::~ScenarioClass()
{
}

int ScenarioClass::GetScenarioID()
{
	return ScenarioID++;
}
