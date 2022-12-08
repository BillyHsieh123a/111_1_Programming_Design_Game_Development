#include <iostream>
#include <thread>
#include <conio.h>
#include <ctime>
#include <windows.h>
#include <string>
//#include "sound.h"
using namespace std;
//constants
const int MAP_SIZE_WIDTH = 100; 
const int MAP_SIZE_HEIGHT = 25;
const int xStart = 50;
const int yStart = 12;      
const int yGap = 3; 
//global settings
int score = 0;
int modeNum = 1;
int attacked = 0;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);//color
//global header
void cursorTo(int x, int y);

//self-defined type
struct Position
{
	int x;
	int y;
	const Position operator+(const Position& p2); 
};
//operator overloading
const Position Position::operator+(const Position& p2)
{
	Position p = {0, 0};
	p.x = this->x + p2.x;
	p.y = this->y + p2.y;
	return p;
}

//another global header
void shoot(Position startPos, Position dir, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH], int range);

//Player
class Player
{
private:
	Position pos;
	int health;
	int bulletNum;
public:
	//public variables
	int direction;
	//constructors
	Player();
	Player(Position p, int h, int b, int d);
	//copy constructors
	Player(const Player& player);
	//member functions
	void printStatus();
	void getItem(int itemNumber);
	void playerShoot(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
	void checkAndMove(Position checkPos, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
	Position getPos();
	void HPcost(int damage);
	int getPlayerHP();
};
//constructors
Player::Player()
{
	pos = {5, 5};
	health = 3;
	bulletNum = 0;
	direction = 1;
}
Player::Player(Position p, int h, int b, int d)
{
	this->pos = p;
	this->health = h;
	this->bulletNum = b;
	this->direction = d;
}
//copy constructor
Player::Player(const Player& player)
{
	this->pos = player.pos;
	this->health = player.health;
	this->bulletNum = player.bulletNum;
	this->direction = player.direction;
}
//member function
void Player::printStatus()
{
    cursorTo(0, MAP_SIZE_HEIGHT);
    //health
    SetConsoleTextAttribute(hConsole, 10);
    cout << "Health: ";
    cout << health;
    //bullet
    SetConsoleTextAttribute(hConsole, 14);
    cout << "\n";
    cout << "Bullet Number: ";
    cout << bulletNum;
    //score
    SetConsoleTextAttribute(hConsole, 11);
    cout << "\n\n";
    cout << "Score: ";
    cout << score;
	//done
    SetConsoleTextAttribute(hConsole, 15);
    return;
}
void Player::getItem(int itemNumber)
{
	switch(itemNumber)
	{
        case 2:
            bulletNum += 1;
            break;
        case 3:
            health += 1;
            break;
        case 4:
            score += 1;
	}
	printStatus();
	return;
}
void Player::playerShoot(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	if (bulletNum == 0)
		return;
	//set color
	SetConsoleTextAttribute(hConsole, 14);
	//settings
	this->bulletNum--;
    int range = 10;
    int bulletDirection = this->direction;
    Position shootDir = {0};
    //shoot
    switch (bulletDirection)
    {
    	case 1:
    		shootDir = {0, -1};
    		break;
    	case 2:
    		shootDir = {0, 1};
    		break;
    	case 4:
    		shootDir = {1, 0};
    		break;
    	case 3:
    		shootDir = {-1, 0};
	}
	shoot(this->pos, shootDir, map, range);
	//reset color
	SetConsoleTextAttribute(hConsole, 15);
	return;
} 
void Player::checkAndMove(Position checkPos, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	//check if front is not wall 
    if(map[checkPos.y][checkPos.x] != 1 && map[checkPos.y][checkPos.x] != 5)
    {
    	//player disappear
    	map[this->pos.y][this->pos.x] = 0;
        cursorTo(this->pos.x, this->pos.y);
        printf(" ");
        //check if get item
    	this->getItem(map[checkPos.y][checkPos.x]);
    	//adjust player position
    	switch(this->direction)
    	{
    		case 1:
    			this->pos.y--;
    			break;
    		case 2:
    			this->pos.y++;
    			break;
    		case 4:
    			this->pos.x++;
    			break;
    		case 3:
    			this->pos.x--;
		}
        map[this->pos.y][this->pos.x] = -1;
    }
    //if front is a movable wall
    else if (map[checkPos.y][checkPos.x] != 1 && map[checkPos.y][checkPos.x] == 5)
    {
    	//check the place in front of the movable wall
    	Position frontOfMW = {0};
    	switch(this->direction)
    	{
    		case 1:
    			frontOfMW.x = this->pos.x;
    			frontOfMW.y = this->pos.y - 2;
    			break;
    		case 2:
    			frontOfMW.x = this->pos.x;
    			frontOfMW.y = this->pos.y + 2;
    			break;
    		case 4:
    			frontOfMW.x = this->pos.x + 2;
    			frontOfMW.y = this->pos.y;
    			break;
    		case 3:
    			frontOfMW.x = this->pos.x - 2;
    			frontOfMW.y = this->pos.y;
		}
		//if movable
    	if(map[frontOfMW.y][frontOfMW.x] != 1 && map[frontOfMW.y][frontOfMW.x] != 5 && map[frontOfMW.y][frontOfMW.x] < 100)
        {
        	//player disappear
        	map[this->pos.y][this->pos.x] = 0;
	        cursorTo(this->pos.x, this->pos.y);
	        printf(" ");
        	//move the movable wall
            cursorTo(frontOfMW.x, frontOfMW.y);
            SetConsoleTextAttribute(hConsole, 8);
            cout << "#";
            SetConsoleTextAttribute(hConsole, 15);
            map[frontOfMW.y][frontOfMW.x] = 5;
            //player position change
            switch(this->direction)
    		{
	    		case 1:
	    			this->pos.y--;
	    			break;
	    		case 2:
	    			this->pos.y++;
	    			break;
	    		case 4:
	    			this->pos.x++;
	    			break;
	    		case 3:
	    			this->pos.x--;
			}
        	map[this->pos.y][this->pos.x] = -1;
        }
	}
	//player appear at new place and item(if is 2, 3, 4) disappear
    cursorTo(this->pos.x, this->pos.y);//moving cursor to the new position

    SetConsoleTextAttribute(hConsole, 14);

    switch(this->direction)
	{                

		case 1:
			printf("^");
			break;
		case 2:
			printf("v");
			break;
		case 4:
			printf(">");
			break;
		case 3:
			printf("<");
	}

    SetConsoleTextAttribute(hConsole, 15);

	return;
}
Position Player::getPos()
{
	return this->pos;
}
void Player::HPcost(int damage)
{
	this->health -= damage;
	return;
}
int Player::getPlayerHP()
{
	return this->health;
}


//Enemy
class Enemy
{
protected:
	static const int moveThershold = 40000;
	Position pos;
	int speed;
	int HP;//alive true or false
	int ATK;
	int enemyID;
	int moveCnt;
	int itemRecord;
	//private functions
	void checkAndMove(Position checkPos, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
    void printBackRecord(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
public:
	//public variables
	int direction;//enemy might don't need this
	//constructors
	Enemy();
	Enemy(Position p, int s, int h, int a, int d, int i);
	//copy constructor
	Enemy(const Enemy& e);
	//member functions
	void enemySpawn(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
	virtual void enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]) = 0;//put this into the main while loop, if (e1.getEnemyHP != 0)
	Position getEnemyPos();
	int getEnemyHP();
};
//constructor
Enemy::Enemy()
{
	pos = {0, 0};
	speed = 0;
	HP = 0;
	ATK = 0;
	direction = 4;
	enemyID = 101;
	moveCnt = 0;
	itemRecord = 0;
}
Enemy::Enemy(Position p, int s, int h, int a, int d, int i)
{

	this->pos = p;
	this->speed = s * modeNum;
	this->HP = h;
	this->ATK = a * modeNum;
	this->direction = d;
	this->enemyID = i;
	this->moveCnt = 0;
	this->itemRecord = 0;
}
//copy constrctor
Enemy::Enemy(const Enemy& e)
{
	this->pos = e.pos;
	this->speed = e.speed;
	this->HP = e.HP;
	this->ATK = e.ATK;
	this->direction = e.direction;
	this->enemyID = e.enemyID;
	this->moveCnt = e.moveCnt;
	this->itemRecord = e.itemRecord;
}
//member function
void Enemy::enemySpawn(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	map[this->pos.y][this->pos.x] = enemyID;
	cursorTo(this->pos.x, this->pos.y);
    SetConsoleTextAttribute(hConsole, 9);
	cout << "@";
    SetConsoleTextAttribute(hConsole, 15);
	return;
}
void Enemy::printBackRecord(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])//differ for every enemy
{
    map[this->pos.y][this->pos.x] = this->itemRecord;
    //print the Item out
    cursorTo(this->pos.x, this->pos.y);
    switch(this->itemRecord)
    {
        case 2:
            SetConsoleTextAttribute(hConsole, 14);
            cout << "B";
            SetConsoleTextAttribute(hConsole, 15);
            break;
        case 3:
            SetConsoleTextAttribute(hConsole, 10);
            cout << "H";
            SetConsoleTextAttribute(hConsole, 15);
            break;
        case 4:
            SetConsoleTextAttribute(hConsole, 11);
            cout << "$";
            SetConsoleTextAttribute(hConsole, 15);
            break;
    }
    this->itemRecord = 0;
}
void Enemy::enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])//differ for every enemy
{
	
}
void Enemy::checkAndMove(Position checkPos, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	//get front ID
	int front = map[checkPos.y][checkPos.x];
	//then...
	if (front != 0)//front is not 0(blank)
	{
		if (front == -1)//front is player
		{
			attacked += this->ATK;
			this->HP = 0;
			score--;
		}
		else if (front >= 2 && front <= 4)//front is item
		{
			//record the itemType
			this->itemRecord = front;
	        //adjust enemy position
			this->pos = checkPos;
		    map[this->pos.y][this->pos.x] = enemyID;
		    cursorTo(this->pos.x, this->pos.y);//moving cursor to the new position
            SetConsoleTextAttribute(hConsole, 9);
            cout << "@";
            SetConsoleTextAttribute(hConsole, 15);
			//move and print the turned(or maybe we can just don't do this)
		}
	}
	else
	{
        //adjust enemy position
    	this->pos = checkPos;
        map[this->pos.y][this->pos.x] = enemyID;
        cursorTo(this->pos.x, this->pos.y);//moving cursor to the new position
        SetConsoleTextAttribute(hConsole, 9);
        cout << "@";
        SetConsoleTextAttribute(hConsole, 15);
	}
	return;
}
Position Enemy::getEnemyPos()
{
	return this->pos;
}
int Enemy::getEnemyHP()
{
	return this->HP;
}

