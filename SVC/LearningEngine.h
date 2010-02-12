#ifndef LEARNINGENGINE_H_
#define LEARNINGENGINE_H_

#include "DataSet.h"

namespace damina
{
	/**
	 *	A class which provides the basic interface for all learning engines.
	 * 	
	 * 	It acts like an interface (in UML or Java meaning) 
	 * 	for all learning engines we/you need to implement.
	 * 	It just force its children to implement the mandatory methods
	 * 	for a learning engine. 
	 * 	
	 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
	 * */
	class LearningEngine
	{
		
		public:
			virtual void learn() = 0;
			virtual void learn(DataSet *) = 0;
			
			virtual void setTrainingSet(DataSet *) = 0;
			virtual DataSet *getTrainingSet() = 0;
			
			virtual void setTestSet(DataSet *) = 0;
			virtual DataSet *getTestSet() = 0;
			
			LearningEngine();
			virtual ~LearningEngine();
	};

}

#endif /*LEARNINGENGINE_H_*/
