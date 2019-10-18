#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
  public:
    GameImpl(int nRows, int nCols);
	~GameImpl();
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);

private:
	int m_rows;
	int m_cols;

	struct ship
	{
		int length;
		char symbol;
		string name;
		ship* next;
	};

	ship* head;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
	m_rows = nRows;
	m_cols = nCols;
	head = nullptr;
    // This compiles but may not be correct
}

GameImpl::~GameImpl()
{
	if (head != nullptr)
	{
		ship* ptr = head;
		ship* temp = nullptr;
		while (ptr->next != nullptr)
		{
			temp = ptr->next;
			delete ptr;
			ptr = temp;
		}
		delete ptr;
	}
}

int GameImpl::rows() const
{
    return m_rows;  // This compiles but may not be correct
}

int GameImpl::cols() const
{
    return m_cols;  // This compiles but may not be correct
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
	//Length, symbol checked in Game

	ship* newShip = new ship{ length, symbol, name, nullptr };

	ship* temp = head;
	if (temp != nullptr)
	{
		for (temp; temp->next != nullptr; temp = temp->next)
			;
		temp->next = newShip;
	}
	else
		head = newShip;

    return true;  // This compiles but may not be correct
}

int GameImpl::nShips() const
{
	int a = 0;
	ship* temp = head;
	for (a; temp != nullptr; a++)
	{
		temp = temp->next;
	}
	return a;
	// This compiles but may not be correct
}

int GameImpl::shipLength(int shipId) const
{
	if (shipId >= nShips() || shipId < 0)
		return -1;

	ship* temp = head;

	for (int a = 0; a < shipId; a++)
	{
		temp = temp->next;
	}
	return temp->length;  
	// This compiles but may not be correct
}

char GameImpl::shipSymbol(int shipId) const
{
//	if (shipId >= nShips() || shipId < 0)
//		return -1;

	ship* temp = head;

	for (int a = 0; a < shipId; a++)
	{
		temp = temp->next;
	}
	return temp->symbol;
	// This compiles but may not be correct
}

string GameImpl::shipName(int shipId) const
{
//	if (shipId >= nShips() || shipId < 0)
//		return -1;

	ship* temp = head;

	for (int a = 0; a < shipId; a++)
	{
		temp = temp->next;
	}
	return temp->name;
	// This compiles but may not be correct
}

//Helper Function
void pause(bool sp)
{
	if (sp)
		waitForEnter();
}

void coordisplay(Point p)
{
	cout << "(" << p.r << "," << p.c << ")";
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
	//Place ships for 2 players, if one fails, return nullptr
	if (!p1->placeShips(b1) || !p2->placeShips(b2))
	{
		cerr << "placeShipError" << endl;
		return nullptr;
	}

	//Game starts
	//Need revise
	bool p1shotHit = false;
	bool p1shipDestroyed = false;
	bool p2shotHit = false;
	bool p2shipDestroyed = false;
	int p1ID = -1;
	int p2ID = -1;
	Point pt1;
	Point pt2;

	while (!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
	{
		cout << p1->name() << "'s turn.  Board for " << p2->name() << endl;
		b2.display(p1->isHuman());
		//If invalid point
		pt1 = p1->recommendAttack();
		if (!b2.attack(pt1, p1shotHit, p1shipDestroyed, p1ID))
		{
			p1->recordAttackResult(pt1, false, false, false, p1ID);
			p2->recordAttackByOpponent(pt1);
			cout << p1->name() << " wasted a shot at ";
			coordisplay(pt1);
			cout << "." << endl;
		}
		else
		{
			cerr << p1->name() << " attacked at ";
			cerr << "(" << pt1.r << "," << pt1.c << ")";
			cerr << "." << endl;
			p1->recordAttackResult(pt1, true, p1shotHit, p1shipDestroyed, p1ID);
			p2->recordAttackByOpponent(pt1);
			//			cerr << "recordAttackResult fn exits successfully" << endl;
			if (p1shotHit)
			{
				cout << p1->name() << " attacked ";
				coordisplay(pt1);
				cout << " and "; 
				if (p1shipDestroyed)
					cout << "destroyed the " << shipName(p1ID);
				else 
					cout << "hit something";
				
				cout << ", resulting in : " << endl;
				b2.display(p1->isHuman());
			}
			else
			{
				cout << p1->name() << " attacked "; 
				coordisplay(pt1);
				cout << " and missed, resulting in:" << endl;
				b2.display(p1->isHuman());
			}
		}

		//if all ships destroyed for p1, end game early
		if (b2.allShipsDestroyed())
		{
			cout << p1->name() << " wins!" << endl;
			return p1;
		}

		pause(shouldPause);

		cout << p2->name() << "'s turn.  Board for " << p1->name() << endl;
		b1.display(p2->isHuman());
		//If invalid point
		pt2 = p2->recommendAttack();
		if (!b1.attack(pt2, p2shotHit, p2shipDestroyed, p2ID))
		{
			p2->recordAttackResult(pt2, false, false, false, p2ID);
			p1->recordAttackByOpponent(pt2);
			cout << p2->name() << " wasted a shot at ";
			coordisplay(pt2);
			cout << "." << endl;
		}
		else
		{
			cerr << p2->name() << " attacked at ";
			cerr << "(" << pt2.r << "," << pt2.c << ")";
			cerr << "." << endl;
			p2->recordAttackResult(pt2, true, p2shotHit, p2shipDestroyed, p2ID);
			p1->recordAttackByOpponent(pt2);
			if (p2shotHit)
			{
				cout << p2->name() << " attacked ";
				coordisplay(pt2);
				cout << " and ";
				if (p2shipDestroyed)
					cout << "destroyed the " << shipName(p2ID);
				else
					cout << "hit something";

				cout << ", resulting in : " << endl;
				b1.display(p2->isHuman());
			}
			else
			{
				cout << p2->name() << " attacked ";
				coordisplay(pt2);
				cout << " and missed, resulting in:" << endl;
				b1.display(p2->isHuman());
			}
		}


		if (b1.allShipsDestroyed())
		{
			cout << p2->name() << " wins!" << endl;
			return p2;
		}


		pause(shouldPause);
	}

	//It should never reach here,the following codes are meaningless
    return nullptr;  // This compiles but may not be correct
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

