#pragma once

#include <vector>
#include <iostream>
#include <cstdlib>

#include "Eigen/Dense"

using Eigen::MatrixXd;

class ActivationFunc {
public:

    virtual ~ActivationFunc(){}

    virtual double apply(double x) = 0;
    virtual double applyDerivative(double x) = 0;

    virtual MatrixXd matApply(MatrixXd mat) = 0;
    virtual MatrixXd matApplyDerivative(MatrixXd mat) = 0;
};

class Sigmoid : public ActivationFunc {
public:
    virtual double apply(double x) override {
        std::cout << "Calling apply" << std::endl;
        return 0.0; 
    }
    virtual double applyDerivative(double x) override {
        double sig = apply(x);
        return sig*(1-sig);
    }

    virtual MatrixXd matApply(MatrixXd mat) override {
        mat *= -1;
        mat = (mat.array().exp()).matrix();
        mat = (mat.array() + 1).matrix();
        
        mat = (mat.array().cwiseInverse()).matrix();
        return mat;
    }
    virtual MatrixXd matApplyDerivative(MatrixXd mat) override {
        MatrixXd sig = matApply(mat);
        return sig.cwiseProduct((1 - sig.array()).matrix());
    } 

};

//TODO: do as much at compile time as possible
class DenseNN {
public:
    DenseNN(std::vector<int>& layers, std::vector<ActivationFunc*>& activations){
        if(m_layers.size() != m_activations.size()){
            std::cout << "Expected same number of activations as layers" << std::endl;
            exit(EXIT_FAILURE);
        }
        m_layers = layers;
        m_activations = activations;

        for(int i = 0; i < layers.size()-1; i++){
            m_weights.push_back(MatrixXd::Random(layers[i+1], layers[i]));
            m_biases.push_back(MatrixXd::Random(layers[i+1], 1));
        }
    }

    ~DenseNN(){ for(ActivationFunc* a : m_activations){
            delete a;
        }
    }

    MatrixXd apply(MatrixXd in){
        //TODO: typecheck in
        MatrixXd mat = in;
        for(int layer = 0; layer < m_weights.size(); layer++){
            mat = m_activations[layer]->matApply(mat);
            mat = m_weights[layer] * mat;
            mat += m_biases[layer];
        }
        //apply activation func on the final
        mat = m_activations[m_layers.size()-1]->matApply(mat);
        return mat;
    }

    MatrixXd train(MatrixXd in, MatrixXd expected, double learningRate){
        //feed forward and store activations and pre activations
        std::vector<MatrixXd> preActivations;
        std::vector<MatrixXd> activations;
        MatrixXd mat = in;
        for(int layer = 0; layer < m_weights.size(); layer++){
            mat = m_activations[layer]->matApply(mat);
            activations.push_back(mat);
            mat = m_weights[layer] * mat;
            mat += m_biases[layer];
            preActivations.push_back(mat);
        }
        mat = m_activations.back()->matApply(mat);

        //calculate the partial derivative of the cost function with respect to all the preActivations
        std::vector<MatrixXd> errors = std::vector<MatrixXd>(m_weights.size());
        MatrixXd lastError = mat - expected;
        lastError = lastError.cwiseProduct(m_activations.back()->matApplyDerivative(preActivations.back()));
        errors.back() = lastError;
        for(int layer = errors.size()-2; layer >= 0; layer--){
            errors[layer] = m_weights[layer+1].transpose() * errors[layer+1];
            errors[layer] = errors[layer].cwiseProduct(m_activations[layer+1]->matApplyDerivative(preActivations[layer]));
        }

        //use the partial derivative with respect to preactivations to calculate the partial derivative with respect to each weight
        //and update biases based on this partial as the partial derivative with respect to preactivations 
        //is the partial derivative with respect to the biases

        for(int i = 0; i < m_weights.size(); i++){
            MatrixXd weightPartial = errors[i] * activations[i].transpose();
            m_weights[i] -= learningRate * weightPartial;
        }

        for(int i = 0; i < m_biases.size(); i++){
            m_biases[i] -= learningRate * errors[i];
        }

        return mat;
    }

    void print(){
        std::cout << "Weights: " << std::endl;
        for(MatrixXd mat : m_weights){
            std::cout << mat << std::endl << std::endl;
        }

        std::cout << "Biases: " << std::endl;
        for(MatrixXd mat : m_biases){
            std::cout << mat << std::endl << std::endl;
        }
    }
private:
    std::vector<ActivationFunc*> m_activations;
    std::vector<int> m_layers;
    std::vector<MatrixXd> m_weights;
    std::vector<MatrixXd> m_biases;
};
