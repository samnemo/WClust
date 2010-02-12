#ifndef L2DISTANCE_H_
#define L2DISTANCE_H_

#include "EuclideanDistance.h"

namespace damina
{

	/**
 	 *	L2 Distance implementation.
 	 *
 	 * */
	class L2Distance: public damina::EuclideanDistance
	{	
		public:
			virtual double calculateDistance(struct svm_node *, struct svm_node *);
			L2Distance();
			virtual ~L2Distance();
	};
}

#endif /*L2DISTANCE_H_H*/
