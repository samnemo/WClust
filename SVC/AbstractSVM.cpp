#include "stdafx.h"
#include "AbstractSVM.h"

namespace damina
{

	/**
	 * 	Constructor
	 * */
	AbstractSVM::AbstractSVM() {
		testSet = NULL;		
	}
	
	/**
	 * 	Destructor
	 * */
	AbstractSVM::~AbstractSVM() {
		delete trainingSet;
		if (testSet != NULL) delete testSet;
	}


	/**
	 * 	Set the kernel type for the SVM.
	 * 	The values representing the Kernel Types are available in the 
	 * 	KernelType class as public static attributes.
	 * 
	 * 	@see KernelType
	 * 	
	 * 	@param	k	The integer value representing the Kernel Type
	 * */
	void AbstractSVM::setKernel(int k) {
		this->params->kernel_type = k;
	}
	
	/**
	 * 	Set both the kernel type and the kernel width (gamma) for the SVM.
	 * 	The values representing the Kernel Types are available in the 
	 * 	KernelType class as public static attributes.
	 * 	The gamma (width) parameter is ignored in case of Linear Kernel.
	 * 
	 * 	@see KernelType
	 * 
	 * 	@param	k	The integer value representing the Kernel Type
	 * 	@param	width	The kernel gamma, a real value in [0,1]
	 * */
	void AbstractSVM::setKernel(int k, double width) {
		this->params->kernel_type = k;
		this->params->gamma = width;
	}
	
	/**
	 *	Set both the kernel type and the kernel degree for the SVM.
	 * 	The values representing the Kernel Types are available in the 
	 * 	KernelType class as public static attributes.
	 * 	The degree is only for the Polynomial Kernel. If you set a different type
	 * 	of Kernel with this method, the degree will be simply ignored.
	 * 
	 * 	@see KernelType
	 * 
	 * 	@param	k	The integer value representing the Kernel Type
	 * 	@param	degree	An integer value representing the degree in the Polynomial Kernel
	 * */
	void AbstractSVM::setKernel(int k, int degree) {
		this->params->kernel_type = k;
		this->params->degree = degree;
	}
	
	
	/**
	 *	Set the kernel type, the kernel degree and the kernel width for the SVM.
	 * 	The values representing the Kernel Types are available in the 
	 * 	KernelType class as public static attributes.
	 * 	The degree is only for the Polynomial Kernel. If you set a different type
	 * 	of Kernel with this method, the degree will be simply ignored.
	 * 	The gamma (width) parameter is ignored in case of Linear Kernel.
	 * 
	 * 	@see KernelType
	 * 	
	 * 	@param	k	The integer value representing the Kernel Type
	 * 	@param	degree	An integer value representing the degree in the Polynomial Kernel
	 * 	@param	width	The kernel gamma, a real value in [0,1]
	 * */
	void AbstractSVM::setKernel(int k, int degree, double width) {
		this->params->kernel_type = k;
		this->params->degree = degree;
		this->params->gamma = width;
	}
	
	/**
	 * 	Set the kernel width (gamma) for the SVM.
	 * 	The gamma (width) parameter is ignored in case of Linear Kernel.
	 * 
	 * 	@param	w	The kernel gamma, a real value in [0,1]
	 * */
	void AbstractSVM::setKernelWidth(double w) {
		this->params->gamma = w;
	}
	
	/**
	 *	Set the kernel degree for the SVM.
	 * 	The degree is only for the Polynomial Kernel. If you set a different type
	 * 	of Kernel, the degree will be simply ignored.
	 * 
	 * 	@param	deg	An integer value representing the degree in the Polynomial Kernel
	 * */
	void AbstractSVM::setKernelDegree(int deg) {
		this->params->degree = deg;
	}
	
	
	/**
	 * 	Set the tolerance of termination criterion.
	 * 
	 * 	Pratically, the training of an SVM is always sub-optimal and 
	 * 	a stop criterion is used to choose when the training algorithm will stop.
	 * 
	 * 	This method set the tolerance of the criterion used in the LibSVM.
	 * 
	 * 	@param	t	A real-valued parameter representing the tolerance 
	 * 				of the termination criterion. Usually it assumes values 
	 * 				between 0.001 and 0.00001
	 * */
	void AbstractSVM::setTolerance(double t) {
		this->params->eps = t;
	}
	
