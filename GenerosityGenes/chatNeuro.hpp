#pragma once

#include "include.hpp"

const double precision = 0.05;
using std::string; using std::vector;
class NeuralNetwork;
class Layer {
public:
    Layer(size_t input_size, size_t output_size);
    const vector<vector<double>>& weights() const;
    void save_weights(const string& filename) const;
    void load_weights(const string& filename);

    size_t input_size_;
    size_t output_size_;
private:
    vector<vector<double>> weights_;
    friend NeuralNetwork;

};



class NeuralNetwork {
public:
    NeuralNetwork(const vector<std::pair<size_t, size_t>>& layer_sizes, string way);
    void mutate();
    vector<double> forward_minion(const vector<double> input) const;
    vector<double> forward_colony(const vector<double> input) const;
    void train(const vector<double>& input, const vector<double>& output, size_t answerId, const double points);
    void SaveAI()const;
    void LoadAI();
    string NeuralNetworkWay;
    vector<Layer> layers_;
    NeuralNetwork operator=(const NeuralNetwork& ptr);
};