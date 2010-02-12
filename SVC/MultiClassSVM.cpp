#include "stdafx.h"
#include "MultiClassSVM.h"
#include "KernelMachines.h"
#include <cstdio>
#include <iostream>

namespace damina
{

	/**
	 * 	Constructor
	 * 	
	 * */
	MultiClassSVM::MultiClassSVM() {
		this->params = (struct svm_parameter *)malloc(sizeof(struct svm_parameter));
		this->params->svm_type = C_SVC;
		this->params->eps = 0.001;
		this->params->cache_size = 100;
		this->params->C = 1;
		this->params->kernel_type = LINEAR;
		this->params->shrinking = 1;
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	The training set
	 * */
	MultiClassSVM::MultiClassSVM(DataSet *train) {
		this->params = (struct svm_parameter *)malloc(sizeof(struct svm_parameter));
		this->params->svm_type = C_SVC;
		this->params->eps = 0.001;
		this->params->cache_size = 100;
		this->params->C = 1;
		this->params->kernel_type = LINEAR;
		this->params->shrinking = 1;
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	The problem in libsvm format
	 * */
	MultiClassSVM::MultiClassSVM(struct svm_problem *prob) {
		this->params = (struct svm_parameter *)malloc(sizeof(struct svm_parameter));
		this->params->svm_type = C_SVC;
		this->params->eps = 0.001;
		this->params->cache_size = 100;
		this->params->C = 1;
		this->params->kernel_type = LINEAR;
		this->params->shrinking = 1;
		this->problem = prob;
	}
	
	/**
	 * 	Destructor
	 * */
	MultiClassSVM::~MultiClassSVM() {
		free(this->params);
		free(this->problem);
		free(this->model);
	}


	/**
	 * 	Set the C soft-margin parameter for the SVM classifier
	 * 	
	 * 	@param	C	The soft-margin parameter
	 * */
	void MultiClassSVM::setC(double C) {
		this->params->C = C;
	}
	
	/**
	 * 	Returns the current values for soft-margin C parameter
	 * 
	 * 	@return	 The current values for 'C' parameter
	 * */
	double MultiClassSVM::getC() {
		return this->params->C;
	}
	
	
	/**
	 * 	Perform the learning process.
	 * 	
	 * */
	void MultiClassSVM::learn() {
		this->model = svm_train(this->problem, this->params);
	}	
	
	/**
	 * 	Perform the learning process.
	 * 
	 * 	@param	ts	The new training set to use in learning process
	 * */
	void MultiClassSVM::learn(DataSet *ts) {
		this->setTrainingSet(ts);
		this->trainingSetToProblem();
		this->learn();
	}
	
	/**
	 * 	Perform the learning process.
	 * 
	 * 	@param	p	The new problem in libsvm format to use in learning process
	 * */
	void MultiClassSVM::learn(struct svm_problem *p) {
		this->setProblem(p);
		this->learn();
	}
	
	/**
	 * 	Set the new problem in libsvm format for the learning process
	 * 
	 * 	@param	p	The new problem in libsvm format to use in learning process
	 * */
	void MultiClassSVM::setProblem(struct svm_problem *p) {
		this->problem = p;
	}	
	
	
	/**
	 * 	Returns the problem in libsvm format
	 * 
	 * 	@return The problem in LibSVM format
	 * */
	struct svm_problem *MultiClassSVM::getProblem() {
		return this->problem;
	}
	
	double *MultiClassSVM::getClassification() {
		return test_problem->y;
	}
	
	/**
	 *	Set the new training set 
	 * 	
	 * 	@param	ts 	The training set to set
	 * */
	void MultiClassSVM::setTrainingSet(DataSet *ts) {
		AbstractSVM::setTrainingSet(ts);
		this->trainingSetToProblem();
	}
	
	
	/**
	 * 	The classification process
	 * 
	 * */
	void MultiClassSVM::classify() {
		int i;
		for (i = 0; i < this->test_problem->l; i++) {
			test_problem->y[i] = svm_predict(model, test_problem->x[i]);
		}
	}
	
	/**
	 * 	The classification process
	 * 	
	 * 	@param	test	The test set to set before run the classification process
	 * */
	void MultiClassSVM::classify(DataSet *test) {
		// TODO to implement
		// it acts on test set
	}

	/**
	 * 	The classification process
	 * 	
	 * 	@param	test	The test set to set before run the classification process (in LibSVM format)
	 * */
	void MultiClassSVM::classify(struct svm_problem *test) {
		test_problem = test;
		classify();
	}



	/**
	 * 	Returns the trained model
	 * 
	 * 	@reteurn The trained model, with objective function, lagrangians, etc.
	 * */
	struct svm_model *MultiClassSVM::getModel() {
		return this->model;
	}
	
	
	// private methods
	
	
	/**
	 * 	Convert a training set format to svm_problem libsvm format
	 * 
	 * */
	void MultiClassSVM::trainingSetToProblem() {
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
