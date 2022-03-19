#pragma once

#include <Always.h>

#include <Vector.h>
#include <Defines.h>

enum ENUM_ABSTRACT_FLAG
{
	ABSTRACT_FLAG_NONE = 0,
	ABSTRACT_FLAG_OBJECT = 1,
	ABSTRACT_FLAG_TECHNO = 2,
	ABSTRACT_FLAG_FOOT = 4,
};

enum ENUM_RTTI_TYPE
{
	RTTI_TYPE_ABSTRACT = 0,
};

class AbstractClass
{
public:
	static std::vector<AbstractClass*> Array;

	explicit AbstractClass() noexcept;
	
	CLASS_NOCOPY(AbstractClass);
	CLASS_CANMOVE(AbstractClass);

	virtual ~AbstractClass();


private:
	int ID;
	ENUM_RTTI_TYPE RTTIType;

};