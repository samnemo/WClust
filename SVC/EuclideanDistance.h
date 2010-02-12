#ifndef EUCLIDEANDISTANCE_H_
#define EUCLIDEANDISTANCE_H_

#include "svm.h"
#include <cmath>

namespace damina
{
	/**
 	 *	The EuclideanDistance class is a base class to implement euclidean norms (L2, L1, ...)
 	 *	This version use basic structures of LIBSVM to manipulate vectors.
 	 * */
	class EuclideanDistance 
	{	
		public:
			virtual double calculateDistance(struct svm_node *, struct svm_node *) = 0;	
			EuclideanDistance();
			virtual ~EuclideanDistance();
	};
}

#endif /*EUCLIDEANDISTANCE_H_H*/
