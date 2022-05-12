//
//  Board.cpp
//  Project_3
//
//  Created by Setareh Khoylow on 5/4/22.
//

#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <vector>

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
    char board[MAXROWS][MAXCOLS];
    struct ShipInfo{
        int id;
        int numOfHits;
        char symbol;
    };
    vector<ShipInfo> shipInfo;
    int numOfShipsDestroyed;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g), numOfShipsDestroyed(0)
{
    // create a board array
    for(int r = 0; r < g.rows(); r++){
        for(int c = 0; c < g.cols(); c++){
            board[r][c] = '.';
        }
    }
}

void BoardImpl::clear()
{
    for(int r = 0; r < m_game.rows(); r++){
        for(int c = 0; c < m_game.cols(); c++){
            board[r][c] = '.';
        }
    }
}

void BoardImpl::block()
{
      // Block half the cells
    int totalPoints = m_game.rows() * m_game.cols();
    int totalBlocked = 0;
    while(totalBlocked < totalPoints/2){
        int randNum = randInt(totalPoints);
        int col = randNum / m_game.cols();
        int row = randNum % m_game.rows();
        
        if(m_game.isValid(Point(row, col))){
            if(board[row][col] == '.'){
                board[row][col] = '#';
                totalBlocked++;
            }
        }
    }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            if(board[r][c] == '#')
                board[r][c] = '.'; // TODO:  Replace this with code to unblock cell (r,c) if blocked
        }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    if(shipId >= m_game.nShips())
        return false;
    
    int shipLength = m_game.shipLength(shipId);
    char shipSymbol = m_game.shipSymbol(shipId);
    
    if(!m_game.isValid(topOrLeft))
        return false;
    
    if((topOrLeft.r + shipLength > m_game.rows() && dir == VERTICAL) || (topOrLeft.c + shipLength > m_game.cols() && dir == HORIZONTAL))
        return false;
    
    for(vector<ShipInfo>::iterator i = shipInfo.begin(); i != shipInfo.end(); i++){//if the ship is in this vector then it was already placed
        if(i->id == shipId)
            return false;
    }
    ShipInfo newShip;
    newShip.id = shipId;
    newShip.numOfHits = 0;
    newShip.symbol = shipSymbol;
    
    if(dir == VERTICAL){
        for(int r = topOrLeft.r; r < topOrLeft.r+shipLength; r++){
            if(board[r][topOrLeft.c] != '.'){
                // remove previously placed symbols
                for(int i = r-1; i >= topOrLeft.r; i--){
                    board[i][topOrLeft.c] = '.';
                }
                return false;
            }
            board[r][topOrLeft.c] = shipSymbol;
        }
    }else if(dir == HORIZONTAL){
        for(int c = topOrLeft.c; c < topOrLeft.c+shipLength; c++){
            if(board[topOrLeft.r][c] != '.'){
                // remove previously placed symbols
                for(int i = c-1; i >= topOrLeft.c; i--){
                    board[topOrLeft.r][i] = '.';
                }
                return false;
            }
            board[topOrLeft.r][c] = shipSymbol;
        }
    }
    shipInfo.push_back(newShip);
    
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if(shipId >= m_game.nShips())
        return false;
    
    if(!m_game.isValid(topOrLeft))
        return false;
    
    int shipLength = m_game.shipLength(shipId);
    char shipSymbol = m_game.shipSymbol(shipId);
    
    if(dir == VERTICAL){
        for(int r = topOrLeft.r; r < topOrLeft.r+shipLength; r++){
            if(board[r][topOrLeft.c] != shipSymbol){
                // remove previously placed symbols
                for(int i = r-1; i >= topOrLeft.r; i--){
                    board[i][topOrLeft.c] = shipSymbol;
                }
                return false;
            }
            board[r][topOrLeft.c] = '.';
        }
    }else if(dir == HORIZONTAL){
        for(int c = topOrLeft.c; c < topOrLeft.c+shipLength; c++){
            if(board[topOrLeft.r][c] != shipSymbol){
                // remove previously placed symbols
                for(int i = c-1; i >= topOrLeft.c; i--){
                    board[topOrLeft.r][i] = shipSymbol;
                }
                return false;
            }
            board[topOrLeft.r][c] = '.';
        }
    }
    
    vector<ShipInfo>::iterator deleteShipInd = shipInfo.begin();
    for(; deleteShipInd != shipInfo.end(); deleteShipInd++){ //if the ship is in this vector then it was already placed
        if(deleteShipInd->id == shipId)
            break;
    }

    shipInfo.erase(deleteShipInd); // deletes ship's info that was unplaced
    return true;
}

void BoardImpl::display(bool shotsOnly) const
{
    for(int i = 0; i < m_game.rows(); i++){
        if(i == 0){
            cout << "  ";
            for(int j = 0; j < m_game.cols(); j++){
                cout <<  j << " ";
            }
            cout << endl;
        }
        cout << i << " ";
        for(int j = 0; j < m_game.cols(); j++){
            if(shotsOnly && (board[i][j] == '.' || board[i][j] == 'X' || board[i][j] == 'o'))
                cout << board[i][j] << " ";
            else if(!shotsOnly)
                cout << board[i][j] << " ";
            else
                cout << ". ";
        }
        cout << endl;
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId) // X for hit and o for miss
{
    if(!m_game.isValid(p)) // if its an invalid point
        return false;
    
    char symbolHit = board[p.r][p.c];
    
    if(symbolHit == 'X' || symbolHit == 'o')// if the attack was already made
        return false;
    
    if(symbolHit != '.'){ // if its not a . or a X or an o then it hit a ship
        shotHit = true;
        board[p.r][p.c] = 'X';
    }else {
        shotHit = false;
        board[p.r][p.c] = 'o';
    }
    
    if(shotHit){
        vector<ShipInfo>::iterator i;
        for(i = shipInfo.begin(); i != shipInfo.end(); i++){
            if(i->symbol == symbolHit){
                i->numOfHits += 1;
                break;
            }
        }
        if(i != shipInfo.end()){
            if(i->numOfHits == m_game.shipLength(i->id)){
                shipDestroyed = true;
                shipId = i->id;
                numOfShipsDestroyed += 1;
            }
        }
    }
    
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
    return numOfShipsDestroyed == shipInfo.size();
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

