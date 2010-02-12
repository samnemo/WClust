#include "stdafx.h"
#include "L2Distance.h"
#include <cmath>

namespace damina
{
	/**
	 *	Constructor
	 * */
	L2Distance::L2Distance() {
		
	}

	/**
	 *	Destructor
	 * */
	L2Distance::~L2Distance() {
	
	}

	/**
	 * 	Computes the L2 Distance between two points (poits are in libsvm format)
	 * 
	 * 	@param	px	The first point
	 * 	@param	py	The second point
	 * 	@return	The value of the distance between two points 
	 * */
	double L2Distance::calculateDistance(struct svm_node *x, struct svm_node *y) {
		double sum = 0;
		while(x->index != -1 && y->index !=-1)
		{
			if(x->index == y->index)
			{
				double d = x->value - y->value;
				sum += d*d;
				++x;
				++y;
			}
			else
			{
				if(x->index > y->index)
				{	
					sum += y->value * y->value;
					++y;
				}
				else
				{
					sum += x->value * x->value;
					++x;
				}
			}
		}

		while(x->index != -1)
		{
			sum += x->value * x->value;
			++x;
		}

		while(y->index != -1)
		{
			sum += y->value * y->value;
			++y;
		}
		return (sum > 0.0 ? sum: 0.0);
	}
}
