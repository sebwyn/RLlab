#include "Entity.hpp"

void Entity::update(){
    for(Component* component : m_components){
        component->update();
    }
}

void Entity::handleInput(int key){
    for(Component* component : m_components){
        component->handleInput(key);
    }
}

void Entity::addComponent(Component* component){
    m_components.push_back(component);
}
