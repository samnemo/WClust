#include "stdafx.h"
#include "OneClassSVM.h"
#include <cstdio>

namespace damina
{

	/**
	 * 	Constructor
	 * 	
	 * */
	OneClassSVM::OneClassSVM() {
		this->params = (struct svm_parameter *)malloc(sizeof(struct svm_parameter));
		this->params->svm_type = ONE_CLASS;
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	The training set
	 * */
	OneClassSVM::OneClassSVM(DataSet *train) {
		this->params = (struct svm_parameter *)malloc(sizeof(struct svm_parameter));
		this->params->svm_type = ONE_CLASS;
		this->trainingSetToProblem();
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	The problem in libsvm format
	 * */
	OneClassSVM::OneClassSVM(struct svm_problem *prob) {
		this->params = (struct svm_parameter *)malloc(sizeof(struct svm_parameter));
		this->params->svm_type = ONE_CLASS;
		this->problem = prob;
	}
	
	/**
	 * 	Destructor
	 * */
	OneClassSVM::~OneClassSVM() {
		free(this->params);
		free(this->problem);
		free(this->model);
	}


	/**
	 * 	Set the nu parameter for the One Class SVM.
	 * 	
	 * 	The parameter 'nu' in (0,1] in an upper bound on the fraction of training
	 * 	errors and a lower bound of the fraction of support vectors.
	 * 
	 * 
	 * 	@param	nu	The nu parameter for the OneClass SVM
	 * */
	void OneClassSVM::setNU(double nu) {
		this->params->nu = nu;
	}
	
	/**
	 * 	Returns the current values for 'nu' parameter
	 * 
	 * 	The parameter 'nu' in (0,1] in an upper bound on the fraction of training
	 * 	errors and a lower bound of the fraction of support vectors.
	 * 
	 * 
	 * 	@return	 The current values for 'nu' parameter
	 * */
	double OneClassSVM::getNU() {
		return this->params->nu;
	}
	
	
	/**
	 * 	Perform the learning process.
	 * 	
	 * */
	void OneClassSVM::learn() {
		this->model = svm_train(this->problem, this->params);
	}	
	
	/**
	 * 	Perform the learning process.
	 * 
	 * 	@param	ts	The new training set to use in learning process
	 * */
	void OneClassSVM::learn(DataSet *ts) {
		this->setTrainingSet(ts);
		this->trainingSetToProblem();
		this->learn();
	}
	
	/**
	 * 	Perform the learning process.
	 * 
	 * 	@param	p	The new problem in libsvm format to use in learning process
	 * */
	void OneClassSVM::learn(struct svm_problem *p) {
		this->setProblem(p);
		this->learn();
	}
	
	/**
	 * 	Set the new problem in libsvm format for the learning process
	 * 
	 * 	@param	p	The new problem in libsvm format to use in learning process
	 * */
	void OneClassSVM::setProblem(struct svm_problem *p) {
		this->problem = p;
	}	
	
	
	/**
	 * 	Returns the problem in libsvm format
	 * 
	 * 	@return The problem in LibSVM format
	 * */
	struct svm_problem *OneClassSVM::getProblem() {
		return this->problem;
	}
	
	
	/**
	 *	Set the new training set 
	 * 	
	 * 	@param	ts 	The training set to set
	 * */
	void OneClassSVM::setTrainingSet(DataSet *ts) {
		AbstractSVM::setTrainingSet(ts);
		this->trainingSetToProblem();
	}
	
	
	/**
	 * 	The classification process
	 * 
	 * */
	void OneClassSVM::classify() {
		//	TODO to implement
		// it acts on test set
	}
	
	/**
	 * 	The classification process
	 * 	
	 * 	@param	test	The test set to set before run the classification process
	 * */
	void OneClassSVM::classify(DataSet *test) {
		// TODO to implement
		// it acts on test set
	}


	/**
	 * 	Returns the trained model
	 * 
	 * 	@reteurn The trained model, with objective function, lagrangians, etc.
	 * */
	struct svm_model *OneClassSVM::getModel() {
		return this->model;
	}
	
	
	// private methods
	
	
	/**
	 * 	Convert a training set format to svm_problem libsvm format
	 * 
	 * */
	void OneClassSVM::trainingSetToProblem() {
		DataSet *ts = this->getTrainingSet();
		Point *temp;
		
		unsigned long int i, j;
					
		this->problem->l = ts->getSize();
		this->problem->y = (double *) malloc(this->problem->l * sizeof(double));
		this->problem->x = (struct svm_node **) malloc(this->problem->l * sizeof(struct svm_node*));
		
		// iteration on Points
		for (i = 0; i < ts->getSize(); i++) {
			temp = ts->get(i);
			
			this->problem->x[i] = (struct svm_node*) malloc((temp->getSize() + 1) * sizeof(struct svm_node));
			this->problem->y[i] = temp->getLabel();
			
			//iteration on Features
			for (j = 0; j < temp->getSize(); j++) {
				this->problem->x[i][j].index = temp->get(j)->getIndex();
				this->problem->x[i][j].value = temp->get(j)->getValue();  				
			}
			
			this->problem->x[i][j].index = -1;
			this->problem->x[i][j].value = 0;
		}
	}
}