//right  Clockwise
class EnemyClockwise : public Enemy
{
private:
	
public:
	//constructors
	EnemyClockwise();
	EnemyClockwise(Position p, int s, int h, int a, int d, int i);
	//copy constructor
	EnemyClockwise(const EnemyClockwise& e);
	//member function
	void enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
};
//constructor
EnemyClockwise::EnemyClockwise()
{
	
}
EnemyClockwise::EnemyClockwise(Position p, int s, int h, int a, int d, int i) : Enemy(p, s, h, a, d, i)
{
	
}
//copy constructor
EnemyClockwise::EnemyClockwise(const EnemyClockwise& e) : Enemy(e)
{
	
}
//memner function
void EnemyClockwise::enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	//if enemy has died
	if (this->HP == 0 || map[this->pos.y][this->pos.x] == -99 || map[this->pos.y][this->pos.x] == -1)
	{
		//if bumped by player
		if (map[this->pos.y][this->pos.x] == -1 && this->HP > 0)
		{
			attacked = this->ATK;
			score--;
			this->HP = 0;
		}
		else if (this->itemRecord != 0 && map[this->pos.y][this->pos.x] == -99)
		{
			printBackRecord(map);
            this->HP = 0;
		}
		else
		{
			this->HP = 0;
			map[this->pos.y][this->pos.x] = 0;
			cursorTo(this->pos.x, this->pos.y);
			cout << " ";
		}
		return;
	}
	//enemy move speed
	this->moveCnt += this->speed;
	//deciding move direction
	//clockwise
	Position checkPos = this->pos;
	switch(this->direction)
	{
		case 1://up
			checkPos = {this->pos.x, this->pos.y - 1};
			break;
		case 2://down
			checkPos = {this->pos.x, this->pos.y + 1};
			break;
		case 3://left
			checkPos = {this->pos.x - 1, this->pos.y};
			break;
		case 4://right
			checkPos = {this->pos.x + 1, this->pos.y};
	}
	//if front is not wall(any kind) or start/end point, change direction
	if (map[checkPos.y][checkPos.x] == 1 || map[checkPos.y][checkPos.x] == 5 || map[checkPos.y][checkPos.x] > 100)
	{
		switch(this->direction)
		{
			case 1:
				this->direction = 4;
				break;
			case 2:
				this->direction = 3;
				break;
			case 3:
				this->direction = 1;
				break;
			case 4:
				this->direction = 2;
		}
	}
	//if movable
	if (this->moveCnt >= moveThershold)
	{
		this->moveCnt = 0;
		if (this->itemRecord != 0)
		{
			printBackRecord(map);
		}
		else
		{
			//enemy disappear
			map[this->pos.y][this->pos.x] = 0;
			cursorTo(this->pos.x, this->pos.y);
			cout << " ";
		}
		checkAndMove(checkPos, map);
	}	
	return;
}

