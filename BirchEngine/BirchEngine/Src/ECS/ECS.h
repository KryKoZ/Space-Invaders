#pragma once

//Ne pas confondre composant et entité. Une entité va être un joueur, une ennemi etc.. Un composant va être la physique du jouer/ennemi, sa position etc...
//Using est la façon dont on délcare des variables en dehors de classes (ici Component et Entity) et dont la portée se limite au fichier.
//Fichier en grande partie générique, allant du composant le plus sépcifique a celui le plus général (Component class < Entity class < Manager class)
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>

class Component;
class Entity;
class Manager;

using ComponentID = std::size_t;
using Group = std::size_t;

//Génere un ID pour chaque nouveau composant et le retourn. Est incrémenté de 1 pour chaque nouveau composant créé.
inline ComponentID getNewComponentTypeID() 
{ 
	static ComponentID lastID = 0u;
	return lastID++;
}

//Crée un ID pour chaque type de composant et le retourne.(ex: ID.1=physics, ID.2=position..)
template <typename T> inline ComponentID getComponentTypeID() noexcept
{
	static ComponentID typeID = getNewComponentTypeID();
	return typeID;
}

//Une entité ne peut pas avoir plus de 32 composants différents.
constexpr std::size_t maxComponents = 32;
//Mécanique des layers
constexpr std::size_t maxGroups = 32;

//Attribuer un bitset à une entité nous permettra de comparer si une entité possède bien tous les composants qui lui sont attribués ou non.
//Un bitset est un tableau composé de 0 ou de 1, donc un tableau optimisé ne contenant que des valeurs booléennes.
using ComponentBitSet = std::bitset<maxComponents>;
//Comme au dessus, mais pour quel layer est activé ou non
using GroupBitSet = std::bitset<maxGroups>;
//ComponentArray est un tableau qui contient des pointeurs sur chacun des composants attribués à une entité avec pour taille maximale maxComponent
using ComponentArray = std::array<Component*, maxComponents>;

//Classe virtuelle dont les fonctions seront re-utilisées par des classes enfant (chaque composant)
class Component
{
public:
	Entity* entity;

	//Fonctions virtuelles afin de pouvoir être re-écrites pour chaque type d'entité avec les classes correspondantes
	virtual void init() {}
	virtual  void update() {}
	virtual void draw() {}

	virtual ~Component() {}
};

//Classe entity avec le système de composants
class Entity
{
private:
	Manager& manager;
	//Si active(true) -> en jeu ; Si nonActive (false) -> Supprimée du jeu
	bool active = true;
	//Liste de tous les composants que l'entité contient dans un vector
	std::vector<std::unique_ptr<Component>> components;
	ComponentArray componentArray;
	ComponentBitSet componentBitSet;
	GroupBitSet groupBitSet;

public:

	Entity(Manager& mManager) : manager(mManager) {}

	//Pour chaque composant que contient l'entité, on fait appel aux fonctions update() et draw() du composant redéfinies dans les classes enfants (en parcourant le vector components)
	void update()
	{
		for (auto& c : components) c->update();
	}

	void draw() 
	{
		for (auto& c : components) c->draw();
	}

	bool isActive() const { return active;  }
	void destroy() { active = false;  }

	bool hasGroup(Group mGroup)
	{
		return groupBitSet[mGroup];
	}

	void addGroup(Group mGroup);
	void delGroup(Group mGroup)
	{
		groupBitSet[mGroup] = false;
	}
	//Retourne vrai ou faux selon si l'entité possède le composant qui à l'ID retournée par getComponentTypeID
	template <typename T> bool hasComponent() const
	{
		return componentBitSet[getComponentTypeID<T>()];
	}

	//Ajoute un composant à une entité
	template <typename T, typename... TArgs>
	// Ici les "&&", std::forward, "..." etc.. Sont pour résumer des outils pour faire passer des arguments de manière générique selon le constructeur et le type du composant.
	T& addComponent(TArgs&&... mArgs)
	{
		//Instanciation d'un nouveau composant de type T avec les arguments mArgs (prog générique)
		T* c(new T(std::forward<TArgs>(mArgs)...));
		//Le composant est lié à l'entité courante (cf attribut "entity" du composant)
		c->entity = this;
		//On crée un pointeur sur le composant fraichement généré.
		std::unique_ptr<Component> uPtr{ c };
		//On ajoute le composant fraichement généré à la fin du vector contenant tous les composants de l'entité.
		components.emplace_back(std::move(uPtr));

		//Permet d'attribuer un certain type de composant à une position dans le tableau. Ex : Peu importe l'entité courante ou les autres composants attribués à l'entité, le composant "position" sera toujours à l'emplacement 2 du tableau.
		componentArray[getComponentTypeID<T>()] = c;
		//On indique au bitset que désormais, l'entité courante possède le composant que l'on vient de créer
		componentBitSet[getComponentTypeID<T>()] = true;

		c->init();
		return *c;
	}

	//Renvoie le composant de type T de l'entité courante, ce qui nous permettra de faire des choses simplifiés comme par exemple : gameObject.getComponent<PositionComponent>().setXpos(25);
	template<typename T> T& getComponent() const
	{
		auto ptr(componentArray[getComponentTypeID<T>()]);
		return *static_cast<T*>(ptr);
	}

};

//Permet de gérer les entité
class Manager
{
private:
	//Listes des entités en jeu
	std::vector<std::unique_ptr<Entity>> entities;
	//Groupe les pointurs sur entités par 32 (maxGroups)
	std::array<std::vector<Entity*>, maxGroups> groupedEntities;

public:
	void update() 
	{
		for (auto& e : entities) e->update(); //Update chacune des entités qui elles même update chacun des composants par la suite..
	}

	void draw()
	{
		for (auto& e : entities) e->draw(); // Même qu'update mais draw
	}

	//Nettoie les entitées qui ne sont plus actives (if suivi d'un erase sur un vector en plus compact)
	void refresh()
	{
		for (auto i(0u); i < maxGroups; i++)
		{
			auto& v(groupedEntities[i]);
			v.erase(
				std::remove_if(std::begin(v), std::end(v), [i](Entity* mEntity)
			{
				return !mEntity->isActive() || !mEntity->hasGroup(i);
			}),
			std::end(v));
		}

		entities.erase(std::remove_if(std::begin(entities), std::end(entities), [](const std::unique_ptr<Entity>& mEntity)
			{
				return !mEntity->isActive();
			}),
			std::end(entities));
	}

	void AddToGroup(Entity* mEntity, Group mGroup)
	{
		groupedEntities[mGroup].emplace_back(mEntity);
	}

	std::vector<Entity*>& getGroup(Group mGroup)
	{
		return groupedEntities[mGroup];
	}

	//Instancie une nouvelle entité et l'ajoute dans le vector contenant toutes les entités.
	Entity& addEntity()
	{
		Entity* e = new Entity(*this);
		std::unique_ptr<Entity> uPtr{ e };
		entities.emplace_back(std::move(uPtr));
		return *e;
	}
};