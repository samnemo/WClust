#ifndef KERNELMACHINES_H_
#define KERNELMACHINES_H_

/**
 * 	The KernelMachines header file includes a set of general purpose classes
 *  you can use with various Kernel Machines, including SVMs.
 * 
 * */

/**
 * 	Data Mining Library namespace
 * */
namespace damina
{

	/**
	 * 	Provides encoding for representing Kernel Types.
	 * 
	 * 	This class provides a set of public static attributes each representing a kernel type
	 * 	The integer values encoding the Kernel Types correspond to the values used in 
	 * 	the LibSVM library.
	 * 
	 * */
	class KernelType
	{
		public:
		
			/**
			 * 	Linear Kernel
			 * 	
			 * 	K(x,y) = x' * y
			 * */
			static const int linear = 0;	
			
			/**
			 * 	Polynomial Kernel
			 * 
			 * 	K(x,y) = (gamma * x' * y + coef)^degree
			 * */
			static const int polynomial = 1;
			
			/**
			 * 	Gaussian Kernel (also know as Radial Basis Kernel Function (RBF))
			 * 
			 * 	K(x,y) = exp(-gamma * ||x-y||^2) 
			 * 
			 * 	Here 'gamma' corresponds to 1/sigma^2, where 'sigma' is the variance
			 * */
			static const int gaussian = 2;
			
			/**
			 * 	Sigmoid Kernel
			 * 
			 * 	K(x,y) = tanh(gamma * x' * y + coef)
			 * */
			static const int sigmoid = 3;
	
			/**
			 *	Stump Kernel
			 *	
			 *	K(x,y) = -|x-y|_1 + coef
			 * */
			static const int stump = 4;

			/**
			 *	Perceptron Kernel
			 *
			 *	K(x, Y) = -||x-y||_2 + coef 
			 * */
			static const int perc = 5;

			/**
			 *	Laplacian Kernel
			 *
			 *	K(x,y) = exp(-gamma*|x-y|_1) 
			 * */
			static const int laplacian = 6;

			/**
			 * 	Exponential Kernel 
			 *
			 *	K(x,y) = exp(-gamma*||x-y||)	
			 * */
			static const int expo = 7;
						
			/**
			 * 	Precomputed Kernel
			 * 
			 * 	In LibSVM users may precompute kernel values and input them as 
			 * 	training and testing files.
			 * */
			static const int precomputed = 8;
	};

}

#endif /*KERNELMACHINES_H_*/