//counter
class EnemyCounterClockwise : public Enemy
{
private:
	
public:
	//constructors
	EnemyCounterClockwise();
	EnemyCounterClockwise(Position p, int s, int h, int a, int d, int i);
	//copy constructor
	EnemyCounterClockwise(const EnemyCounterClockwise& e);
	//member function
	void enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
};
//constructor
EnemyCounterClockwise::EnemyCounterClockwise()
{
	
}
EnemyCounterClockwise::EnemyCounterClockwise(Position p, int s, int h, int a, int d, int i) : Enemy(p, s, h, a, d, i)
{
	
}
//copy constructor
EnemyCounterClockwise::EnemyCounterClockwise(const EnemyCounterClockwise& e) : Enemy(e)
{
	
}
//memner function
void EnemyCounterClockwise::enemyMove(int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])
{
	//if enemy has died
	if (this->HP == 0 || map[this->pos.y][this->pos.x] == -99 || map[this->pos.y][this->pos.x] == -1)
	{
		//if bumped by player
		if (map[this->pos.y][this->pos.x] == -1 && this->HP > 0)
		{
			attacked = this->ATK;
			score--;
			this->HP = 0;
		}
		else if (this->itemRecord != 0 && map[this->pos.y][this->pos.x] == -99)
		{
			printBackRecord(map);
            this->HP = 0;
		}
		else
		{
			this->HP = 0;
			map[this->pos.y][this->pos.x] = 0;
			cursorTo(this->pos.x, this->pos.y);
			cout << " ";
		}
		return;
	}
	//enemy move speed
	this->moveCnt += this->speed;
	//deciding move direction
	//clockwise
	Position checkPos = this->pos;
	switch(this->direction)
	{
		case 1://up
			checkPos = {this->pos.x, this->pos.y - 1};
			break;
		case 2://down
			checkPos = {this->pos.x, this->pos.y + 1};
			break;
		case 3://left
			checkPos = {this->pos.x - 1, this->pos.y};
			break;
		case 4://right
			checkPos = {this->pos.x + 1, this->pos.y};
	}
	//if front is not wall(any kind) or start/end point, change direction
	if (map[checkPos.y][checkPos.x] == 1 || map[checkPos.y][checkPos.x] == 5 || map[checkPos.y][checkPos.x] > 100)
	{
		switch(this->direction)
		{
			case 1:
				this->direction = 3;
				break;
			case 2:
				this->direction = 4;
				break;
			case 3:
				this->direction = 2;
				break;
			case 4:
				this->direction = 1;
		}
	}
	//if movable
	if (this->moveCnt >= moveThershold)
	{
		this->moveCnt = 0;
		if (this->itemRecord != 0)
		{
			printBackRecord(map);
		}
		else
		{
			//enemy disappear
			map[this->pos.y][this->pos.x] = 0;
			cursorTo(this->pos.x, this->pos.y);
			cout << " ";
		}
		checkAndMove(checkPos, map);
	}	
	return;
}

