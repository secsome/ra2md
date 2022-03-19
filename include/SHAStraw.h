#pragma once

#include <Always.h>

#include <Straw.h>
#include <SHA.h>

class SHAStraw : public Straw
{
public:
	explicit SHAStraw() noexcept = default;

	CLASS_NOCOPY(SHAStraw);
	CLASS_CANMOVE(SHAStraw);

	virtual ~SHAStraw() override;

	virtual int Get(void* buffer, int length) override;

	int Result(void* result) const;

protected:
	SHAEngine SHA;
};