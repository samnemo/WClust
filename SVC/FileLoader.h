#ifndef FILELOADER_H_
#define FILELOADER_H_

#include "DataSet.h"

namespace damina
{

	/**
	 * 	Implements the file loader.
	 * 
	 *	This class loads data from file and returns them encapsulated in the
	 * 	DataSet class, ready to be managed from the classifiers, regressors, etc.
	 *  
	 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
	 * 
	 * */
	class FileLoader
	{
		
		public:
		
			//TODO temporary pure virtual - no time to implement
			
			virtual DataSet *load() = 0;
			virtual DataSet *load(char *) = 0;
			
			FileLoader();
			virtual ~FileLoader();
	};

}

#endif /*FILELOADER_H_*/