//enemys
class EnemyTeam
{
private:
	int capacity;
	int cnt;
	Enemy** enemyPtr;
public:
	//constructors
	EnemyTeam();
	//copy constructor
//	EnemyTeam(const EnemyTeam& team);//might not need here
	//destructors
	~EnemyTeam();
	//member functions
	void addCEnemy(Position p, int s, int h, int a, int d, int i);
	void addCCEnemy(Position p, int s, int h, int a, int d, int i);
};
EnemyTeam::EnemyTeam()
{
	this->cnt = 0;
	this->capacity = 20000;
	this->enemyPtr = new Enemy*[this->capacity];
}
EnemyTeam::~EnemyTeam()
{
	for(int i = 0; i < this->cnt; i++)
		delete this->enemyPtr[i];
	delete [] this->enemyPtr;
	return;
}
void EnemyTeam::addCEnemy(Position p, int s, int h, int a, int d, int i)
{
	if (this->cnt < this->capacity)
	{
		this->enemyPtr[cnt] = new EnemyClockwise(p, s, h, a, d, i);
		cnt++;
		return;
	}
	return;
}
void EnemyTeam::addCCEnemy(Position p, int s, int h, int a, int d, int i)
{
	if (this->cnt < this->capacity)
	{
		this->enemyPtr[cnt] = new EnemyCounterClockwise(p, s, h, a, d, i);
		cnt++;
		return;
	}
	return;
}


