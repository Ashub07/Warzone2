#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Player.h"
#include "Map.h"
#include "LoggingObserver.h"

//create Orders class, and the subclasses are the deploy, attack, negotiate, etc. user input determines which subclass is created 
//(and can also make invalid order that's placed in list and then jsut ignored)
//Orderlist class will hold the orders

class Orders : public Subject, public ILoggable
{
protected:
	Player* player;

public:
	//constructors
	Orders();
	Orders(Player* player);

	Orders(const Orders& order);

	//destructor
	virtual ~Orders();

	//assignment+stream insertion operator
	Orders& operator=(const Orders& order);
	friend std::ostream& operator<<(std::ostream& os, const Orders& order);

	//getters
	Player getPlayer() const;

	//setters
	void setPlayer(Player playr);

	//methods needed:
	virtual bool validate() const = 0;
	virtual bool execute() const = 0;
	virtual Orders* clone() const = 0;
};

//subclasses
class Deploy : public Orders {
private:
	Territory* targ;
	int* armyNum;
public:
	//constructors
	Deploy();
	Deploy(Player* player, Territory* target, int* armnum);

	Deploy(const Deploy& order);

	//destructor
	~Deploy();

	//assignment+stream insertion operator
	Deploy& operator=(const Deploy& order);
	friend std::ostream& operator<<(std::ostream& os, const Deploy& order);

	//getters
	Player getPlayer() const;
	Territory getTarg() const;
	int getArmynum() const;

	//setters
	void setPlayer(Player playr);
	void setTarget(Territory target);
	void setArmynum(int armnum);

	//methods needed:
	virtual bool validate() const;
	virtual bool execute() const;
	virtual Deploy* clone() const;

	std::string stringToLog() const override; //part 5
};

class Advance : public Orders {
private:
	Territory* targ;
	Territory* source;
	int* armyNum;

public:
	//constructors
	Advance();
	Advance(Player* player, Territory* targ, Territory* source, int* armnum);

	Advance(const Advance& order);

	//destructor
	~Advance();

	//assignment+stream insertion operator
	Advance& operator=(const Advance& order);
	friend std::ostream& operator<<(std::ostream& os, const Advance& order);

	//getters
	Player getPlayer() const;
	Territory getTarg() const;
	Territory getSource() const;
	int getArmynum() const;

	//setters
	void setPlayer(Player playr);
	void setTarget(Territory target);
	void setSource(Territory source);
	void setArmynum(int armnum);

	//methods needed:
	virtual bool validate() const;
	virtual bool execute() const;
	virtual Advance* clone() const;

	std::string stringToLog() const override; //part 5
};

class Bomb : public Orders {
private:
	Territory* targ;

public:
	//constructors
	Bomb();
	Bomb(Player* player, Territory* target);

	Bomb(const Bomb& order);

	//destructor
	~Bomb();

	//assignment+stream insertion operator
	Bomb& operator=(const Bomb& order);
	friend std::ostream& operator<<(std::ostream& os, const Bomb& order);

	//getters
	Player getPlayer() const;
	Territory getTarg() const;

	//setters
	void setPlayer(Player playr);
	void setTarget(Territory target);

	//methods needed:
	virtual bool validate() const;
	virtual bool execute() const;
	virtual Bomb* clone() const;

	std::string stringToLog() const override; //part 5
};

class Blockade : public Orders {
private:
	Territory* targ;

public:
	//constructors
	Blockade();
	Blockade(Player* player, Territory* target);

	Blockade(const Blockade& order);

	//destructor
	~Blockade();

	//assignment+stream insertion operator
	Blockade& operator=(const Blockade& order);
	friend std::ostream& operator<<(std::ostream& os, const Blockade& order);

	//getters
	Player getPlayer() const;
	Territory getTarg() const;

	//setters
	void setPlayer(Player playr);
	void setTarget(Territory target);
	
	//methods needed:
	virtual bool validate() const;
	virtual bool execute() const;
	virtual Blockade* clone() const;

	std::string stringToLog() const override; //part 5
};

class Airlift : public Orders {
private:
	Territory* targ;
	Territory* source;
	int* armyNum;

public:
	//constructors
	Airlift();
	Airlift(Player* player, Territory* target, Territory* source, int* armnum);
	Airlift(const Airlift& order);

	//destructor
	~Airlift();

	//assignment+stream insertion operator
	Airlift& operator=(const Airlift& order);
	friend std::ostream& operator<<(std::ostream& os, const Airlift& order);

	//getters
	Player getPlayer() const;
	Territory getTarg() const;
	Territory getSource() const;
	int getArmynum() const;

	//setters
	void setPlayer(Player playr);
	void setTarget(Territory target);
	void setSource(Territory source);
	void setArmynum(int armnum);

	//methods needed:
	virtual bool validate() const;
	virtual bool execute() const;
	virtual Airlift* clone() const;

	std::string stringToLog() const override; //part 5
};

class Negotiate : public Orders {
private:
	Player* targ;

public:
	//constructors
	Negotiate();
	Negotiate(Player* player, Player* targ);

	Negotiate(const Negotiate& order);

	//destructor
	~Negotiate();

	//assignment+stream insertion operator
	Negotiate& operator=(const Negotiate& order);
	friend std::ostream& operator<<(std::ostream& os, const Negotiate& order);

	//getters
	Player getPlayer() const;
	Player getTarget() const;

	//setters
	void setPlayer(Player playr);
	void setTarget(Player targ);

	//methods needed:
	virtual bool validate() const;
	virtual bool execute() const;
	virtual Negotiate* clone() const;

	std::string stringToLog() const override; //part 5
};

class OrdersList : public Subject, public ILoggable
{
private:
	std::vector<Orders*>* orders;
	int length;

public:
	//constructors
	OrdersList();
	OrdersList(const OrdersList& order);

	//destructor
	~OrdersList();

	//assignment+stream insertion operator
	OrdersList& operator=(const OrdersList& order);
	friend std::ostream& operator<<(std::ostream& os, const OrdersList& order);

	//getters
	std::vector<Orders*> getOrders() const;
	//setters
	void setOrders(std::vector<Orders*> orderss);

	//methods needed:
	void remove(Orders* order);
	void add(Orders* order);
	void move(Orders* order1, Orders* order2);

	std::string stringToLog() const override; //part 5
};