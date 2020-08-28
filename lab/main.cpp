#include <iostream>

#include "RoguelikeGame.hpp"
#include "DenseNN.hpp" 
#include "Camera.hpp"
#include "BasicDungeonGenerator.hpp"
#include "Player.hpp"

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
    RoguelikeGame game;
    game.addEntity().addComponent(new Camera(&game));
    auto dungeon = new BasicDungeonGenerator(&game, 25, 50, 82745987, 100, 4, 8);
    game.addEntity().addComponent(dungeon);
    game.addEntity().addComponent(new Player(&game, dungeon->getSpawnPoint()));
    game.start();

    return 0;
}