//header 
void menu();
void mode();
void settings();
void printInfo();//game hint
int direction(Position from, Position to);
void printMap(int direction, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]);
void gameEnding();
void delayPrint(string s);

////////////////////////////////////////////////////   


int main()
{
////////settings////////////////////////////////////

	//threads
	// thread t1(playSound);
	
    //create map 
    int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH] = 
    {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    };    

	//character settings 
	//player
	Position spawnPos = {1, 18};
	Player player(spawnPos, 5, 6, 1);
	map[player.getPos().y][player.getPos().x] = -1;
	//enemy(use array)
	EnemyClockwise e({10, 10}, 20, 3, 1, 4, 101);
	EnemyCounterClockwise e2({1, 1}, 40, 3, 1, 3, 101);
	
	
	//game status
    bool gameRunning = true;
    
////////GAME START//////////////////////////////////

    // open menu
   menu();

    //enemy(use array)


	//print map and info
    system("cls");
    printMap(player.direction, map);
    //enemySpawn
    e.enemySpawn(map);
    player.printStatus(); 
    printInfo();
    cursorTo(player.getPos().x, player.getPos().y);

	//game resume
    while(gameRunning)
    {  
    	if (attacked > 0)
    	{
    		player.HPcost(attacked);
    		attacked = 0;
    		player.printStatus();
		}
		if (e.getEnemyHP() != 0)
		{
			e.enemyMove(map);
		}
		if (e2.getEnemyHP() != 0)
		{
			e2.enemyMove(map);
		}
        if(kbhit())
        {
            int ch = getch();

            if(ch == 'w')
            {
            	player.direction = 1;
            	Position checkPos = {player.getPos().x, player.getPos().y - 1};
            	player.checkAndMove(checkPos, map);
            }

            if(ch == 'a')
            {
                player.direction = 3;
            	Position checkPos = {player.getPos().x - 1, player.getPos().y};
            	player.checkAndMove(checkPos, map); 
            }

            if(ch == 's')
            {
                player.direction = 2;
            	Position checkPos = {player.getPos().x, player.getPos().y + 1};
            	player.checkAndMove(checkPos, map);
            }

            if(ch == 'd')
            {
                player.direction = 4;
            	Position checkPos = {player.getPos().x + 1, player.getPos().y};
            	player.checkAndMove(checkPos, map);      
            }                

            if(ch == 'e')
            {
                player.playerShoot(map); 
                player.printStatus();
                cursorTo(player.getPos().x, player.getPos().y);
            }
        
            if(ch == 'q')
            {
            	gameRunning = false;
                gameEnding();
            }
        }
        if (player.getPlayerHP() <= 0)
        {
        	gameRunning = false;
        	gameEnding();
		}
    }

	//t1.join();

    return 0;
}

//body

//maybe can change into Position pos
void cursorTo(int x, int y)
{
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    return;
}

