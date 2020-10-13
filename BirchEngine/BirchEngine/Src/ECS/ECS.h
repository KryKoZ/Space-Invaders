#pragma once

//Ne pas confondre composant et entit�. Une entit� va �tre un joueur, une ennemi etc.. Un composant va �tre la physique du jouer/ennemi, sa position etc...
//Using est la fa�on dont on d�lcare des variables en dehors de classes (ici Component et Entity) et dont la port�e se limite au fichier.
//Fichier en grande partie g�n�rique, allant du composant le plus s�pcifique a celui le plus g�n�ral (Component class < Entity class < Manager class)
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

//G�nere un ID pour chaque nouveau composant et le retourn. Est incr�ment� de 1 pour chaque nouveau composant cr��.
inline ComponentID getNewComponentTypeID() 
{ 
	static ComponentID lastID = 0u;
	return lastID++;
}

//Cr�e un ID pour chaque type de composant et le retourne.(ex: ID.1=physics, ID.2=position..)
template <typename T> inline ComponentID getComponentTypeID() noexcept
{
	static ComponentID typeID = getNewComponentTypeID();
	return typeID;
}

//Une entit� ne peut pas avoir plus de 32 composants diff�rents.
constexpr std::size_t maxComponents = 32;
//M�canique des layers
constexpr std::size_t maxGroups = 32;

//Attribuer un bitset � une entit� nous permettra de comparer si une entit� poss�de bien tous les composants qui lui sont attribu�s ou non.
//Un bitset est un tableau compos� de 0 ou de 1, donc un tableau optimis� ne contenant que des valeurs bool�ennes.
using ComponentBitSet = std::bitset<maxComponents>;
//Comme au dessus, mais pour quel layer est activ� ou non
using GroupBitSet = std::bitset<maxGroups>;
//ComponentArray est un tableau qui contient des pointeurs sur chacun des composants attribu�s � une entit� avec pour taille maximale maxComponent
using ComponentArray = std::array<Component*, maxComponents>;

//Classe virtuelle dont les fonctions seront re-utilis�es par des classes enfant (chaque composant)
class Component
{
public:
	Entity* entity;

	//Fonctions virtuelles afin de pouvoir �tre re-�crites pour chaque type d'entit� avec les classes correspondantes
	virtual void init() {}
	virtual  void update() {}
	virtual void draw() {}

	virtual ~Component() {}
};

//Classe entity avec le syst�me de composants
class Entity
{
private:
	Manager& manager;
	//Si active(true) -> en jeu ; Si nonActive (false) -> Supprim�e du jeu
	bool active = true;
	//Liste de tous les composants que l'entit� contient dans un vector
	std::vector<std::unique_ptr<Component>> components;
	ComponentArray componentArray;
	ComponentBitSet componentBitSet;
	GroupBitSet groupBitSet;

public:

	Entity(Manager& mManager) : manager(mManager) {}

	//Pour chaque composant que contient l'entit�, on fait appel aux fonctions update() et draw() du composant red�finies dans les classes enfants (en parcourant le vector components)
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
	//Retourne vrai ou faux selon si l'entit� poss�de le composant qui � l'ID retourn�e par getComponentTypeID
	template <typename T> bool hasComponent() const
	{
		return componentBitSet[getComponentTypeID<T>()];
	}

	//Ajoute un composant � une entit�
	template <typename T, typename... TArgs>
	// Ici les "&&", std::forward, "..." etc.. Sont pour r�sumer des outils pour faire passer des arguments de mani�re g�n�rique selon le constructeur et le type du composant.
	T& addComponent(TArgs&&... mArgs)
	{
		//Instanciation d'un nouveau composant de type T avec les arguments mArgs (prog g�n�rique)
		T* c(new T(std::forward<TArgs>(mArgs)...));
		//Le composant est li� � l'entit� courante (cf attribut "entity" du composant)
		c->entity = this;
		//On cr�e un pointeur sur le composant fraichement g�n�r�.
		std::unique_ptr<Component> uPtr{ c };
		//On ajoute le composant fraichement g�n�r� � la fin du vector contenant tous les composants de l'entit�.
		components.emplace_back(std::move(uPtr));

		//Permet d'attribuer un certain type de composant � une position dans le tableau. Ex : Peu importe l'entit� courante ou les autres composants attribu�s � l'entit�, le composant "position" sera toujours � l'emplacement 2 du tableau.
		componentArray[getComponentTypeID<T>()] = c;
		//On indique au bitset que d�sormais, l'entit� courante poss�de le composant que l'on vient de cr�er
		componentBitSet[getComponentTypeID<T>()] = true;

		c->init();
		return *c;
	}

	//Renvoie le composant de type T de l'entit� courante, ce qui nous permettra de faire des choses simplifi�s comme par exemple : gameObject.getComponent<PositionComponent>().setXpos(25);
	template<typename T> T& getComponent() const
	{
		auto ptr(componentArray[getComponentTypeID<T>()]);
		return *static_cast<T*>(ptr);
	}

};

//Permet de g�rer les entit�
class Manager
{
private:
	//Listes des entit�s en jeu
	std::vector<std::unique_ptr<Entity>> entities;
	//Groupe les pointurs sur entit�s par 32 (maxGroups)
	std::array<std::vector<Entity*>, maxGroups> groupedEntities;

public:
	void update() 
	{
		for (auto& e : entities) e->update(); //Update chacune des entit�s qui elles m�me update chacun des composants par la suite..
	}

	void draw()
	{
		for (auto& e : entities) e->draw(); // M�me qu'update mais draw
	}

	//Nettoie les entit�es qui ne sont plus actives (if suivi d'un erase sur un vector en plus compact)
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

	//Instancie une nouvelle entit� et l'ajoute dans le vector contenant toutes les entit�s.
	Entity& addEntity()
	{
		Entity* e = new Entity(*this);
		std::unique_ptr<Entity> uPtr{ e };
		entities.emplace_back(std::move(uPtr));
		return *e;
	}
};