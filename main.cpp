#include <iostream>

#include "RoguelikeGame.hpp"
#include "DenseNN.hpp"

int main(){

/*    
    std::vector<int> layers;
    std::vector<ActivationFunc*> activations;
    layers.push_back(2);
    activations.push_back(new Sigmoid());
    layers.push_back(2);
    activations.push_back(new Sigmoid());
    layers.push_back(1);
    activations.push_back(new Sigmoid());

    DenseNN nn(layers, activations);
    nn.print();     

    MatrixXd in(2, 1);
    in << 0.0, 0.0;
    MatrixXd expected(1, 1);
    expected << 0.0;
    nn.train(in, expected, 1.0);
    
    nn.print();
*/
    RoguelikeGame game = RoguelikeGame();
   
    game.start();

    return 0;
}
