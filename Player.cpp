#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>


using namespace std;



//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
//	b.display(0);
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.
//typedef AwfulPlayer HumanPlayer;

class HumanPlayer : public Player
{
public:
	HumanPlayer(string nm, const Game& g);
	virtual bool isHuman() const { return true; }		//Test purposes
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
};

HumanPlayer::HumanPlayer(string nm, const Game& g)
	: Player(nm, g)
{}

bool HumanPlayer::placeShips(Board& b)
{
	//Initial Display
	cout << name() << " must place " << game().nShips() << " ships." << endl;
	b.display(false);

	for (int ID = 0; ID < game().nShips(); ID++)
	{
		//Check if ship can fit
		if (game().shipLength(ID) > game().cols() && game().shipLength(ID) > game().rows())
			return false;

		//Get input for direction
		string direction;
		cout << "Enter h or v for direction of "
			<< game().shipName(ID) << " (length " << game().shipLength(ID) << "): ";
		getline(cin, direction);
		int a;

		for (a = direction.length() - 1; a >= 0 && direction[a] == ' '; a--)
			;
		direction = direction.substr(0, a + 1);			//Review this part

		while (direction != "h" && direction != "v")
		{
			cout << "Direction must be  h or v" << endl;
			cout << "Enter h or v for direction of "
				<< game().shipName(ID) << " (length " << game().shipLength(ID) << "): ";
			getline(cin, direction);
			for (a = direction.length() - 1; a >= 0 && direction[a] == ' '; a--)
				;
			direction = direction.substr(0, a + 1);			//Review this part
		//	cerr << direction << '.' << endl;
		}
		Direction drt;
		if (direction == "h")
			drt = HORIZONTAL;
		if (direction == "v")
			drt = VERTICAL;

		//Get input for topOrLeft coord
		int r = -1;
		int c = -1;
		cout << "Enter row and column of leftmost cell(e.g. 3 5) : ";
		while (!getLineWithTwoIntegers(r, c))
		{
			cout << "You must enter two integers." << endl;
			cout << "Enter row and column of leftmost cell(e.g. 3 5) : ";
		}
		Point* tol = new Point(r, c);
		//cerr << "Coord Obtained" << endl;

		//Place Aircraft Carrier (Length 5) and display
		while (!b.placeShip(*tol, ID, drt))
		{
			cout << "The ship can not be placed there." << endl;
			cout << "Enter row and column of leftmost cell(e.g. 3 5) : ";
			while (!getLineWithTwoIntegers(r, c))
			{
				cout << "You must enter two integers." << endl;
				cout << "Enter row and column of leftmost cell(e.g. 3 5) : ";
			}
			delete tol;
			tol = new Point(r, c);
		}
		delete tol;
		b.display(false);
	}
	return true;
}

Point HumanPlayer::recommendAttack()
{
	int r;
	int c;
cout << "Enter the row and column to attack (e.g, 3 5): ";
while (!getLineWithTwoIntegers(r, c))
{
	cout << "You must enter two integers." << endl;
	cout << "Enter row and column to attack(e.g. 3 5): ";
}
Point p(r, c);
return p;
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
	bool shipDestroyed, int shipId)
{}

void HumanPlayer::recordAttackByOpponent(Point p)
{}



//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class MediocrePlayer : public Player
{
public:
	MediocrePlayer(string nm, const Game& g);
	virtual ~MediocrePlayer();
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);

private:
	//Use in placeShips:
	bool blockindicator;
	bool unplaceindicator;
	struct shipList
	{
		int ID;
		int length;
		char symbol;
		string name;
		shipList* next;
		shipList* prev;
		int row;
		int col;
		Direction dir;
	};

	shipList* head;

	int placeshipcount;

	//Use in Attack fn:
	struct Pt
	{
		int r;
		int c;
		Pt* next;
	};
	Pt* headPtr;		//No need delete

	Point* hit;			//deleted in function
	Point* recmPt;		//deleted in function
	bool overwritePt;
};

MediocrePlayer::~MediocrePlayer()
{
	//Delete shipList Linked List
	shipList* temp = head;
	if (temp != nullptr)
	while (temp != nullptr && temp->next != nullptr)
		temp = temp->next;
	if (temp != nullptr)
	{
		while (temp != nullptr && temp->prev != nullptr)
		{
			temp = temp->prev;
			delete temp->next;
		}
		delete temp;
	}

	//Delete Pt Linked List
	Pt* tem = headPtr;
	Pt* tem2 = headPtr->next;
	for (; tem2 != nullptr; tem2 = tem2->next)
	{
		delete tem;
		tem = tem2;
	}
	delete tem;

	if (overwritePt)
	{
		delete recmPt;
		delete hit;
	}
}

MediocrePlayer::MediocrePlayer(string nm, const Game& g) : Player(nm, g)
{
	placeshipcount = 0;
	blockindicator = false;
	unplaceindicator = false;
	head = nullptr;
	shipList* ptr_prev = nullptr;
	int a;
	for ( a = 0; a < game().nShips(); a++)	//Copy the ship info
	{
		if (head == nullptr)
		{
			head = new shipList;
			head->ID = a;
			head->length = game().shipLength(a);
			head->name = game().shipName(a);
			head->symbol = game().shipSymbol(a);
			head->prev = nullptr;
			head->next = nullptr;
			head->row = 0;
			head->col = 0;
			head->dir = HORIZONTAL;
			ptr_prev = head;
		}
		else
		{
			shipList* newship = new shipList;
			newship->ID = a;
			newship->length = game().shipLength(a);
			newship->name = game().shipName(a);
			newship->symbol = game().shipSymbol(a);
			newship->prev = ptr_prev;
			newship->next = nullptr;
			newship->col = 0;
			newship->row = 0;
			newship->dir = HORIZONTAL;
			ptr_prev->next = newship;
			ptr_prev = ptr_prev->next;
		}
	}
	shipList* newship = new shipList;
	newship->ID = -1;
	newship->name = "Ending Node";
	newship->prev = ptr_prev;
	newship->next = nullptr;
	ptr_prev->next = newship;


	headPtr = nullptr;
	headPtr = new Pt;
	headPtr->next = nullptr;

	recmPt = nullptr;
	overwritePt = false;
	hit = nullptr;
}

