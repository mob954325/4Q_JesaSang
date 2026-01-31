#include "ComponentFactory.h"

const std::unordered_map<std::string, ComponentEntry>& ComponentFactory::GetRegisteredComponents()
{
    return registeredComponents;
}