#ifndef	__LOGO_RANDOM_H__
#define	__LOGO_RANDOM_H__

#include "Logo_Precomp.hpp"

namespace LogRec
{
	//used for random chose edges
 
	const unsigned long maxshort = 65535L;
	const unsigned long multiplier = 1194211693L;
	const unsigned long adder = 12345L;

	class RandomNumber
	{
	private:
		unsigned long randSeed;

	public:
		RandomNumber(unsigned long s = 0)
		{
			if (s == 0)
			{
				randSeed = time(0);
			}
			else
			{
				randSeed = s;
			}
		}

		unsigned short Random(unsigned long n)
		{
			randSeed = multiplier * randSeed + adder;
			return (unsigned short)((randSeed >> 16) % n);
		}
		double fRandom(void)
		{
			return Random(maxshort)/double(maxshort);
		}
	};
}

#endif