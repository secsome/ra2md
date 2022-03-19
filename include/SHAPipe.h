#pragma once

#include <Always.h>

#include <Pipe.h>
#include <SHA.h>

class SHAPipe : public Pipe
{
public:
	explicit SHAPipe() noexcept;

	CLASS_NOCOPY(SHAPipe);
	CLASS_CANMOVE(SHAPipe);

	virtual ~SHAPipe() override;

	virtual int Put(const void* source, int slen) override;

	int Result(void* result) const;

protected:
	SHAEngine SHA;
};