#include "chatNeuro.hpp"

Layer::Layer(size_t input_size, size_t output_size) : input_size_(input_size), output_size_(output_size) {
    // ≥н≥ц≥ал≥зуЇмо випадков≥ ваги
    weights_ = std::vector<std::vector<double>>(input_size_, std::vector<double>(output_size_));
    srand(rand());
    for (size_t i = 0; i < input_size_; ++i) {
        for (size_t j = 0; j < output_size_; ++j) {
            weights_[i][j] = (rand() % 200 * 0.01)-1; // -1.0000 to 1.0000
        }
    }
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
    if (file.is_open()) {
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
}

NeuralNetwork::NeuralNetwork(const std::vector<std::pair<size_t, size_t>>& layer_sizes, string way) :NeuralNetworkWay(way) {
    for (const auto& layer_size : layer_sizes) {
        layers_.emplace_back(layer_size.first, layer_size.second);
    }
}
//Neural network calculation for minion
vector<double> NeuralNetwork::forward_minion(vector<double> input) const {
    std::vector<double> output;
    double sum;
    for (size_t l = 0; l < layers_.size(); ++l)
    {
        
        if (l>0) //¬х≥д наступного вих≥д минулого починаючи з 2 шару
        {
            input.resize(layers_[l].input_size_);
            std::copy(output.begin(), output.end(), input.begin());
        }
        output.resize(layers_[l].output_size_);

        //ќбрахунки
        for (size_t j = 0; j < layers_[l].output_size_; ++j) {
            sum = 0;
            for (size_t i = 0; i < layers_[l].input_size_; ++i) {
                // рахуЇмо вих≥д шару // перш≥ 75 з першого слою за формулою sum += cos((input[i]+weights_[i][j])*(PI/2)) ус≥ ≥нш≥ sum += input[i] * layer.weights_[i][j]; s
                if (i < 75 && l == 0) {
                    sum += std::sin((input[i] + layers_[l].weights()[i][j]) * (3.14 / 2));//sin((input *  weight)*pi/2)
                }
                else
                {
                    sum += input[i] * layers_[l].weights_[i][j]; //input *  weight
                }
            }
            output[j] = (2.0 / (exp(sum) + 1.0)) - 1.0;
        }
    }
    return output;
}
vector<double> NeuralNetwork::forward_colony(vector<double> input) const {
    std::vector<double> output;
    double sum;
    for (size_t l = 0; l < layers_.size(); ++l)
    {

        if (l > 0) //¬х≥д наступного вих≥д минулого починаючи з 2 шару
        {
            input.resize(layers_[l].input_size_);
            std::copy(output.begin(), output.end(), input.begin());
        }
        output.resize(layers_[l].output_size_);

        //ќбрахунки
        for (size_t j = 0; j < layers_[l].output_size_; ++j) {
            sum = 0;
            for (size_t i = 0; i < layers_[l].input_size_; ++i) 
            {
                sum += input[i] * layers_[l].weights_[i][j];
            }
            output[j] = (2.0 / (exp(sum) + 1.0)) - 1.0;
        }
    }
    return output;
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
    for (int h = 0; h < layers_.size(); ++h)
    {
        layers_[h].load_weights(NeuralNetworkWay + '_' + std::to_string(h));
    }
}
NeuralNetwork NeuralNetwork::operator=(const NeuralNetwork& ptr)
{
    layers_.clear();

    for (const Layer& layer : ptr.layers_) {
        Layer newLayer(layer.input_size_, layer.output_size_);
        newLayer.weights_ = layer.weights_;
        layers_.push_back(newLayer);
    }
    NeuralNetworkWay = ptr.NeuralNetworkWay;

    return *this;
}