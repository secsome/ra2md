#pragma once

#include <Always.h>

#include <MP.h>
#include <Straw.h>

template<int PRECISION>
class Int 
{
public:
	explicit Int() noexcept { XMP_Init(&reg[0], 0, PRECISION); }
	explicit Int(unsigned int value) noexcept { XMP_Init(&reg[0], value, PRECISION); }

	Int& operator=(unsigned int value) { XMP_Init(&reg[0], value, PRECISION); }

	void Randomize(Straw& rng, int bitcount) { XMP_Randomize(&reg[0], rng, bitcount, PRECISION); }
	void Randomize(Straw& rng, const Int& minval, const Int& maxval) { XMP_Randomize(&reg[0], rng, minval, maxval, PRECISION); reg[0] |= 1; }

	operator digit* () { return &reg[0]; }
	operator const digit* () const { return &reg[0]; }

	bool operator[](unsigned bit) const { return(XMP_Test_Bit(&reg[0], bit)); }

	Int& operator++() { XMP_Inc(&reg[0], PRECISION); return(*this); }
	Int& operator--() { XMP_Dec(&reg[0], PRECISION); return(*this); }
	bool operator!() const { return(XMP_Test_Eq_Int(&reg[0], 0, PRECISION)); }
	Int operator~() { XMP_Not(&reg[0], PRECISION); return(*this); }
	Int operator-() const { Int a = *this; a.Negate(); return (a); }

	int ByteCount() const { return(XMP_Count_Bytes(&reg[0], PRECISION)); }
	int BitCount() const { return(XMP_Count_Bits(&reg[0], PRECISION)); }
	bool Is_Negative() const { return(XMP_Is_Negative(&reg[0], PRECISION)); }
	unsigned MaxBitPrecision() const { return PRECISION * (sizeof(unsigned int) * CHAR_BIT); }
	bool IsSmallPrime() const { return(XMP_Is_Small_Prime(&reg[0], PRECISION)); }
	bool SmallDivisorsTest() const { return(XMP_Small_Divisors_Test(&reg[0], PRECISION)); }
	bool FermatTest(unsigned rounds) const { return(XMP_Fermat_Test(&reg[0], rounds, PRECISION)); }
	bool IsPrime() const { return(XMP_Is_Prime(&reg[0], PRECISION)); }
	bool RabinMillerTest(Straw& rng, unsigned int rounds) const { return(XMP_Rabin_Miller_Test(rng, &reg[0], rounds, PRECISION)); }

	Int& operator+=(const Int& number) { Carry = XMP_Add(&reg[0], &reg[0], number, 0, PRECISION); return(*this); }
	Int& operator-=(const Int& number) { Borrow = XMP_Sub(&reg[0], &reg[0], number, 0, PRECISION); return(*this); }
	Int& operator*=(const Int& multiplier) { Remainder = *this; Error = XMP_Signed_Mult(&reg[0], Remainder, multiplier, PRECISION); return(*this); }
	Int& operator/=(const Int& t) { *this = (*this) / t; return *this; }
	Int& operator%=(const Int& t) { *this = (*this) % t; return *this; }
	Int& operator<<=(int bits) { XMP_Shift_Left_Bits(&reg[0], bits, PRECISION); return *this; }
	Int& operator>>=(int bits) { XMP_Shift_Right_Bits(&reg[0], bits, PRECISION); return *this; }

	Int operator+(const Int& number) const { Int term; Carry = XMP_Add(term, &reg[0], number, 0, PRECISION); return(term); }
	Int operator+(unsigned short b) const { Int result; Carry = XMP_Add_Int(result, &reg[0], b, 0, PRECISION); return(result); }
	Int operator-(const Int& number) const { Int term; Borrow = XMP_Sub(term, &reg[0], number, 0, PRECISION); return(term); }
	Int operator-(unsigned short b) const { Int result; Borrow = XMP_Sub_Int(result, &reg[0], b, 0, PRECISION); return(result); }
	Int operator*(const Int& multiplier) const { Int result; Error = XMP_Signed_Mult(result, &reg[0], multiplier, PRECISION); return result; }
	Int operator*(unsigned short b) const { Int result; Error = XMP_Unsigned_Mult_Int(result, &reg[0], b, PRECISION); return(result); }
	Int operator/(const Int& divisor) const { Int quotient = *this; XMP_Signed_Div(Remainder, quotient, &reg[0], divisor, PRECISION); return (quotient); }
	Int operator/(unsigned int b) const { return(*this / Int<PRECISION>(b)); }
	Int operator/(unsigned short divisor) const { Int quotient; Error = XMP_Unsigned_Div_Int(quotient, &reg[0], divisor, PRECISION); return(quotient); }
	Int operator%(const Int& divisor) const { Int remainder; XMP_Signed_Div(remainder, Remainder, &reg[0], divisor, PRECISION); return(remainder); }
	Int operator%(unsigned int b) const { return(*this % Int<PRECISION>(b)); }
	unsigned short operator%(unsigned short divisor) const { return(XMP_Unsigned_Div_Int(Remainder, &reg[0], divisor, PRECISION)); }

	Int operator>>(int bits) const { Int result = *this; XMP_Shift_Right_Bits(result, bits, PRECISION); return result; }
	Int operator<<(int bits) const { Int result = *this; XMP_Shift_Left_Bits(result, bits, PRECISION); return result; }

	int operator==(const Int& b) const { return (memcmp(&reg[0], &b.reg[0], (MAX_BIT_PRECISION / CHAR_BIT)) == 0); }
	int operator!=(const Int& b) const { return !(*this == b); }
	int operator>(const Int& number) const { return(XMP_Compare(&reg[0], number, PRECISION) > 0); }
	int operator>=(const Int& number) const { return(XMP_Compare(&reg[0], number, PRECISION) >= 0); }
	int operator<(const Int& number) const { return(XMP_Compare(&reg[0], number, PRECISION) < 0); }
	int operator<=(const Int& number) const { return(XMP_Compare(&reg[0], number, PRECISION) <= 0); }

