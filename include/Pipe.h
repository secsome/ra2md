#pragma once

#include <Always.h>

class Pipe
{
public:
	explicit Pipe() noexcept;

	CLASS_NOCOPY(Pipe);
	CLASS_CANMOVE(Pipe);

	virtual ~Pipe();

	virtual int Flush();
	virtual int End();
	virtual void Put_To(Pipe* pipe);
	virtual int Put(const void* source, int slen);

	void Put_To(Pipe& pipe) { Put_To(&pipe); }

	Pipe* ChainTo;
	Pipe* ChainFrom;
};