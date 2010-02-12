#ifndef CLUSTERINGENGINE_H_
#define CLUSTERINGENGINE_H_

#include "LearningEngine.h"

namespace damina
{

	/**
	 *	A class which provides the basic interface for all clustering engines.
	 * 	
	 * 	It acts like an interface (in UML or Java meaning) 
	 * 	for all clustering engines we/you need to implement.
	 * 	It just force its children to implement two overloaded methods 
	 * 	for the clustering, that is the only one mandatory operation
	 * 	for a clustering engine. 
	 * 	
	 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
	 * */
	class ClusteringEngine : public damina::LearningEngine
	{
		public:
		
			virtual void clusterize() = 0;
			virtual void clusterize(DataSet *) = 0;
			
		
			ClusteringEngine();
			virtual ~ClusteringEngine();
	};

}

#endif /*CLUSTERINGENGINE_H_*/