	void Negate() { XMP_Neg(&reg[0], PRECISION); }
	Int Abs() { XMP_Abs(&reg[0], PRECISION); return(*this); }
	Int times_b_mod_c(Int const& multiplier, Int const& modulus) const {
		Int result;
		Error = xmp_stage_modulus(modulus, PRECISION);
		Error = XMP_Mod_Mult(result, &reg[0], multiplier, PRECISION);
		XMP_Mod_Mult_Clear(PRECISION);
		return result;
	}

	Int exp_b_mod_c(const Int& e, const Int& m) const {
		Int result;
		Error = xmp_exponent_mod(result, &reg[0], e, m, PRECISION);
		return result;
	}

	static Int Unsigned_Mult(Int const& multiplicand, Int const& multiplier) { Int product; Error = XMP_Unsigned_Mult(&product.reg[0], &multiplicand.reg[0], &multiplier.reg[0], PRECISION); return(product); }
	static void Unsigned_Divide(Int& remainder, Int& quotient, const Int& dividend, const Int& divisor) { Error = XMP_Unsigned_Div(remainder, quotient, dividend, divisor, PRECISION); }
	static void Signed_Divide(Int& remainder, Int& quotient, const Int& dividend, const Int& divisor) { XMP_Signed_Div(remainder, quotient, dividend, divisor, PRECISION); }
	Int Inverse(const Int& modulus) const { Int result; XMP_Inverse_A_Mod_B(result, &reg[0], modulus, PRECISION); return(result); }

	static Int Decode_ASCII(char const* string) { Int result; XMP_Decode_ASCII(string, result, PRECISION); return(result); }

	int Encode(unsigned char* output) const { return(XMP_Encode(output, &reg[0], PRECISION)); }
	int Encode(unsigned char* output, unsigned length) const { return(XMP_Encode(output, length, &reg[0], PRECISION)); }
	void Signed_Decode(const unsigned char* from, int frombytes) { XMP_Signed_Decode(&reg[0], from, frombytes, PRECISION); }
	void Unsigned_Decode(const unsigned char* from, int frombytes) { XMP_Unsigned_Decode(&reg[0], from, frombytes, PRECISION); }

	int DEREncode(unsigned char* output) const { return(XMP_DER_Encode(&reg[0], output, PRECISION)); }
	void DERDecode(const unsigned char* input) { XMP_DER_Decode(&reg[0], input, PRECISION); }

	friend Int<PRECISION> Generate_Prime(Straw& rng, int pbits, Int<PRECISION> const* = 0);
	friend Int<PRECISION> Gcd(const Int<PRECISION>& a, const Int<PRECISION>& b);
	static int Error;

	static bool Carry;
	static bool Borrow;
	static Int Remainder;

private:
	digit reg[PRECISION];

	struct RemainderTable
	{
		RemainderTable(const Int<PRECISION>& p) : HasZeroEntry(false)
		{
			for (unsigned i = 0; i < ARRAY_SIZE(MPPrimesHolder::primeTable); i++) {
				table[i] = p % MPPrimesHolder::primeTable[i];
			}
		}
		bool HasZero() const { return(HasZeroEntry); }
		void Increment(unsigned short increment = 1)
		{
			HasZeroEntry = false;
			for (unsigned int i = 0; i < ARRAY_SIZE(MPPrimesHolder::primeTable); i++) {
				table[i] += increment;
				while (table[i] >= MPPrimesHolder::primeTable[i]) {
					table[i] -= MPPrimesHolder::primeTable[i];
				}
				HasZeroEntry = (HasZeroEntry || !table[i]);
			}
		}
		void Increment(const RemainderTable& rtQ)
		{
			HasZeroEntry = false;
			for (unsigned int i = 0; i < ARRAY_SIZE(MPPrimesHolder::primeTable); i++) {
				table[i] += rtQ.table[i];
				if (table[i] >= MPPrimesHolder::primeTable[i]) {
					table[i] -= MPPrimesHolder::primeTable[i];
				}
				HasZeroEntry = (HasZeroEntry || !table[i]);
			}
		}

		bool HasZeroEntry;
		unsigned short table[ARRAY_SIZE(MPPrimesHolder::primeTable)];
	};

};


template<class T>
T Gcd(const T& a, const T& n)
{
	T g[3] = { n, a, 0UL };

	unsigned int i = 1;
	while (!!g[i % 3]) {
		g[(i + 1) % 3] = g[(i - 1) % 3] % g[i % 3];
		i++;
	}
	return g[(i - 1) % 3];
}

template<class T>
T Generate_Prime(Straw& rng, int pbits, T const*)
{
	T minQ = (T(1UL) << (unsigned short)(pbits - (unsigned short)2));
	T maxQ = ((T(1UL) << (unsigned short)(pbits - (unsigned short)1)) - (unsigned short)1);

	T q;
	T p;

	do {
		q.Randomize(rng, minQ, maxQ);
		p = (q * 2) + (unsigned short)1;

		typename T::RemainderTable rtQ(q);
		typename T::RemainderTable rtP(p);

		while (rtQ.HasZero() || rtP.HasZero() || !q.IsPrime() || !p.IsPrime()) {
			q += 2;
			p += 4;
			if (q > maxQ) break;

			rtQ.Increment(2);
			rtP.Increment(4);
		}
	} while (q > maxQ);

	return(p);
}

typedef Int<MAX_UNIT_PRECISION>	bignum;
typedef Int<MAX_UNIT_PRECISION>	BigInt;