bool MediocrePlayer::placeShips(Board& b)
{
	if (placeshipcount >= 50)			//Return false if reset recursion more than 50 times
	{
		cerr << "Over 50 attemps" << endl;
		return false;
	}


	if (!blockindicator)			//Check if already blocked
	{
		b.block();
		blockindicator = true;
	}

	if (head->next != nullptr)	//Check if already recur to the last ship;
	{
		Point pt(head->row, head->col);
		if ( !unplaceindicator && b.placeShip(pt, head->ID, head->dir))		//If successfully placed
		{
			head = head->next;
		}
		else											//If cannot place at this point
		{
			unplaceindicator = false;
			//change direction
			if (head->dir == HORIZONTAL)
				head->dir = VERTICAL;
			//change a point
			else if (head->row < game().rows()-1)		//Add the row number if possible
			{
				head->dir = HORIZONTAL;
				head->row++;
			}
			else if (head->col < game().cols()-1)		//If row reaches max, reset row and change col
			{
				head->dir = HORIZONTAL;
				head->row = 0;
				head->col++;
			}
			else										//All points are not possible, rollback
			{
				if (head->prev == nullptr)			//If the shipID 0 cannot be placed, go back step1, unblock & block again
				{
					placeshipcount++;			//Add count
					cerr << "placeshipcount is now " << placeshipcount << endl;
					b.unblock();
				//Reset shipList
					shipList* temp = head;
					while (temp->next != nullptr)
						temp = temp->next;
					if (temp != nullptr)
					{
						while (temp != nullptr && temp->prev != nullptr)
						{
							temp = temp->prev;
							delete temp->next;
						}
						delete temp;
					}

				//Initiate everything & copy shipList again
					blockindicator = false;
					unplaceindicator = false;
					head = nullptr;
					shipList* ptr_prev = nullptr;
					for (int a = 0; a < game().nShips(); a++)	//Copy the ship info
					{
						if (head == nullptr)
						{
							head = new shipList;
							head->ID = a;
							head->length = game().shipLength(a);
							head->name = game().shipName(a);
							head->symbol = game().shipSymbol(a);
							head->prev = nullptr;
							head->next = nullptr;
							head->row = 0;
							head->col = 0;
							head->dir = HORIZONTAL;
							ptr_prev = head;
						}
						else
						{
							shipList* newship = new shipList;
							newship->ID = a;
							newship->length = game().shipLength(a);
							newship->name = game().shipName(a);
							newship->symbol = game().shipSymbol(a);
							newship->prev = ptr_prev;
							newship->next = nullptr;
							newship->col = 0;
							newship->row = 0;
							newship->dir = HORIZONTAL;
							ptr_prev->next = newship;
							ptr_prev = ptr_prev->next;
						}
					}
					shipList* newship = new shipList;
					newship->ID = -1;
					newship->name = "Ending Node";
					newship->prev = ptr_prev;
					newship->next = nullptr;
					ptr_prev->next = newship;

				}
				else
				{
					//unplace, trace back
					head = head->prev;
					Point pt2(head->row, head->col);
					b.unplaceShip(pt2, head->ID, head->dir);
					unplaceindicator = true;
				}
			}
		}
	}
	else			//All ships are placed
	{
		b.unblock();		//Unblock
		return true;
	}

	if (!placeShips(b))			//Recursion
	{
		return false;
	}
	else
		return true;
}

Point MediocrePlayer::recommendAttack()
{
	if (overwritePt)
	{
		cerr << "overwrite is true" << endl;
		Pt* newPt = new Pt;
		newPt->r = recmPt->r;
		newPt->c = recmPt->c;
		newPt->next = nullptr;
		Pt* temp;
		for (temp = headPtr; temp->next != nullptr; temp = temp->next)
			;
		temp->next = newPt;
		cerr << "recmPt (" << recmPt->r << "," << recmPt->c << "), fn recommendAttack exits here" << endl;
		return *recmPt;
	}

	Point p(randInt(game().rows()), randInt(game().cols()));
	Point* pp = &p;
	//Check if this point is in the linked list
	for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
	{
		if (pp->c == pt->c && pp->r == pt->r)
		{
			Point px(randInt(game().rows()), randInt(game().cols()));
			pp = &px;
			pt = headPtr;
		}
	}

	//Add this point into the linked list (dummy node in singly linked list)
	Pt* newPt = new Pt;
	newPt->r = pp->r;
	newPt->c = pp->c;
	newPt->next = nullptr;
	Pt* temp;
	for (temp = headPtr; temp->next != nullptr; temp = temp->next)
		;
	temp->next = newPt;

	return *pp;

}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
	bool shipDestroyed, int shipId)
{
	if (!overwritePt && validShot && shotHit && !shipDestroyed)
	{
		hit = new Point(p.r, p.c);
		Point step2p(p.r, p.c);
		Point* s2pp = &step2p;
		//Check if this point is in the linked list
		for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
		{
			if (s2pp->c == pt->c && s2pp->r == pt->r)
			{
				cerr << "point exists" << endl;
				int count = 0;
				step2p.r = -1;
				step2p.c = -1;

				while (!game().isValid(step2p) || count >= 50)
				{
					if (randInt(2) == 0)
					{
						step2p.r = hit->r - 4 + randInt(9);
						step2p.c = hit->c;
						count++;
					}
					else
					{
						step2p.r = hit->r;
						step2p.c = hit->c - 4 + randInt(9);
						count++;
					}
				}
				cerr << "recmPt generated is (" << step2p.r << "," << step2p.c << ")." << endl;
				s2pp = &step2p;
				pt = headPtr;
			}
		}
		recmPt = new Point(s2pp->r, s2pp->c);
		overwritePt = true;
	}
	else if (overwritePt && !shipDestroyed)
	{
		Point step2p(p.r, p.c);
		Point* s2pp = &step2p;
		//Check if this point is in the linked list
		for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
		{
			if (s2pp->c == pt->c && s2pp->r == pt->r)
			{
				cerr << "point exists" << endl;
				int count = 0;
				step2p.r = -1;
				step2p.c = -1;

				while (!game().isValid(step2p) || count >= 50)
				{
					if (randInt(2) == 0)
					{
						step2p.r = hit->r - 4 + randInt(9);
						step2p.c = hit->c;
						count++;
					}
					else
					{
						step2p.r = hit->r;
						step2p.c = hit->c - 4 + randInt(9);
						count++;
					}
				}
				cerr << "recmPt generated is (" << step2p.r << "," << step2p.c << ")." << endl;
				s2pp = &step2p;
				pt = headPtr;
			}
		}
		delete recmPt;
		recmPt = new Point(s2pp->r, s2pp->c);
	}
	if (overwritePt && shipDestroyed)
	{
		delete hit;
		delete recmPt;
		overwritePt = false;
		cerr << "shipDestroyed is true so pointers deleted" << endl;
		//for checking purposes: (delete later)
		recmPt = nullptr;
	}
	if (recmPt != nullptr)
	{
		cerr << "recmPt returned at (" << recmPt->r << "," << recmPt->c << ")." << endl;
		cerr << "hit coord at (" << hit->r << "," << hit->c << ")." << endl;
	}
}

void MediocrePlayer::recordAttackByOpponent(Point p)
{

}

// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class GoodPlayer : public Player
{
public:
	GoodPlayer(string nm, const Game& g);
	virtual ~GoodPlayer();
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);

private:
	//helper functions:
