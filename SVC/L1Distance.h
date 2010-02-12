#ifndef L1DISTANCE_H_
#define L1DISTANCE_H_

#include "EuclideanDistance.h"

namespace damina
{
		
	/**
	 *	L1 Distance implementation.
	 * */
	class L1Distance: public damina::EuclideanDistance
	{	
		public:
			virtual double calculateDistance(struct svm_node *, struct svm_node *);
			L1Distance();
			virtual ~L1Distance();
	};
}

#endif /*L1DISTANCE_H_H*/
