//
//  Game.cpp
//  Project_3
//
//  Created by Setareh Khoylow on 5/4/22.
//

#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector> // can i use the vector library

using namespace std;

class GameImpl
{
    public:
        GameImpl(int nRows, int nCols);
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
        struct Ship{
            int m_len;
            char m_symb;
            string m_name;
        };
        vector<Ship> vShips;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
    if(nRows > MAXROWS)
        m_rows = MAXROWS;
    else
        m_rows = nRows;
    
    if(nCols > MAXCOLS)
        m_cols = MAXCOLS;
    else
        m_cols = nCols;
}

int GameImpl::rows() const
{
    return m_rows;
}

int GameImpl::cols() const
{
    return m_cols; 
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
    if(m_rows - length < 0 || m_cols - length < 0)
        return false;
    
    if(symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
        return false;
    
    if(!isascii(symbol)  ||  !isprint(symbol))
        return false;
    
    // if passed all requirements then create a new Ship object
    Ship newShip;
    newShip.m_len = length;
    newShip.m_symb = symbol;
    newShip.m_name = name;
    
    vShips.push_back(newShip);// add ship to the vector of ships
    
    return true;
}

int GameImpl::nShips() const
{
    return int(vShips.size());
}

int GameImpl::shipLength(int shipId) const
{
    if(shipId >= nShips())
        return -1;
    return vShips[shipId].m_len;
}

char GameImpl::shipSymbol(int shipId) const
{
    if(shipId >= nShips())
        return 'X';
    return vShips[shipId].m_symb;
}

string GameImpl::shipName(int shipId) const
{
    if(shipId >= nShips())
        return "";
    return vShips[shipId].m_name;
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    if(!p1->placeShips(b1))
        return nullptr;
    if(!p2->placeShips(b2))
        return nullptr;
    
    bool p1Turn = true;
    while(!(b1.allShipsDestroyed() || b2.allShipsDestroyed())){
        if(shouldPause)
            waitForEnter();
        
        if(p1Turn){
            cout << p1->name() << "'s turn. Board for " << p2->name() << ": " << endl;
            if(p1->isHuman()){
                b2.display(true);
                // ask the player for a point to attack and call the record attack function
                cout << "Enter the row and column to attack (e.g., 3 5): ";
            } else{
                b2.display(false); // make sure displaying correct board
            }
            
            Point p = p1->recommendAttack();
            bool shotHit = false;
            bool shipDestroyed = false;
            int shipId = 0;
            
            if(b2.attack(p, shotHit, shipDestroyed, shipId)){
                p1->recordAttackResult(p, true, shotHit, shipDestroyed, shipId);
                string hitType = "";
                if(shotHit){
                    hitType = "hit something";
                    if(shipDestroyed){
                        hitType = "destroyed the " + shipName(shipId);
                    }
                } else{
                    hitType = "missed";
                }
                cout << p1->name() << " attacked (" << p.r << "," << p.c << ") and " << hitType << ", resulting in: " << endl;
                if(p1->isHuman())
                    b2.display(true);
                else
                    b2.display(false);
            }else {
                cout << p1->name() << " waisted a shot at (" << p.r << "," << p.c << ")." << endl;
            }
        }else{
            cout << p2->name() << "'s turn. Board for " << p1->name() << ": " << endl;
            if(p2->isHuman()){
                b1.display(true);
                cout << "Enter the row and column to attack (e.g., 3 5): ";
            } else{
                b1.display(false); // make sure displaying correct board
            }
            
            Point p = p2->recommendAttack();
            bool shotHit = false;
            bool shipDestroyed = false;
            int shipId = 0;
            
            if(b1.attack(p, shotHit, shipDestroyed, shipId)){
                p2->recordAttackResult(p, true, shotHit, shipDestroyed, shipId);
                string hitType = "";
                if(shotHit){
                    hitType = "hit something";
                    if(shipDestroyed){
                        hitType = "destroyed the " + shipName(shipId);
                    }
                } else{
                    hitType = "missed";
                }
                cout << p2->name() << " attacked (" << p.r << "," << p.c << ") and " << hitType << ", resulting in: " << endl;
                if(p2->isHuman())
                    b1.display(true);
                else
                    b1.display(false);
            }else {
                cout << p2->name() << " waisted a shot at (" << p.r << "," << p.c << ")." << endl;
            }
        }
        p1Turn = !p1Turn;
    }
    
    if(b1.allShipsDestroyed()){ // p2 wins
        cout << p2->name() << " wins!" << endl;
        if(p1->isHuman())
            b2.display(false);
        return p2;
    }
    if(b2.allShipsDestroyed()){ // p1 wins
        cout << p1->name() << " wins!" << endl;
        if(p2->isHuman())
            b1.display(false);
        return p1;
    }
    
    return nullptr;
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