//	void clearShipCoordList();
	bool checkPtAlrExist(int r, int c);

	struct Pt
	{
		int r;
		int c;
		Pt* next;
	};


	//Use in placeShips:
	//bool blockindicator;
	//bool unplaceindicator;
	//struct shipList
	//{
	//	int ID;
	//	int length;
	//	char symbol;
	//	string name;
	//	shipList* next;
	//	shipList* prev;
	//	Pt* shipCoord;
	//};

	//struct LastShipPlaced
	//{
	//	int ID;
	//	int r;
	//	int c;
	//	Direction d;
	//	LastShipPlaced* next;
	//};

	//LastShipPlaced* lsp;

	//int count;

	//shipList* head;

	//int placeshipcount;

	//Use in Attack fn:
	Pt* headPtr;		//No need delete
	Point* hit;			//deleted in function
	Point* hit2;
	Point* hit3;
	Point* hit4;
	Point* hit5;
	Point* hit6;
	Point* recmPt;		//deleted in function
	bool overwritePt;
};

//helper function:
//void GoodPlayer::clearShipCoordList()
//{
//	Pt* temp = shipCoord;
//	Pt* temp2 = shipCoord;
//	while (temp != nullptr)
//	{
//		temp = temp->next;
//		delete temp2;
//		temp2 = temp;
//	}
//}

GoodPlayer::~GoodPlayer()
{
	//Delete shipList Linked List
	//shipList* temp = head;
	//while (temp != nullptr && temp->next != nullptr)
	//	temp = temp->next;
	//temp = temp->prev;
	//delete temp->next;	//delete the indicator node

	//if (temp != nullptr)
	//{
	//	while (temp->prev != nullptr)
	//	{
	//		temp = temp->prev;

	//		Pt* tem = temp->next->shipCoord;
	//		Pt* tem2 = temp->next->shipCoord->next;
	//		for (; tem2 != nullptr; tem2 = tem2->next)
	//		{
	//			delete tem;
	//			tem = tem2;
	//		}
	//		delete tem;

	//		delete temp->next;
	//	}
	//	Pt* tem = temp->shipCoord;
	//	Pt* tem2 = temp->shipCoord->next;
	//	for (; tem2 != nullptr; tem2 = tem2->next)
	//	{
	//		delete tem;
	//		tem = tem2;
	//	}
	//	delete temp;
	//}

	////Delete LSP
	//LastShipPlaced* templ = lsp;
	//LastShipPlaced* templ2 = lsp;
	//while ( templ2 != nullptr && templ2->next != nullptr)
	//{
	//	templ2 = templ2->next;
	//	delete templ;
	//	templ = templ2;
	//}
	//if(templ != nullptr)
	//	delete templ;


	//Delete Pt Linked List
	Pt* tem = headPtr;
	Pt* tem2 = headPtr->next;
	for (; tem2 != nullptr; tem2 = tem2->next)
	{
		delete tem;
		tem = tem2;
	}
	delete tem;

	if (overwritePt)
	{
		delete recmPt;
		delete hit;
		delete hit2;
		delete hit3;
		delete hit4;
		delete hit5;
		delete hit6;
	}




	//The following codes are added when placeship is using new codes:

}

GoodPlayer::GoodPlayer(string nm, const Game& g) : Player(nm, g)
{
	//placeshipcount = 0;
	//blockindicator = false;
	//unplaceindicator = false;
	//head = nullptr;
	//shipList* ptr_prev = nullptr;
	//int a;
	//for (a = 0; a < game().nShips(); a++)	//Copy the ship info
	//{
	//	if (head == nullptr)
	//	{
	//		head = new shipList;
	//		head->ID = a;
	//		head->length = game().shipLength(a);
	//		head->name = game().shipName(a);
	//		head->symbol = game().shipSymbol(a);
	//		head->prev = nullptr;
	//		head->next = nullptr;
	//		ptr_prev = head;
	//		head->shipCoord = nullptr;
	//		head->shipCoord = new Pt;
	//		head->shipCoord->next = nullptr;
	//	}
	//	else
	//	{
	//		shipList* newship = new shipList;
	//		newship->ID = a;
	//		newship->length = game().shipLength(a);
	//		newship->name = game().shipName(a);
	//		newship->symbol = game().shipSymbol(a);
	//		newship->prev = ptr_prev;
	//		newship->next = nullptr;
	//		ptr_prev->next = newship;
	//		ptr_prev = ptr_prev->next;
	//		newship->shipCoord = nullptr;
	//		newship->shipCoord = new Pt;
	//		newship->shipCoord->next = nullptr;
	//	}
	//}
	//shipList* newship = new shipList;
	//newship->ID = -1;
	//newship->name = "Ending Node";
	//newship->prev = ptr_prev;
	//newship->next = nullptr;
	//if (ptr_prev != nullptr)
	//	ptr_prev->next = newship;
	//else
	//	head = newship;


	headPtr = nullptr;
	headPtr = new Pt;
	headPtr->next = nullptr;

	recmPt = nullptr;
	overwritePt = false;
	hit = nullptr;
	hit2 = nullptr;
	hit3 = nullptr;
	hit4 = nullptr;
	hit5 = nullptr;
	hit6 = nullptr;

//	lsp = nullptr;




	//Codes added:
//	shipCoord = nullptr;
}

bool GoodPlayer::checkPtAlrExist(int r, int c)
{
	Pt* temp = headPtr;
	for (temp; temp != nullptr; temp = temp->next)
	{
		if (r == temp->r && c == temp->c)
			return true;
	}
	return false;
}

