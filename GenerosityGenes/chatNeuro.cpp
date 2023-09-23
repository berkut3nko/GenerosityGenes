#include "chatNeuro.hpp"

Layer::Layer(size_t input_size, size_t output_size) : input_size_(input_size), output_size_(output_size) {
    // ініціалізуємо випадкові ваги
    weights_ = std::vector<std::vector<double>>(input_size_, std::vector<double>(output_size_));
    for (size_t i = 0; i < input_size_; ++i) {
        for (size_t j = 0; j < output_size_; ++j) {
            weights_[i][j] = (rand() % 20000 * 0.0001)-1;
        }
    }

}

std::vector<double> Layer::forward(const std::vector<double>& input)const {
    std::vector<double> output(output_size_);

    // рахуємо вихід шару
    for (size_t j = 0; j < output_size_; ++j) {
        double sum = 0;
        for (size_t i = 0; i < input_size_; ++i) {
            sum += input[i] * weights_[i][j];
        }
        output[j] = sum;
    }

    return output;
}
const std::vector<std::vector<double>>& Layer::weights()const {
    return weights_;
}
void Layer::save_weights(const string& filename) const {
    std::ofstream file(filename);
    for (const auto& row : weights_) {
        for (const auto& value : row) {
            file << value << ",";
        }
        file << "\n";
    }
    file.close();
}

void Layer::load_weights(const string& filename) {
    std::ifstream file(filename);
    string line;
    weights_.clear();
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        string value_str;
        std::vector<double> row;
        while (std::getline(iss, value_str, ',')) {
            row.push_back(std::stod(value_str));
        }
        weights_.push_back(row);
    }
    file.close();
}

NeuralNetwork::NeuralNetwork(const std::vector<std::pair<size_t, size_t>>& layer_sizes, string way) :NeuralNetworkWay(way) {
    srand(static_cast<unsigned int>(time(NULL)));
    for (const auto& layer_size : layer_sizes) {
        layers_.emplace_back(layer_size.first, layer_size.second);
    }
}

vector<double> NeuralNetwork::forward(const vector<double>& input) const {
    vector<double> result(input);
    for (const auto& layer : layers_) {
        result = layer.forward(result);
    }
    return result;
}


void NeuralNetwork::train(const vector<double>& input, const vector<double>& output, size_t answerId, const double points)
{
    double learning_step = 0.5;

    for (vector<double> weights : layers_[layers_.size()].weights_)
    {
        if (weights[answerId] > learning_step)weights[answerId] += points;
        else if (weights[answerId] < -learning_step)weights[answerId] -= points;
        else if(weights[answerId] > 0)weights[answerId] -= points;
        else if(weights[answerId] < 0)weights[answerId] += points;
    }
    
}

void NeuralNetwork::mutate()
{
    for (Layer layer : layers_)
    {
        for (vector<double> weights : layer.weights_)
        {
            for (double weight : weights)
            {
                weight += double(rand() % 3 - 1) / 50;
            }
        }
    }

}
void NeuralNetwork::SaveAI()const
{
    for (int i = 0; i < layers_.size(); ++i)
    {
        layers_[i].save_weights(NeuralNetworkWay + '_' + std::to_string(i));
    }
}
void NeuralNetwork::LoadAI()
{
    for (int i = 0; i < layers_.size(); ++i)
    {
        layers_[i].load_weights(NeuralNetworkWay + '_' + std::to_string(i));
    }
}