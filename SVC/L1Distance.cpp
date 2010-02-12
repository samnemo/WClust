#include "stdafx.h"
#include "L1Distance.h"

namespace damina
{
	/**
	 *	Constructor
	 * */
	L1Distance::L1Distance() {
		
	}

	/**
	 *	Destructor
	 * */
	L1Distance::~L1Distance() {
	
	}

	/**
	 * 	Computes the L1 Distance between two points (poits are in libsvm format)
	 * 
	 * 	@param	px	The first point
	 * 	@param	py	The second point
	 * 	@return	The value of the distance between two points 
	 * */
	double L1Distance::calculateDistance(struct svm_node *px, struct svm_node *py) {
			double sum = 0;
			while(px->index != -1 && py->index != -1)
			{
				if(px->index == py->index)
				{
					sum += fabs(px->value - py->value);
					++px;
					++py;
				}
				else
				{
					if(px->index > py->index)
					{
						sum += fabs(py->value);
						++py;
					}
					else
					{
						sum += fabs(px->value);
						++px;
					}
				}
			
			}
			while(px->index != -1)
			{
				sum += fabs(px->value);
				++px;
			}
			while (py->index != -1)
			{
				sum += fabs (py->value);
				++py;
			}
			return sum;
	}

}
