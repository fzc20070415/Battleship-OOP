#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;


class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
      // TODO:  Decide what private members you need.  Here's one that's likely
      //        to be useful:
    const Game& m_game;
	char m_coord[MAXCOLS][MAXROWS];
	bool m_block[MAXCOLS][MAXROWS];
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
	clear();			
    // This compiles, but may not be correct
}

void BoardImpl::clear()
{
	for (int a = 0; a < m_game.rows(); a++)
	{
		for (int b = 0; b < m_game.cols(); b++)
		{
			m_coord[a][b] = '.';
			m_block[a][b] = false;
		}
	}
    // This compiles, but may not be correct
}

void BoardImpl::block()
{
      // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (randInt(2) == 0)
            {
                // TODO:  Replace this with code to block cell (r,c)
				m_block[r][c] = true;
            }

	///////////////////////////////////////////////////////////////////////////////////////For Testing Purposes Only
	/*
	//cout first line
	cout << "  ";
	for (int a = 0; a < m_game.cols(); a++)
		cout << a;
	cout << endl;

	//cout the rest
	for (int a = 0; a < m_game.rows(); a++)
	{
		cout << a << " ";
		for (int b = 0; b < m_game.cols(); b++)
		{
			cout << m_block[a][b];
		}
		cout << endl;
	}
	*/
	//////////////////////////////////////////////////////////////////////////////////////////For Tesing Purposes Only
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
			m_block[r][c] = false; // TODO:  Replace this with code to unblock cell (r,c) if blocked
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	if (shipId < 0 || shipId >= m_game.nShips())		//if invalid shipID
	{
		cerr << "Invalid shipID" << endl;
		return false;
	}

	if (topOrLeft.r >= m_game.rows() || topOrLeft.c >= m_game.cols())	//if invalid point (fully outside the board)
	{
		cerr << "Invalid Coord" << endl;
		return false;
	}

	//if partially outside the board
	if (dir == HORIZONTAL)
		if ((topOrLeft.c + m_game.shipLength(shipId)) > m_game.cols())
		{
			cerr << "Partially outside the board" << endl;
			return false;
		}
	if (dir == VERTICAL)
		if ((topOrLeft.r + m_game.shipLength(shipId)) > m_game.rows())
		{
			cerr << "Partially outside the board" << endl;
			return false;
		}

	//if overlay or coordinate blocked
	for (int a = 0; a < m_game.shipLength(shipId); a++)
	{
		if (dir == HORIZONTAL)
		{
			if (m_coord[topOrLeft.r][topOrLeft.c + a] != '.' || m_block[topOrLeft.r][topOrLeft.c + a] == true)
			{
				cerr << "Overlay or Blocked" << endl;
				return false;
			}
		}
		if (dir == VERTICAL)
		{
			if (m_coord[topOrLeft.r + a][topOrLeft.c] != '.' || m_block[topOrLeft.r + a][topOrLeft.c] == true)
			{
				cerr << "Overlay or Blocked" << endl;
				return false;
			}
		}
	}

	//if this ship symbol already exists (this ship has been placed)
	for (int a = 0; a < m_game.rows(); a++)
	{
		for (int b = 0; b < m_game.cols(); b++)
		{
			if (m_coord[a][b] == m_game.shipSymbol(shipId))
			{
				cerr << "Ship symbol already exists (this ship has been placed)" << endl;
				return false;
			}
		}
	}

	//Add the ship
	for (int a = 0; a < m_game.shipLength(shipId); a++)
	{
		if (dir == HORIZONTAL)
		{
			m_coord[topOrLeft.r][topOrLeft.c + a] = m_game.shipSymbol(shipId);
		}
		if (dir == VERTICAL)
		{
			m_coord[topOrLeft.r + a][topOrLeft.c] = m_game.shipSymbol(shipId);
		}
	}

	return true; // This compiles, but may not be correct
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	if (shipId < 0 || shipId >= m_game.nShips())		//if invalid shipID
		return false;

	//if no such ship on this coord
	if (m_coord[topOrLeft.r][topOrLeft.c] != m_game.shipSymbol(shipId))
		return false;

	//if this coord is not topOrLeft of this ship
	if ((topOrLeft.r != 0 && m_coord[topOrLeft.r - 1][topOrLeft.c] == m_game.shipSymbol(shipId))
		|| (topOrLeft.c != 0 && m_coord[topOrLeft.r][topOrLeft.c - 1] == m_game.shipSymbol(shipId)))
		return false;

	//Remove the ship
	for (int a = 0; a < m_game.shipLength(shipId); a++)
	{
		if (dir == HORIZONTAL)
		{
//			assert(m_coord[topOrLeft.r][topOrLeft.c + a] == m_game.shipSymbol(shipId));
			m_coord[topOrLeft.r][topOrLeft.c + a] = '.';
		}
		if (dir == VERTICAL)
		{
//			assert(m_coord[topOrLeft.r + a][topOrLeft.c] == m_game.shipSymbol(shipId));
			m_coord[topOrLeft.r + a][topOrLeft.c] = '.';
		}
	}

	return true; // This compiles, but may not be correct
}

void BoardImpl::display(bool shotsOnly) const
{
	//cout first line
	cout << "  ";
	for (int a = 0; a < m_game.cols(); a++)
		cout << a;
	cout << endl;

	//cout the rest
	for (int a = 0; a < m_game.rows(); a++)
	{
		cout << a << " ";
		for (int b = 0; b < m_game.cols(); b++)
		{
			if (shotsOnly == false)				//Show all
				cout << m_coord[a][b];
			else								//Show only .oX
			{
				if (m_coord[a][b] == 'X')
					cout << 'X';
				else if (m_coord[a][b] == 'o')
					cout << 'o';
				else
					cout << '.';
			}
		}
		cout << endl;	
	}

    // This compiles, but may not be correct
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
	//if invalid point (outside the board && on previously attacked coords)
	if (p.r < 0 || p.c < 0 || p.r >= m_game.rows() || p.c >= m_game.cols() || m_coord[p.r][p.c] == 'o' || m_coord[p.r][p.c] == 'X')	
		return false;

	//if missed
	if (m_coord[p.r][p.c] == '.')
	{
		m_coord[p.r][p.c] = 'o';
		shotHit = false;
	}
	//if hit
	else
	{
		char temp = m_coord[p.r][p.c];
		m_coord[p.r][p.c] = 'X';
		shotHit = true;

		//check if this ship symbol no longer exists (this ship has been destroyed)
		shipDestroyed = true;
		for (int a = 0; a < m_game.rows(); a++)
		{
			for (int b = 0; b < m_game.cols(); b++)
			{
				if (m_coord[a][b] == temp)
				{
					shipDestroyed = false;
					break;
				}
			}
		}
		//if totally destroyed
		if (shipDestroyed == true)
		for (int a = 0; a < m_game.nShips(); a++)
		{
			if (m_game.shipSymbol(a) == temp)
			{
				shipId = a;
				break;
			}
		}
	}

    return true; 
	// This compiles, but may not be correct
}

bool BoardImpl::allShipsDestroyed() const
{
	for (int a = 0; a < m_game.rows(); a++)
	{
		for (int b = 0; b < m_game.cols(); b++)
		{
			if (m_coord[a][b] != '.' && m_coord[a][b] != 'o' && m_coord[a][b] != 'X')
				return false;
		}
	}

    return true; 
	// This compiles, but may not be correct
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