void menu()
{
    system("cls");
    int position = 1;
    bool change = false;

	cursorTo(xStart - 13, yStart);
	cout << " _   _ _____ _____  _   _ ________  ___  ___  ______ _____ \n";
	Sleep(1000);
	cursorTo(xStart - 13, yStart + 1);
	cout << "| \\ | |_   _|  __ \\| | | |_   _|  \\/  | / _ \\ | ___ \\  ___| \n";
	Sleep(1000);
	cursorTo(xStart - 13, yStart + 2);
	cout << "|  \\| | | | | |  \\/| |_| | | | | .  . |/ /_\\ \\| |_/ / |__ \n";
	Sleep(1000);
	cursorTo(xStart - 13, yStart + 3);
	cout << "| . ` | | | | | __ |  _  | | | | |\\/| ||  _  ||    /|  __| \n";
	Sleep(1000);
	cursorTo(xStart - 13, yStart + 4);
	cout << "| |\\  |_| |_| |_\\ \\| | | | | | | |  | || | | || |\\ \\| |___ \n";
	Sleep(1000);
	cursorTo(xStart - 13, yStart + 5);
	cout << "\\_| \\_/\\___/ \\____/\\_| |_/ \\_/ \\_|  |_/\\_| |_/\\_| \\_\\____/ \n";
	Sleep(5000);

	system("cls");
                                                                                                       
    cursorTo(xStart, yStart);
    cout << "    --> PLAY <--" << "\n";   

    cursorTo(xStart, yStart + yGap);
    cout << "        MODE " << "\n";   

    cursorTo(xStart, yStart + yGap * 2);
    cout << "      SETTINGS" << "\n";           

    while(true)
    {
        if(kbhit())
        {
            int ch = getch();
			
			//recording
            if(ch == 'w' && position != 1)
            {
                position -= 1;
                change = true;
            }
            else if(ch == 's' && position != 3)
            {
                position += 1;
                change = true;
            }
			
			//selecttion
            if(ch == 32 && position == 1)
                return;
            else if(ch == 32 && position == 2)
                mode();
            else if(ch == 32 && position == 3)
                settings();
			
			//selecting
            if(position == 1 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "    --> PLAY <--" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "        MODE" << "\n"; 
                cursorTo(xStart, yStart + yGap * 2);
                cout << "      SETTINGS" << "\n";  
                bool change = false;
            }
            else if(position == 2 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "        PLAY" << "\n"; 
                cursorTo(xStart, yStart + yGap);
                cout << "    --> MODE <--" << "\n"; 
                cursorTo(xStart, yStart + yGap * 2);
                cout << "      SETTINGS" << "\n";       
                bool change = false;
           
            }
            else if(position == 3 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "        PLAY" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "        MODE" << "\n";   
                cursorTo(xStart, yStart + yGap * 2);
                cout << "  --> SETTINGS <--" << "\n";  
                bool change = false;
            }

        }

    }

}

void mode()
{
    system("cls");
    int position = 1;
    bool change = false;  
    
    if(modeNum == 1)
    {
        cursorTo(xStart, yStart);
        cout << "   --> *EASY* <--" << "\n";   
        cursorTo(xStart, yStart + yGap);
        cout << "        HARD " << "\n";  
        cursorTo(xStart, yStart + yGap * 2);   
        cout << "    BACK TO MENU " << "\n";    
    }
    else if(modeNum == 2)
    {
        position = 2;
        cursorTo(xStart, yStart);
        cout << "        EASY " << "\n";   
        cursorTo(xStart, yStart + yGap);
        cout << "   --> *HARD* <--" << "\n";  
        cursorTo(xStart, yStart + yGap * 2);   
        cout << "    BACK TO MENU " << "\n";            
    }
      
    while(true)
    {
        if(kbhit())
        {
            int ch = getch();

            if(ch == 'w' && position != 1)
            {
                position -= 1;
                change = true;
            }
            else if(ch == 's' && position != 3)
            {
                position += 1;
                change = true;
            }

            if(ch == 32 && position == 1)
            {
                modeNum = 1;
                change = true;
            }
            else if(ch == 32 && position == 2)
            {
                modeNum = 2;
                change = true;
            }
            else if(ch == 32 && position == 3)
            {
                return;
            }

            if(position == 1 && modeNum == 1 && change == true)
            {
                system("cls");
        		cursorTo(xStart, yStart);
                cout << "   --> *EASY* <--" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "        HARD" << "\n";  
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "    BACK TO MENU " << "\n";   
                bool change = false;
            }
            else if(position == 2 && modeNum == 1 && change == true)
            {
                system("cls");
        		cursorTo(xStart, yStart);
                cout << "       *EASY*" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "    --> HARD <--" << "\n";      
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "    BACK TO MENU " << "\n";
                bool change = false;
           
            }
            else if(position == 1 && modeNum == 2 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "    --> EASY <--" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "       *HARD*" << "\n";   
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "    BACK TO MENU " << "\n";
                bool change = false;
            }
            else if(position == 2 && modeNum == 2 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "        EASY"  << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "   --> *HARD* <--" << "\n";   
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "    BACK TO MENU " << "\n";
                bool change = false;
            }
            else if(position == 3 && modeNum == 1 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "       *EASY* " << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "        HARD" << "\n";  
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "--> BACK TO MENU <--" << "\n";   
                bool change = false;
            }
            else if(position == 3 && modeNum == 2 && change == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "        EASY"  << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "       *HARD*" << "\n";  
                cursorTo(xStart, yStart + yGap * 2);   
                cout << "--> BACK TO MENU <--" << "\n";    
                bool change = false;
            }            

        }

    }    
}