bool GoodPlayer::placeShips(Board& b)
{
	//	if (placeshipcount >= 60)			//Return false if reset recursion more than 50 times
	//	{
	//		cerr << "Over 50 attemps" << endl;
	//		return false;
	//	}
	//
	//	if (!blockindicator)			//Check if already blocked
	//	{
	//		b.block();
	//		blockindicator = true;
	//	}
	//	
	//	Direction di = HORIZONTAL;
	//
	//	Point rdm(randInt(game().rows()), randInt(game().cols()));
	//
	//	if (head->next != nullptr)	//Check if already recur to the last ship;
	//	{
	//		//Check if this point is in the linked list
	//		for (Pt* pt = head->shipCoord->next; pt != nullptr; pt = pt->next)
	//		{
	//			if (rdm.c == pt->c && rdm.r == pt->r)
	//			{
	//				if (pt->d == HORIZONTAL)
	//				{
	//					pt->d = VERTICAL;
	//					di = VERTICAL;
	//					break;
	//				}
	//				cerr << "point tried" << endl;
	//				rdm.r = -1;
	//				rdm.c = -1;
	//
	//				while (!game().isValid(rdm))
	//				{
	//					rdm.r = randInt(game().rows());
	//					rdm.c = randInt(game().cols());
	//				}
	//				pt = head->shipCoord;
	//			}
	//		}
	//
	//		if (di != VERTICAL)
	//		{
	//			Pt* newPt = new Pt;
	//			newPt->r = rdm.r;
	//			newPt->c = rdm.c;
	//			newPt->d = HORIZONTAL;
	//			newPt->next = nullptr;
	//			Pt* temp;
	//			for (temp = head->shipCoord; temp->next != nullptr; temp = temp->next)
	//				;
	//			temp->next = newPt;
	//		}
	//
	//		//The following codes are for testing purposes only:
	////		Pt* temp;
	////		for (temp = head->shipCoord; temp->next != nullptr; temp = temp->next)
	////			;
	////		cerr << rdm.r << "," << rdm.c << " " << di << " " << endl;
	//	
	//		//now rdm is a new point
	//
	//		cerr << "CheckPoint1" << endl;
	//
	//		if (!unplaceindicator && b.placeShip(rdm, head->ID, di))		//If successfully placed
	//		{
	//			head = head->next;
	//			cerr << "Successfully placed" << endl;
	//			//record it
	//			LastShipPlaced* newShipP = new LastShipPlaced;
	//			newShipP->ID = head->ID;
	//			newShipP->r = rdm.r;
	//			newShipP->c = rdm.c;
	//			newShipP->d = di;
	//			newShipP->next = nullptr;
	//			LastShipPlaced* templ = nullptr;
	//			for (templ = lsp; templ != nullptr && templ->next != nullptr; templ = templ->next)
	//				;
	//			if (templ != nullptr)
	//				templ->next = newShipP;
	//			else
	//				lsp = newShipP;
	//		}
	//		else											//If cannot place at this point
	//		{
	//			cerr << "CheckPoint2" << endl;
	//
	//			unplaceindicator = false;
	//			//change direction
	//			int count = 0;
	//			for (Pt* x = head->shipCoord->next; x != nullptr; x = x->next)	//there exists an dummy node in this linked list
	//			{
	//				if (x->d == VERTICAL)
	//					count++;
	////				cerr << "The count is " << count << endl;
	//			}
	////			cerr << "CheckPoint3" << endl;
	//
	//			if (count < (game().rows() * game().cols()))
	//			{
	//
	//			}
	//			else										//All points are not possible, rollback
	//			{
	//				cerr << "CheckPoint3" << endl;
	//				if (head->prev == nullptr)			//If the shipID 0 cannot be placed, go back step1, unblock & block again
	//				{
	//					placeshipcount++;			//Add count
	//					cerr << "placeshipcount is now " << placeshipcount << endl;
	//					b.unblock();
	//					//Reset shipList
	//					//Delete shipList Linked List
	//					shipList* temp = head;
	//					while (temp != nullptr && temp->next != nullptr)
	//						temp = temp->next;
	//					temp = temp->prev;
	//					delete temp->next;	//delete the indicator node
	//					if (temp != nullptr)
	//					{
	//						while (temp->prev != nullptr)
	//						{
	//							temp = temp->prev;
	//
	//							Pt* tem = temp->next->shipCoord;
	//							Pt* tem2 = temp->next->shipCoord->next;
	//							for (; tem2 != nullptr; tem2 = tem2->next)
	//							{
	//								delete tem;
	//								tem = tem2;
	//							}
	//							delete tem;
	//
	//							delete temp->next;
	//						}
	//						Pt* tem = temp->shipCoord;
	//						Pt* tem2 = temp->shipCoord->next;
	//						for (; tem2 != nullptr; tem2 = tem2->next)
	//						{
	//							delete tem;
	//							tem = tem2;
	//						}
	//						delete temp;
	//						temp = nullptr;
	//					}
	//					//Initiate everything & copy shipList again
	//					blockindicator = false;
	//					unplaceindicator = false;
	//					head = nullptr;
	//					shipList* ptr_prev = nullptr;
	//					for (int a = 0; a < game().nShips(); a++)	//Copy the ship info
	//					{
	//						if (head == nullptr)
	//						{
	//							head = new shipList;
	//							head->ID = a;
	//							head->length = game().shipLength(a);
	//							head->name = game().shipName(a);
	//							head->symbol = game().shipSymbol(a);
	//							head->prev = nullptr;
	//							head->next = nullptr;
	//							ptr_prev = head;
	//							head->shipCoord = nullptr;
	//							head->shipCoord = new Pt;
	//							head->shipCoord->next = nullptr;
	//						}
	//						else
	//						{
	//							shipList* newship = new shipList;
	//							newship->ID = a;
	//							newship->length = game().shipLength(a);
	//							newship->name = game().shipName(a);
	//							newship->symbol = game().shipSymbol(a);
	//							newship->prev = ptr_prev;
	//							newship->next = nullptr;
	//							ptr_prev->next = newship;
	//							ptr_prev = ptr_prev->next;
	//							newship->shipCoord = nullptr;
	//							newship->shipCoord = new Pt;
	//							newship->shipCoord->next = nullptr;
	//						}
	//					}
	//					shipList* newship = new shipList;
	//					newship->ID = -1;
	//					newship->name = "Ending Node";
	//					newship->prev = ptr_prev;
	//					newship->next = nullptr;
	//					ptr_prev->next = newship;
	//
	//				}
	//				else
	//				{
	//					//unplace, trace back, delete record
	//					head = head->prev;
	//
	//					LastShipPlaced* g = lsp;
	//					if (g->next != nullptr)
	//					{
	//						LastShipPlaced* k = lsp->next;
	//						for (; k->next != nullptr; k = k->next, g = g->next)
	//							;
	//						Point pt2(k->r, k->c);
	//						b.unplaceShip(pt2, k->ID, k->d);
	//						unplaceindicator = true;
	//						delete k;
	//						g->next = nullptr;
	//					}
	//					else
	//					{
	//						Point pt2(g->r, g->c);
	//						b.unplaceShip(pt2, g->ID, g->d);
	//						unplaceindicator = true;
	//						delete g;
	//						g = nullptr;
	//					}
	//
	//				}
	//			}
	//		}
	//	}
	//	else			//All ships are placed
	//	{
	//		b.unblock();		//Unblock
	//		return true;
	//	}
	//
	//	if (!placeShips(b))			//Recursion
	//	{
	//		return false;
	//	}
	//	else
	//		return true;


	//while (!blockindicator)
	//{
	//	b.block();
	//	blockindicator = true;
	//	if (placeshipcount >= 50)
	//		return false;
	//	placeshipcount++;
	//	for (int a = 0; a < game().nShips(); a++)
	//	{
	//		int r = randInt(game().rows());
	//		int c = randInt(game().cols());
	//		Point p(r, c);
	//		Direction d;
	//		if (randInt(2) == 0)
	//			d = HORIZONTAL;
	//		else
	//			d = VERTICAL;
	//		if (checkPtAlrExist(r, c, d))
	//		{
	//			a--;
	//			continue;
	//		}
	//		int count = 0;
	//		if (b.placeShip(p, a, d))
	//		{
	//			//successful!!! record this position
	//			LastShipPlaced* newShipP = new LastShipPlaced;
	//			newShipP->ID = a;
	//			newShipP->r = p.r;
	//			newShipP->c = p.c;
	//			newShipP->d = d;
	//			newShipP->next = nullptr;
	//			LastShipPlaced* templ = nullptr;
	//			for (templ = lsp; templ != nullptr && templ->next != nullptr; templ = templ->next)
	//				;
	//			if (templ != nullptr)
	//				templ->next = newShipP;
	//			else
	//				lsp = newShipP;
	//			clearShipCoordList();
	//			continue;
	//		}
	//		else
	//		{
	//			//unsuccessful, record failed position and retry
	//			Pt* newShipP = new Pt;
	//			newShipP->r = p.r;
	//			newShipP->c = p.c;
	//			newShipP->d = d;
	//			newShipP->next = nullptr;
	//			Pt* templ = nullptr;
	//			for (templ = shipCoord; templ != nullptr && templ->next != nullptr; templ = templ->next)
	//				;
	//			if (templ != nullptr)
	//				templ->next = newShipP;
	//			else
	//				shipCoord = newShipP;
	//			a--;
	//			continue;
	//		}

	//		if (count >= 2 * game().rows() * game().cols())		//all pts & directions attempted
	//		{
	//			//rollback
	//			if (lsp == nullptr)
	//			{
	//				//reset
	//				b.unblock();
	//				blockindicator = false;
	//				clearShipCoordList();
	//				break;
	//			}
	//			else
	//			{
	//				//unplaceship
	//				LastShipPlaced* temp = lsp;
	//				LastShipPlaced* temp2 = lsp;
	//				while (temp->next != nullptr)
	//				{
	//					temp2 = temp;
	//					temp = temp->next;
	//				}
	//				Point p1(temp->r, temp->c);
	//				b.unplaceShip(p1, temp->ID, temp->d);
	//				if (temp != temp2)
	//				{
	//					delete temp;
	//					temp2->next = nullptr;
	//				}
	//				else
	//				{
	//					delete temp;
	//					temp = nullptr;
	//				}
	//			}
	//		}
	//	}
	//}

	//b.unblock();
	//return true;



	vector<Point>x;
//	x.push_back(Point(0, 0));
//	x.push_back(Point(game().rows()-1, 0));
//	x.push_back(Point(0, game().cols()-1));
	x.push_back(Point((game().rows() / 2 - 1), (game().cols() / 3 - 1)));				//7
	x.push_back(Point((game().rows() / 4 - 1), (game().cols() / 4 - 1)));				//2
	x.push_back(Point((game().rows() / 3 - 1), (game().cols() / 3 * 2 - 1)));			//6
	x.push_back(Point((game().rows() / 5 * 4 - 1), (game().cols() / 5 * 4 - 1)));		//15
	x.push_back(Point((game().rows() / 10* 9 - 1), (game().cols() / 5 - 1)));			//13
	x.push_back(Point((game().rows() / 5 * 4 - 1) , (game().cols() / 5 - 1)));			//16
	x.push_back(Point((game().rows() / 3 - 1), (game().cols() / 3 - 1)));				//3
	x.push_back(Point((game().rows() / 3 * 2 - 1), (game().cols() / 3 * 2 - 1)));		//4
	x.push_back(Point((game().rows() / 3 * 2 - 1), (game().cols() / 3 - 1)));			//5
	x.push_back(Point((game().rows() / 3 - 1), (game().cols() / 2 - 1)));				//8
	x.push_back(Point((game().rows() / 3 * 2 - 1), (game().cols() / 2 - 1)));			//9
	x.push_back(Point((game().rows() / 2 - 1), (game().cols() / 3 * 2 - 1)));			//10
	x.push_back(Point((game().rows() / 4 * 3 - 1), (game().cols() / 4 - 1)));			//11
	x.push_back(Point((game().rows() / 4 - 1), (game().cols() / 4 * 3 - 1)));			//12
	x.push_back(Point((game().rows() / 4 * 3 - 1), (game().cols() / 4 * 3 - 1)));		//13
	x.push_back(Point((game().rows() / 5 - 1), (game().cols() / 5 - 1)));				//14
	x.push_back(Point((game().rows() / 2 - 1), (game().cols() / 2 - 1)));				//1




	for (int a = 0; a < game().nShips(); a++)
	{
		int c;
		for (c = 0; c < x.size(); c++)
		{
			if (b.placeShip(x[c], a, HORIZONTAL))
			{
				break;
			}
			else
			{
				if (b.placeShip(x[c], a, VERTICAL))
				{
					break;
				}
				else
					;
			}
		}
		if (c == x.size())
		{
			Point t(randInt(game().rows()), randInt(game().cols()));
			Direction d = HORIZONTAL;
			while (!b.placeShip(t, a, d))
			{
				if (d == HORIZONTAL)
					d = VERTICAL;
				else
				{
					t.r = randInt(game().rows());
					t.c = randInt(game().cols());
				}
			}
		}
		cerr << "nShips() is " << game().nShips() << endl;
		cerr << "a is " << a << endl;

	}

	return true;


}

