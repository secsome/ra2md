#pragma once

#include <Always.h>

class Straw
{
public:
	explicit Straw() noexcept;
	
	CLASS_NOCOPY(Straw);
	CLASS_CANMOVE(Straw);

	virtual ~Straw();
	
	virtual void Get_From(Straw* straw);
	virtual int Get(void* buffer, int length);

	void Get_From(Straw& straw) { Get_From(&straw); }

	Straw* ChainTo;
	Straw* ChainFrom;
};