void settings()
{
    system("cls");

    int position = 1;
    bool change = false;
    bool mute = false;
    
    cursorTo(xStart, yStart);
    cout << "  --> SOUND ON <--" << "\n";   

    cursorTo(xStart, yStart + yGap);
    cout << "    BACK TO MENU " << "\n";           

    while(true)
    {

        if(kbhit())
        {
            int ch = getch();

            if(ch == 'w' && position != 1)
            {
                position -= 1;
                change = true;
            }
            else if(ch == 's' && position != 2)
            {
                position += 1;
                change = true;
            }

            if(ch == 32 && position == 1)
            {
                mute = !mute;
                change = true;
            }
            else if(ch == 32 && position == 2)
            {
                return;
            }

            
            if(position == 1 && change == true && mute == false)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "  --> SOUND ON <--" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "    BACK TO MENU" << "\n";     
                bool change = false;
            }
            else if(position == 2 && change == true && mute == false)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "      SOUND ON" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "--> BACK TO MENU <--" << "\n";        
                bool change = false;
            }
            else if(position == 1 && change == true && mute == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "  --> SOUND OFF <--" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "    BACK TO MENU" << "\n";     
                bool change = false;
            }
            else if(position == 2 && change == true && mute == true)
            {
                system("cls");
                cursorTo(xStart, yStart);
                cout << "      SOUND OFF" << "\n";   
                cursorTo(xStart, yStart + yGap);
                cout << "--> BACK TO MENU <--" << "\n";        
                bool change = false;
            }
        }
    }
}    

void shoot(Position startPos, Position dir, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH], int range)
{
	bool metItem = false;
    for(int i = 0; i < range; i++)
    {
    	Position temp = {0};
    	temp.x = startPos.x + dir.x;
    	temp.y = startPos.y + dir.y;
        //if front is blank
        if(map[temp.y][temp.x] == 0)
        {
        	startPos.x += dir.x;
            startPos.y += dir.y;
            cursorTo(startPos.x, startPos.y);
            cout << "o";
            if(i != 0 && !metItem)
            {
                Sleep(100);
                cursorTo(startPos.x - dir.x, startPos.y - dir.y);
                cout << " ";                   
            }
            metItem = false;
        }
        //if front is enemy
        else if (map[temp.y][temp.x] > 100)
        {
        	if (i == 0)
        	{
        		map[temp.y][temp.x] = -99;
			}
        	else if(i != 0)
			{
				if (!metItem)
				{
					cursorTo(startPos.x, startPos.y);
	        		cout << " ";
				}
	        	map[temp.y][temp.x] = -99;
			}
        	return;
		}
		else if (map[temp.y][temp.x] <= 4 && map[temp.y][temp.x] >= 2)
		{
			startPos.x += dir.x;
            startPos.y += dir.y;
            if(i != 0 && !metItem)
            {
                Sleep(100);
                cursorTo(startPos.x - dir.x, startPos.y - dir.y);
                cout << " ";                    
            }
            metItem = true;
		}
        //if front is wall
        else if(map[temp.y][temp.x] == 1)
            break;
        //if front is movable wall?
    }
    //if bullet didn't move
    if(map[startPos.y][startPos.x] != -1)
    {
        Sleep(100);
        cursorTo(startPos.x, startPos.y);
        cout << " ";                 
    }
    return;
}