	/**
	 * 	Set the kernel cache size.
	 * 
	 * 	The kernel cache is used to store the kernel values already computed
	 * 
	 * 	@param	sizeMB	The cache size im Megabytes 
	 * */
	void AbstractSVM::setCacheSize(int sizeMB) {
		this->params->cache_size = sizeMB;
	}
	
	/**
	 *	Set the test set
	 * 
	 *	@param	test	The test set 
	 * */
	void AbstractSVM::setTestSet(DataSet *test) {
		this->testSet = test;
	}
	
	/**
	 * 	Set the training set
	 * 
	 * 	@param	train	The training set
	 * */
	void AbstractSVM::setTrainingSet(DataSet *train) {
		this->trainingSet = train;
	}
	
	/**
	 * 	Returns the current test set
	 * 
	 * 	@return The test set
	 * */
	DataSet *AbstractSVM::getTestSet() {
		return this->testSet;
	}
	
	
	/**
	 * 	Returns the current training set
	 * 
	 * 	@return The training set
	 * */
	DataSet *AbstractSVM::getTrainingSet() {
		return this->trainingSet;
	}
	
	
	/**
	 * 	Returns the current kernel type
	 * 
	 * 	@return	The kernel type
	 * */
	int AbstractSVM::getKernel() {
		return this->params->kernel_type;
	}
	
	
	/**
	 * 	Returns the degree of the current kernel
	 * 
	 * 	@return The kernel degree
	 * */
	int AbstractSVM::getKernelDegree() {
		return this->params->degree;
	}
	
	/**
	 * 	Returns the gamma of the current kernel
	 * 
	 * 	@return The gamma of the kernel 
	 **/
	double AbstractSVM::getKernelWidth() {
		return this->params->gamma;
	}
	
	
	/**
	 * 	Returns	the tolerance of the stopping criteria
	 * 
	 * 	@returns	The value of the tolerance parameter for stopping criteria
	 * */
	double AbstractSVM::getTolerance() {
		return this->params->eps;
	}
	
	/**
	 * 	Returns the current kernel cache size
	 * 
	 * 	@return The size, in MBytes, of the current kernel cache 
	 * */
	double AbstractSVM::getCacheSize() {
		return this->params->cache_size;
	}
	
	
	/**
	 * 	Enable the Shrinking Heuristics.
	 * 
	 * 	Since for many problems the number of free support vectors is small, 
	 *	the shrinking technique reduces the size of the working problem without considering 
	 *	some bounded variables.
	 * 	
	 * 	References: <em>T. Joachims, "Making large-scale support vector machine learning practical," , 
	 * 	Schšlkopf, B., Burges, C., and Smola, A., Eds., MIT Press, Cambridge, MA, 1998.</em>
	 * 
	 * */
	void AbstractSVM::enableShrinkingHeuristics() {
		this->params->shrinking = 1;
	}
	
	
	/**
	 * 	Disable the Shrinking Heuristics.
	 * 
	 * 	Since for many problems the number of free support vectors is small, 
	 *	the shrinking technique reduces the size of the working problem without considering 
	 *	some bounded variables.
	 * 	
	 * 	References: <em>T. Joachims, "Making large-scale support vector machine learning practical," , 
	 * 	Schšlkopf, B., Burges, C., and Smola, A., Eds., MIT Press, Cambridge, MA, 1998.</em>
	 * 
	 * */
	void AbstractSVM::disableShrinkingHeuristics() {
		this->params->shrinking = 0;
	}
	
	/**
	 * 	Returns true if the Shrinking is enabled. False otherwise.
	 * 
	 * 	Since for many problems the number of free support vectors is small, 
	 *	the shrinking technique reduces the size of the working problem without considering 
	 *	some bounded variables.
	 * 	
	 * 	References: <em>T. Joachims, "Making large-scale support vector machine learning practical," , 
	 * 	Schšlkopf, B., Burges, C., and Smola, A., Eds., MIT Press, Cambridge, MA, 1998.</em>
	 * 
	 * 	@return	True if the Shrinking is enabled. False otherwise.
	 * */
	bool AbstractSVM::isShrinkingHeuristicsEnabled() {
		if (this->params->shrinking)
			return true;
			
		return false;
	}

	/**
	 *	Returns the SVM paramters
     * 
     *  @return The whole set of SVM parameters
	 * */
	struct svm_parameter *AbstractSVM::getParameters() {
		return params;
	}
}