Point GoodPlayer::recommendAttack()
{
	if (overwritePt)
	{
		cerr << "overwrite is true" << endl;
		Pt* newPt = new Pt;
		newPt->r = recmPt->r;
		newPt->c = recmPt->c;
		newPt->next = nullptr;
		Pt* temp;
		for (temp = headPtr; temp->next != nullptr; temp = temp->next)
			;
		temp->next = newPt;
		cerr << "recmPt (" << recmPt->r << "," << recmPt->c << "), fn recommendAttack exits here" << endl;
		return *recmPt;
	}


	vector<Point>x;
	x.push_back(Point(5, 5));
	x.push_back(Point(4, 4));
	x.push_back(Point(6, 6));
	x.push_back(Point(7, 3));
	x.push_back(Point(3, 7));
	x.push_back(Point(2, 2));
	x.push_back(Point(8, 8));
	x.push_back(Point(2, 8));
	x.push_back(Point(8, 2));
	x.push_back(Point(5, 9));
	x.push_back(Point(9, 5));
	x.push_back(Point(1, 5));
	x.push_back(Point(5, 1));
	x.push_back(Point(4, 6));
	x.push_back(Point(6, 4));
	x.push_back(Point(9, 9));
	x.push_back(Point(0, 4));
	x.push_back(Point(4, 0));
	x.push_back(Point(3, 3));
	x.push_back(Point(7, 7));
	//x.push_back(Point());
	//x.push_back(Point());
	//x.push_back(Point());


	int c;
	Point* pp;
	for (c = 0; c < x.size(); c++)
	{
		if (!checkPtAlrExist(x[c].r, x[c].c))
		{
			pp = &x[c];
			break;
		}
	}
	if (c == x.size())
	{
		if (randInt(2) == 0)
		{
			Point p((randInt(game().rows()) + 1) / 2 * 2, (randInt(game().cols() + 1) / 2 * 2));
			pp = &p;
		}
		else
		{
			Point p((randInt(game().rows()) + 1) / 2 * 2 + 1, (randInt(game().cols() + 1) / 2 * 2 + 1));
			pp = &p;
		}
		int countloops = 0;
		//Check if this point is in the linked list
		for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
		{
			if (countloops > (2 * game().rows() * game().cols()) && pp->c == pt->c && pp->r == pt->r || !game().isValid(*pp))
			{
				Point px(randInt(game().rows()), randInt(game().cols()));
				pp = &px;
				pt = headPtr;
				countloops++;

			}
			else if (pp->c == pt->c && pp->r == pt->r || !game().isValid(*pp))
			{
				if (randInt(2) == 0)
				{
					Point px((randInt(game().rows() + 1) / 2 * 2 + 1), (randInt(game().cols() + 1) / 2 * 2 + 1));
					pp = &px;
					pt = headPtr;
					countloops++;
				}
				else
				{
					Point px((randInt(game().rows() + 1) / 2 * 2), (randInt(game().cols() + 1) / 2 * 2));
					pp = &px;
					pt = headPtr;
					countloops++;
				}
			}
		}
	}
	//Add this point into the linked list (dummy node in singly linked list)
	Pt* newPt = new Pt;
	newPt->r = pp->r;
	newPt->c = pp->c;
	newPt->next = nullptr;
	Pt* temp;
	for (temp = headPtr; temp->next != nullptr; temp = temp->next)
		;
	temp->next = newPt;

	return *pp;

}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
	bool shipDestroyed, int shipId)
{
	bool indicator1 = false;
	bool indicator2 = false;
	Point* ptr2p = nullptr;

	if (!overwritePt && validShot && shotHit && !shipDestroyed)
	{
		hit = new Point(p.r, p.c);
		Point step2p(p.r, p.c);
		ptr2p = &step2p;
		//Check if this point is in the linked list
		int count = 0;
		int a = 1;
		cerr << "CheckPoint1" << endl;
		for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
		{
			cerr << "headPtr points to (" << pt->r << "," << pt->c << ")" << endl;
			cerr << "CheckPoint2" << endl;
			if (step2p.c == pt->c && step2p.r == pt->r)
			{
				if (count > 20)
				{
					count = 0;
					a++;
				}
				cerr << "point exists" << endl;
				step2p.r = -1;
				step2p.c = -1;

				while (!game().isValid(step2p))
				{
					if (randInt(2) == 0)
					{
						if (randInt(2) == 0)
						{
							step2p.r = hit->r + a;
							step2p.c = hit->c;
						}
						else
						{
							step2p.r = hit->r - a;
							step2p.c = hit->c;
						}
					}
					else
					{
						if (randInt(2) == 0)
						{
							step2p.r = hit->r;
							step2p.c = hit->c + a;
						}
						else
						{
							step2p.r = hit->r;
							step2p.c = hit->c - a;
						}
					}
				}
				count++;
				cerr << "1recmPt generated is (" << step2p.r << "," << step2p.c << ")." << endl;
				pt = headPtr;
			}
			//while (!game().isValid(step2p))
			//{
			//	if (randInt(2) == 0)
			//	{
			//		step2p.r = hit->r - 4 + randInt(9);
			//		step2p.c = hit->c;
			//	}
			//	else
			//	{
			//		step2p.r = hit->r;
			//		step2p.c = hit->c - 4 + randInt(9);
			//	}
			//}

		}
		recmPt = new Point(step2p.r, step2p.c);
		overwritePt = true;
	}

	else if (overwritePt && shotHit && !shipDestroyed)
	{
		if (hit2 != nullptr)
		{
			if (hit3 == nullptr)
				hit3 = new Point(p.r, p.c);
			else
			{
				if (hit4 == nullptr)
					hit4 = new Point(p.r, p.c);
				else
				{
					if (hit5 == nullptr)
						hit5 = new Point(p.r, p.c);
					else
					{
						if (hit6 == nullptr)
							hit6 = new Point(p.r, p.c);
					}
				}
			}
		}
		else
			hit2 = new Point(p.r, p.c);

		if (hit2 != nullptr)
		{
			if (hit6 != nullptr)
			{
				if (hit5->r == p.r)
				{
					Point step2p(p.r, 2 * p.c - hit5->c);
					ptr2p = &step2p;
				}
				else
				{
					Point step2p(2 * p.r - hit5->c, p.c);
					ptr2p = &step2p;
				}
			}
			if (hit5 != nullptr)
			{
				if (hit4->r == p.r)
				{
					Point step2p(p.r, 2 * p.c - hit4->c);
					ptr2p = &step2p;
				}
				else
				{
					Point step2p(2 * p.r - hit4->r, p.c);
					ptr2p = &step2p;
				}
			}
			if (hit4 != nullptr)
			{
				if (hit3->r == p.r)
				{
					Point step2p(p.r, 2 * p.c - hit3->c);
					ptr2p = &step2p;
				}
				else
				{
					Point step2p(2 * p.r - hit3->r, p.c);
					ptr2p = &step2p;
				}
			}
			else if (hit3 != nullptr)
			{
				if (hit2->r == p.r)
				{
					Point step2p(p.r, 2 * p.c - hit2->c);
					ptr2p = &step2p;
				}
				else
				{
					Point step2p(2 * p.r - hit2->r, p.c);
					ptr2p = &step2p;
				}
			}
			else if (hit2 != nullptr)
			{
				if (hit->r == p.r)
				{
					Point step2p(p.r, 2 * p.c - hit->c);
					ptr2p = &step2p;
				}
				else
				{
					Point step2p(2 * p.r - hit->r, p.c);
					ptr2p = &step2p;
				}
			}
			if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(*ptr2p))
			{
				indicator2 = true;
			}
			else
			{
				if (hit6 != nullptr)
				{
					delete hit6;
					delete hit5;
					delete hit4;
					delete hit3;
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
					hit4 = nullptr;
					hit5 = nullptr;
					hit6 = nullptr;
					if (hit->r == p.r)
					{
						Point step2p(p.r, 2 * hit2->c - hit->c);
						ptr2p = &step2p;
						if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(step2p))
						{
							indicator2 = true;
						}
						else
						{
							hit3 = hit;
							hit = hit2;
							hit2 = hit3;
							hit3 = nullptr;
						}
					}
					else
					{
						Point step2p(2 * hit2->r - hit->r, p.c);
						ptr2p = &step2p;
						if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(step2p))
						{
							indicator2 = true;
						}
						else
						{
							hit3 = hit;
							hit = hit2;
							hit2 = hit3;
							hit3 = nullptr;
						}
					}
				}
				else if (hit5 != nullptr)
				{
					delete hit5;
					delete hit4;
					delete hit3;
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
					hit4 = nullptr;
					hit5 = nullptr;
					if (hit->r == p.r)
					{
						Point step2p(p.r, 2 * hit2->c - hit->c);
						ptr2p = &step2p;
						if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(step2p))
						{
							indicator2 = true;
						}
						else
						{
							hit3 = hit;
							hit = hit2;
							hit2 = hit3;
							hit3 = nullptr;
						}
					}
					else
					{
						Point step2p(2 * hit2->r - hit->r, p.c);
						ptr2p = &step2p;
						if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(step2p))
						{
							indicator2 = true;
						}
						else
						{
							hit3 = hit;
							hit = hit2;
							hit2 = hit3;
							hit3 = nullptr;
						}
					}
				}
				else if (hit4 != nullptr)
				{
					delete hit4;
					delete hit3;
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
					hit4 = nullptr;
					if (hit->r == p.r)
					{
						Point step2p(p.r, 2 * hit2->c - hit->c);
						ptr2p = &step2p;
						if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(step2p))
						{
							indicator2 = true;
						}
						else
						{
							hit3 = hit;
							hit = hit2;
							hit2 = hit3;
							hit3 = nullptr;
						}
					}
					else
					{
						Point step2p(2 * hit2->r - hit->r, p.c);
						ptr2p = &step2p;
						if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(step2p))
						{
							indicator2 = true;
						}
						else
						{
							hit3 = hit;
							hit = hit2;
							hit2 = hit3;
							hit3 = nullptr;
						}
					}
				}
				else if (hit3 != nullptr)
				{
					delete hit3;
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
					if (hit->r == p.r)
					{
						Point step2p(p.r, 2 * hit2->c - hit->c);
						ptr2p = &step2p;
						if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(step2p))
						{
							indicator2 = true;
						}
						else
						{
							hit3 = hit;
							hit = hit2;
							hit2 = hit3;
							hit3 = nullptr;
						}
					}
					else
					{
						Point step2p(2 * hit2->r - hit->r, p.c);
						ptr2p = &step2p;
						if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(step2p))
						{
							indicator2 = true;
						}
						else
						{
							hit3 = hit;
							hit = hit2;
							hit2 = hit3;
							hit3 = nullptr;
						}
					}
				}
				else if (hit2 != nullptr)
				{
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
					if (hit->r == p.r)
					{
						Point step2p(p.r, 2 * hit2->c - hit->c);
						ptr2p = &step2p;
						if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(step2p))
						{
							indicator2 = true;
						}
						else
						{
							hit3 = hit;
							hit = hit2;
							hit2 = hit3;
							hit3 = nullptr;
						}
					}
					else
					{
						Point step2p(2 * hit2->r - hit->r, p.c);
						ptr2p = &step2p;
						if (!checkPtAlrExist(ptr2p->r, ptr2p->c) && game().isValid(step2p))
						{
							indicator2 = true;
						}
						else
						{
							hit3 = hit;
							hit = hit2;
							hit2 = hit3;
							hit3 = nullptr;
						}
					}
				}
			}

			for (int a = 1; a <= 4; a++)
			{
				if (indicator2 == true)
					break;
				if (hit->r == p.r)
				{
					Point step2p(p.r, p.c + a);
					ptr2p = &step2p;
					for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
					{
						indicator2 = true;

						if ((step2p.c == pt->c && step2p.r == pt->r) || !game().isValid(step2p))
						{
							step2p.c = hit->c - a;
							for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
							{
								if ((step2p.c == pt->c && step2p.r == pt->r) || !game().isValid(step2p))
								{
									indicator2 = false;
									break;
								}
							}
							break;
						}
					}
				}
				else		//hit->c == p.c
				{
					Point step2p(p.r + a, p.c);
					ptr2p = &step2p;
					for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
					{
						indicator2 = true;
						if ((step2p.c == pt->c && step2p.r == pt->r) || !game().isValid(step2p))
						{
							step2p.r = hit->r - a;
							for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
							{
								if ((step2p.c == pt->c && step2p.r == pt->r) || !game().isValid(step2p))
								{
									indicator2 = false;
									break;
								}
							}
							break;
						}
					}
				}
			}

			if (indicator2 == false)
			{
				Point step2p(p.r, p.c);
				ptr2p = &step2p;
				//Check if this point is in the linked list
				int a = 1;
				int count = 0;
				for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
				{
					if (step2p.c == pt->c && step2p.r == pt->r)
					{
						if (count > 20)
						{
							count = 0;
							a++;
						}
						cerr << "point exists" << endl;
						step2p.r = -1;
						step2p.c = -1;

						while (!game().isValid(step2p))
						{
							if (randInt(2) == 0)
							{
								if (randInt(2) == 0)
								{
									step2p.r = hit->r + a;
									step2p.c = hit->c;
								}
								else
								{
									step2p.r = hit->r - a;
									step2p.c = hit->c;
								}
							}
							else
							{
								if (randInt(2) == 0)
								{
									step2p.r = hit->r;
									step2p.c = hit->c + a;
								}
								else
								{
									step2p.r = hit->r;
									step2p.c = hit->c - a;
								}
							}
							count++;
						}
						cerr << "2recmPt generated is (" << step2p.r << "," << step2p.c << ")." << endl;
						pt = headPtr;
					}
				}
			}
			delete recmPt;
			recmPt = new Point(ptr2p->r, ptr2p->c);
		}
	}
	else if (overwritePt && !shotHit && !shipDestroyed)
	{
		if (hit6 != nullptr)
		{
			delete hit6;
			delete hit5;
			delete hit4;
			delete hit3;
			hit3 = hit;
			hit = hit2;
			hit2 = hit3;
			hit3 = nullptr;
			hit4 = nullptr;
			hit5 = nullptr;
			hit6 = nullptr;
			if (hit->r == p.r)
			{
				Point step2p(p.r, 2 * hit2->c - hit->c);
				ptr2p = &step2p;
				if (!checkPtAlrExist(ptr2p->r, ptr2p->c) || !game().isValid(step2p))
				{
					indicator2 = true;
				}
				else
				{
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
				}
			}
			else
			{
				Point step2p(2 * hit2->r - hit->r, p.c);
				ptr2p = &step2p;
				if (!checkPtAlrExist(ptr2p->r, ptr2p->c) || !game().isValid(step2p))
				{
					indicator2 = true;
				}
				else
				{
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
				}
			}
		}
		else if (hit5 != nullptr)
		{
			delete hit5;
			delete hit4;
			delete hit3;
			hit3 = hit;
			hit = hit2;
			hit2 = hit3;
			hit3 = nullptr;
			hit4 = nullptr;
			hit5 = nullptr;
			if (hit->r == p.r)
			{
				Point step2p(p.r, 2 * hit2->c - hit->c);
				ptr2p = &step2p;
				if (!checkPtAlrExist(ptr2p->r, ptr2p->c) || !game().isValid(step2p))
				{
					indicator2 = true;
				}
				else
				{
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
				}
			}
			else
			{
				Point step2p(2 * hit2->r - hit->r, p.c);
				ptr2p = &step2p;
				if (!checkPtAlrExist(ptr2p->r, ptr2p->c) || !game().isValid(step2p))
				{
					indicator2 = true;
				}
				else
				{
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
				}
			}
		}
		else if (hit4 != nullptr)
		{
			delete hit4;
			delete hit3;
			hit3 = hit;
			hit = hit2;
			hit2 = hit3;
			hit3 = nullptr;
			hit4 = nullptr;
			if (hit->r == p.r)
			{
				Point step2p(p.r, 2 * hit2->c - hit->c);
				ptr2p = &step2p;
				if (!checkPtAlrExist(ptr2p->r, ptr2p->c) || !game().isValid(step2p))
				{
					indicator2 = true;
				}
				else
				{
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
				}
			}
			else
			{
				Point step2p(2 * hit2->r - hit->r, p.c);
				ptr2p = &step2p;
				if (!checkPtAlrExist(ptr2p->r, ptr2p->c) || !game().isValid(step2p))
				{
					indicator2 = true;
				}
				else
				{
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
				}
			}
		}
		else if (hit3 != nullptr)
		{
			delete hit3;
			hit3 = hit;
			hit = hit2;
			hit2 = hit3;
			hit3 = nullptr;
			if (hit->r == p.r)
			{
				Point step2p(p.r, 2 * hit2->c - hit->c);
				ptr2p = &step2p;
				if (!checkPtAlrExist(ptr2p->r, ptr2p->c) || !game().isValid(step2p))
				{
					indicator2 = true;
				}
				else
				{
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
				}
			}
			else
			{
				Point step2p(2 * hit2->r - hit->r, p.c);
				ptr2p = &step2p;
				if (!checkPtAlrExist(ptr2p->r, ptr2p->c) || !game().isValid(step2p))
				{
					indicator2 = true;
				}
				else
				{
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
				}
			}
		}
		else if (hit2 != nullptr)
		{
			hit3 = hit;
			hit = hit2;
			hit2 = hit3;
			hit3 = nullptr;
			if (hit->r == p.r)
			{
				Point step2p(p.r, 2 * hit2->c - hit->c);
				ptr2p = &step2p;
				if (!checkPtAlrExist(ptr2p->r, ptr2p->c) || !game().isValid(step2p))
				{
					indicator2 = true;
				}
				else
				{
					delete hit;
					hit = hit2;
					hit2 = nullptr;
				}
			}
			else
			{
				Point step2p(2 * hit2->r - hit->r, p.c);
				ptr2p = &step2p;
				if (!checkPtAlrExist(ptr2p->r, ptr2p->c) || !game().isValid(step2p))
				{
					indicator2 = true;
				}
				else
				{
					hit3 = hit;
					hit = hit2;
					hit2 = hit3;
					hit3 = nullptr;
				}
			}
		}

		if ((p.r == hit->r && (p.c - hit->c == 1 || p.c - hit->c == -1)) || (p.c == hit->c && (p.r - hit->r == 1 || p.r - hit->r == -1)))
		{
			for (int a = 1; a <= 4; a++)
			{
				if (indicator2 == true)
					break;
				if (hit->r == p.r)
				{
					Point step2p(hit->r + a, hit->c);
					ptr2p = &step2p;
					indicator2 = true;
					if (checkPtAlrExist(step2p.r, step2p.c) || !game().isValid(step2p))
					{
						step2p.r = hit->r - a;
						if (checkPtAlrExist(step2p.r, step2p.c) || !game().isValid(step2p))
						{
							step2p.r = hit->r;
							step2p.c = 2 * hit->c - p.c;
							if (checkPtAlrExist(step2p.r, step2p.c) || !game().isValid(step2p))
							{
								indicator2 = false;
								break;
							}
						}
						break;
					}
				}
				else		//hit->c == p.c
				{
					Point step2p(hit->r, hit->c + a);
					ptr2p = &step2p;
					indicator2 = true;
					if (checkPtAlrExist(step2p.r, step2p.c) || !game().isValid(step2p))
					{
						step2p.c = hit->c - a;
						if (checkPtAlrExist(step2p.r, step2p.c) || !game().isValid(step2p))
						{
							step2p.c = hit->c;
							step2p.r = 2 * hit->r - p.r;
							if (checkPtAlrExist(step2p.r, step2p.c) || !game().isValid(step2p))
							{
								indicator2 = false;
								break;
							}
						}
						break;
					}
				}
			}
		}

		if (indicator2 == false)
		{
			Point step2p(p.r, p.c);
			ptr2p = &step2p;
			//Check if this point is in the linked list
			int a = 1;
			int count = 0;
			for (Pt* pt = headPtr->next; pt != nullptr; pt = pt->next)
			{
				if (step2p.c == pt->c && step2p.r == pt->r)
				{
					if (count > 20)
					{
						count = 0;
						a++;
					}
					cerr << "point exists" << endl;
					step2p.r = -1;
					step2p.c = -1;

					while (!game().isValid(step2p))
					{
						if (randInt(2) == 0)
						{
							if (randInt(2) == 0)
							{
								step2p.r = hit->r + a;
								step2p.c = hit->c;
							}
							else
							{
								step2p.r = hit->r - a;
								step2p.c = hit->c;
							}
						}
						else
						{
							if (randInt(2) == 0)
							{
								step2p.r = hit->r;
								step2p.c = hit->c + a;
							}
							else
							{
								step2p.r = hit->r;
								step2p.c = hit->c - a;
							}
						}
						count++;
						cerr << count << endl;
					}
					cerr << "3recmPt generated is (" << step2p.r << "," << step2p.c << ")." << endl;
					pt = headPtr;
				}
			}
		}
		delete recmPt;
		recmPt = new Point(ptr2p->r, ptr2p->c);
	}

	if (overwritePt && shipDestroyed)
	{
		delete hit;
		delete hit2;
		delete hit3;
		delete hit4;
		delete hit5;
		delete hit6;
		hit = nullptr;
		hit2 = nullptr;
		hit3 = nullptr;
		hit4 = nullptr;
		hit5 = nullptr;
		hit6 = nullptr;
		delete recmPt;
		overwritePt = false;
		cerr << "shipDestroyed is true so pointers deleted" << endl;
		//for checking purposes: (delete later)
		recmPt = nullptr;
	}
	if (recmPt != nullptr)
	{
		cerr << "recmPt returned at (" << recmPt->r << "," << recmPt->c << ")." << endl;
		cerr << "hit coord at (" << hit->r << "," << hit->c << ")." << endl;
	}
}

void GoodPlayer::recordAttackByOpponent(Point p)
{

}


//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };

    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}