void printInfo()
{
    SetConsoleTextAttribute(hConsole, 8);

    cursorTo(MAP_SIZE_WIDTH + 5, 5);
    cout << "MOVE UP: " << "W" << "\n";

    cursorTo(MAP_SIZE_WIDTH + 5, 6);
    cout << "MOVE LEFT: " << "A" << "\n";

    cursorTo(MAP_SIZE_WIDTH + 5, 7);
    cout << "MOVE DOWN: " << "S" << "\n";

    cursorTo(MAP_SIZE_WIDTH + 5, 8);
    cout << "MOVE RIGHT: " << "D" << "\n";

    cursorTo(MAP_SIZE_WIDTH + 5, 10);
    cout << "SHOOT: " << "E" << "\n"; 

    cursorTo(MAP_SIZE_WIDTH + 5, 12);
    cout << "QUIT: " << "Q" << "\n";          

    SetConsoleTextAttribute(hConsole, 15);

}

//no need now
int direction(Position from, Position to)//must be at different place
{
	if (from.x - to.x == 0)
	{
		//go up
		if (from.y - to.y > 0)
			return 1;
		//go down
		else if (from.y - to.y < 0)
			return 2;
	}
	else
	{
		//go left
		if (from.x - to.x > 0)
			return 3;
		//go right
		else if (from.x - to.x < 0)
			return 4;
	}
	return 0;
}

void printMap(int direction, int map[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH])//clean screen first? then I can use it in enemyMeetItem
{
    for(int i = 0; i < MAP_SIZE_HEIGHT; i++)
    {
        for(int j = 0; j < MAP_SIZE_WIDTH; j++)
        {
            if(map[i][j] == 0)
                cout << " ";
            else if(map[i][j] == 1)//wall
            {
                SetConsoleTextAttribute(hConsole, 15);
                cout << "#";
                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == 2)//bullet
            {
                SetConsoleTextAttribute(hConsole, 14);
                cout << "B";
                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == 3)//recover
            {   
                SetConsoleTextAttribute(hConsole, 10);
                cout << "H";
                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == 4)//score(money)
            {   
                SetConsoleTextAttribute(hConsole, 11);
                cout << "$";
                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == 5) //movable object
            {
                SetConsoleTextAttribute(hConsole, 8);
                cout << "#";
                SetConsoleTextAttribute(hConsole, 15);
            }
            else if(map[i][j] == -1)//character direction indication
            {
                SetConsoleTextAttribute(hConsole, 14);

                if(direction == 1)//up
                    cout << "^";
                else if(direction == 2)//down
                    cout << "v";
                else if(direction == 3)//left
                    cout << "<";
                else if(direction == 4)//right
                    cout << ">";

                SetConsoleTextAttribute(hConsole, 15);

            }
        }
        cout << "\n";
    }    
}
void gameEnding()
{
	clock_t endTime = clock();
    system("cls");
    cout << "GAME OVER!" << "\n" ;
    cout << "YOUR SCORE: " << score << "\n";
    cout << "PLAYING TIME: " << endTime / CLOCKS_PER_SEC << "s" << "\n";
	return;
}
void delayPrint(string s)
{
	system("cls");
	int i = 0, timeCnt = 0;
	while(i < s.length())
	{
		//if print white space(unsure)
		if (s[i] == ' ')
		{
			cout << s[i];
			i++;
			continue;
		}
		//skip
		if (kbhit())
		{
			int ch = getch();
			if (ch == 32)
			{
				system("cls");
				cout << s;
				Sleep(1000);
				break;
			}
		}
		//wait
		if (timeCnt < 10000)
		{
			timeCnt++;
			continue;
		}
		//print
		cout << s[i];
		timeCnt = 0;
		//last print
		if (i == s.length() - 1)
		{
			Sleep(500);
		}
		i++;
	}
	cout << "\n" << "\n";
	cout << "                            press Space to continue...";
	while(true)
	{
		if (kbhit())
		{
			int ch = getch();
			if (ch == 32)
			{
				break;
			}
		}
	}
	return;
}













