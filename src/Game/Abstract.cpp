#include <Abstract.h>

#include <Scenario.h>

std::vector<AbstractClass*> AbstractClass::Array;

AbstractClass::AbstractClass() noexcept
	: ID{ ScenarioClass::Scenario.GetScenarioID() }
	, RTTIType{ RTTI_TYPE_ABSTRACT }
{
	Array.push_back(this);
}

AbstractClass::~AbstractClass()
{
	Array.erase(std::find(Array.begin(), Array.end(), this));
}
