//
//  Player.cpp
//  Project_3
//
//  Created by Setareh Khoylow on 5/4/22.
//

#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <algorithm>

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
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
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

class HumanPlayer : public Player {
    public:
        HumanPlayer(std::string nm, const Game& g)
         : Player(nm, g)
        {}

        virtual ~HumanPlayer() {}
        virtual bool isHuman() const { return true; }
        virtual bool placeShips(Board& b);
        virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
        virtual void recordAttackByOpponent(Point p);
        virtual Point recommendAttack();
};

bool HumanPlayer::placeShips(Board& b){
    cout << name() << " must place " << game().nShips() << " ships." << endl;
    b.display(false);
    for(int i = 0; i < game().nShips(); i++){
        char direction;
        cout << "Enter h or v for direction of " << game().shipName(i) << "(length " << game().shipLength(i) << "): ";
        cin >> direction;
        
        Direction d = HORIZONTAL; // should i just default it to horizontal
        if(direction == 'h')
            d = HORIZONTAL;
        else if(direction == 'v')
            d = VERTICAL;
        
        int row, col = 0;
        cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
        if(!getLineWithTwoIntegers(row, col))
            return false;
        
        Point p(row, col);
        
        if(!b.placeShip(p, i, d))
            return false;
        
        b.display(false); // display board with newly placed ship
    }
    return true;
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){
    // HumanPlayer completely ignores the result of any attack
}

void HumanPlayer::recordAttackByOpponent(Point p){
    // HumanPlayer completely ignores what the opponent does
}

Point HumanPlayer::recommendAttack(){
    int row, col = 0;
    getLineWithTwoIntegers(row, col);
    
    return Point(row, col);
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

class MediocrePlayer : public Player {
  public:
    MediocrePlayer(std::string nm, const Game& g)
     : Player(nm, g), state1(true), previousHit(0,0)
    {
        for(int r = 0; r < g.rows(); r++){
            for(int c = 0; c < g.cols(); c++){
                Point p = Point(r, c);
                allPosHits.push_back(p);
            }
        }
    }

    virtual ~MediocrePlayer() {}

    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    bool findPoint(const Point& p1, bool erase, vector<Point>& cont);
    void addSurroundingPoints(Point prevPoint);

  private:
    bool state1;
    Point previousHit;
    vector<Point> possibleHits;
    vector<Point> allPosHits;
};
// *********************** used in Mediocre and Good Player ************************************** //
bool recurPlaceShips(Board& b, const Game& g, int start, int end){
    if(start == end) // we placed all the ships
        return true;
    
    for(int r = 0; r < g.rows(); r++){
        for(int c = 0; c < g.cols(); c++){
            Point p = Point(r, c);
            if(b.placeShip(p, start, HORIZONTAL)){
                if(!recurPlaceShips(b, g, start+1, end)){
                    b.unplaceShip(p, start, HORIZONTAL);
                    // increment numOfTries here?
                } else{
                    return true;
                }
            } else if(b.placeShip(p, start, VERTICAL)){
                if(!recurPlaceShips(b, g, start+1, end)){
                    b.unplaceShip(p, start, VERTICAL);
                } else{
                    return true;
                }
            }
        }
    }
    return false; // we couldn't place that ship
}

bool MediocrePlayer::placeShips(Board& b){
    int numOfTries = 0;
    while(numOfTries < 50){
        b.block();
        const Game& g = game();
        if(recurPlaceShips(b, g, 0, game().nShips())){
            b.unblock();
            return true;
        }
        b.unblock();
        numOfTries++;
    }
    return false;
}

// *********************** used in Mediocre and Good Player ************************************** //
bool equalPoints(Point p1, Point p2){
    return ((p1.r == p2.r) && (p1.c == p2.c));
}

bool MediocrePlayer::findPoint(const Point& p1, bool erase, vector<Point>& cont){ // checks to see if point passed in hasn't already been attacked
    vector<Point>::iterator p = cont.begin();
    for(; p != cont.end(); p++){
        if(equalPoints(*p, p1))
            break;
    }
    
    if(p != cont.end()){
        if(erase)
            cont.erase(p);
        return true;
    }
    return false;
}

Point MediocrePlayer::recommendAttack(){
    int row = 0, col = 0;
    Point attack = Point(row,col);
    
    if(state1){
        if(allPosHits.empty()){
            int randNum = randInt(int(possibleHits.size()));
            attack = possibleHits[randNum]; // get random point
            possibleHits.erase(possibleHits.begin()+randNum); // delete point once used
        } else{
            int randNum = randInt(int(allPosHits.size())); // get random point from existing possible points so we don't get repeating points
            attack = allPosHits[randNum];
            allPosHits.erase(allPosHits.begin() + randNum);
        }
    } else{
        // pop off the vector to get new rand point
        int randNum = randInt(int(possibleHits.size()));
        Point p = possibleHits[randNum]; // get first point
        possibleHits.erase(possibleHits.begin()+randNum); // delete point once used
        row = p.r;
        col = p.c;
        attack = Point(row,col);
    }

    return attack;
    
}

void MediocrePlayer::addSurroundingPoints(Point prevPoint){
    // using previousHit we are going to go 4 up/down and 4 left/right
    int r = prevPoint.r;
    int c = prevPoint.c;
    for(int u = r-1; u >= 0 && u >= r-4; u--){ // get four points above
        Point p(u, c);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            possibleHits.push_back(p);
    }
    for(int d = r+1; d < game().rows() && d <= r+4; d++){ // get four points below
        Point p(d, c);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            possibleHits.push_back(p);
    }
    for(int l = c-1; l >= 0 && l >= c-4; l--){ // get four points left
        Point p(r, l);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            possibleHits.push_back(p);
    }
    for(int rig = c+1; rig < game().cols() && rig <= c+4; rig++){ // get four points right
        Point p(r, rig);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            possibleHits.push_back(p);
    }
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){
    if(state1){ // if in state 1
        if(shotHit){ // if hit ship
            if(!shipDestroyed){
                state1 = false; // switch to state 2
                previousHit = p;
                addSurroundingPoints(previousHit);
            } else{ // if destroyed the ship then pick random point
                state1 = true;
            }
        } else{
            state1 = true;
        }
    } else{ // if in state 2
        if(shotHit){ // if hit ship
            if(!shipDestroyed){
                state1 = false; // stay to state 2
                previousHit = p;
                addSurroundingPoints(previousHit);
            } else{ // if destroyed the ship then switch back to state 1
                state1 = true;
            }
        } else if(!possibleHits.empty()){ // if there are still points to try
            state1 = false; // stay in state 2
        } else{
            state1 = true;
        }
    }
}

void MediocrePlayer::recordAttackByOpponent(Point p){
    // MediocrePlayer completely ignores what the opponent does
}


//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.

class GoodPlayer : public Player {
  public:
    GoodPlayer(std::string nm, const Game& g);

    virtual ~GoodPlayer() {}

    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    bool findPoint(const Point& p1, bool erase, vector<Point>& cont);

  private:
    bool state1;
    bool state2;
    bool state3;
//    Point previousHit;
    int maxShipLength; // state 2 will use this info to add specific points
    int currPrevHitInd;
    vector<Point> prevHits;
    vector<Point> possibleHits;
    vector<int> possibleShipLengths;
    vector<Point> allPosHits;
    Direction orientation; // state 3 will use this info
};

GoodPlayer::GoodPlayer(std::string nm, const Game& g)
    : Player(nm, g), state1(true), state2(false), state3(false), maxShipLength(0), currPrevHitInd(0), orientation(HORIZONTAL)
{
    for(int r = 0; r < g.rows(); r++){
       for(int c = 0; c < g.cols(); c++){
           Point p = Point(r, c);
           allPosHits.push_back(p);
       }
    }
    
    // loop through ships and store their length
    for(int i = 0; i < game().nShips(); i++){
        int len = game().shipLength(i);
        possibleShipLengths.push_back(len);
    }
    // then sort it in ascending order, with largest length at the end
    sort(possibleShipLengths.begin(), possibleShipLengths.end());
    maxShipLength = possibleShipLengths[possibleShipLengths.size()-1]; 
}

bool GoodPlayer::placeShips(Board& b){
    int numOfTries = 0;
    while(numOfTries < 50){
        b.block();
        const Game& g = game();
        if(recurPlaceShips(b, g, 0, game().nShips())){
            b.unblock();
            return true;
        }
        b.unblock();
        numOfTries++;
    }
    return false;
}

bool GoodPlayer::findPoint(const Point& p1, bool erase, vector<Point>& cont){ // checks to see if point passed in hasn't already been attacked
    vector<Point>::iterator p = cont.begin();
    for(; p != cont.end(); p++){
        if(equalPoints(*p, p1))
            break;
    }
    
    if(p != cont.end()){
        if(erase)
            cont.erase(p);
        return true;
    }
    return false;
}

Point GoodPlayer::recommendAttack(){
    int row = 0, col = 0;
    Point attack = Point(row,col);
    bool hasVisited = false;
    int indPrevHit = currPrevHitInd;
    
    do {
        if(state1){ // if in state 1
            int randNum = randInt(int(allPosHits.size())); // get random point from existing possible points so we don't get repeating points
            attack = allPosHits[randNum];
            allPosHits.erase(allPosHits.begin() + randNum);
        } else if(state2 && !state3){ // if in state 2
            if(indPrevHit < prevHits.size()){ // if there are previous hits to base our possible hits off of
                // using previousHit we are going to go up/down and left/right based on ship's length
                int r = prevHits[indPrevHit].r;
                int c = prevHits[indPrevHit].c;
                
                // ******************** for some reason adding impossible hits to possible hits (126, 0) ********************************* //
                for(int u = r-1; u >= 0 && u > r-maxShipLength; u--){ // get four points above
                    Point p(u, c);
                    if(findPoint(p, false, allPosHits) && !findPoint(p, false, possibleHits))
                        possibleHits.push_back(p);
                }
                for(int d = r+1; d < game().rows() && d < r+maxShipLength; d++){ // get four points below
                    Point p(d, c);
                    if(findPoint(p, false, allPosHits) && !findPoint(p, false, possibleHits))
                        possibleHits.push_back(p);
                }
                for(int l = c-1; l >= 0 && l > c-maxShipLength; l--){ // get four points left
                    Point p(r, l);
                    if(findPoint(p, false, allPosHits) && !findPoint(p, false, possibleHits))
                        possibleHits.push_back(p);
                }
                for(int rig = c+1; rig < game().cols() && rig < c+maxShipLength; rig++){ // get four points right
                    Point p(r, rig);
                    if(findPoint(p, false, allPosHits) && !findPoint(p, false, possibleHits)) // make sure its not in allPosHits and possibleHits
                        possibleHits.push_back(p);
                }
                // pop off vector to get new rand point
                int randNum = randInt(int(possibleHits.size()));
                Point p = possibleHits[randNum]; // get first point
                possibleHits.erase(possibleHits.begin()+randNum); // delete point once used
                row = p.r;
                col = p.c;
                attack = Point(row,col);
                
                if(findPoint(attack, true, allPosHits)){
                    hasVisited = false;
                } else{
                    hasVisited = true;
                    indPrevHit++;
                }
            } else{
                int randNum = randInt(int(possibleHits.size())); // get random point from existing possible points so we don't get repeating points
                attack = possibleHits[randNum];
                possibleHits.erase(possibleHits.begin() + randNum);
            }
        } else if(state3){ // if in state 3, we have the direction
            if(state2){ // if we just figured out the direction then remove other points
                Point prevHit = prevHits[0];
                // using orientation remove points we don't need
                if(orientation == HORIZONTAL){ // if horizontal, remove vertical points
                    for(vector<Point>::iterator p = possibleHits.begin(); p != possibleHits.end(); p++){
                        Point currP = *p;
                        if(prevHit.c == currP.c && prevHit.r != currP.r){
                            p = possibleHits.erase(p);
                            p--;
                        }
                    }
                } else{ // if vertical, remove horizontal points
                    for(vector<Point>::iterator p = possibleHits.begin(); p != possibleHits.end(); p++){
                        Point currP = *p;
                        if(prevHit.c != currP.c && prevHit.r == currP.r){
                            p = possibleHits.erase(p);
                            p--;
                        }
                    }
                }
                prevHits.erase(prevHits.begin()+0); // remove front element b/c already used
                state2 = false;
            }
            
            // pop off vector to get new rand point
            int randNum = randInt(int(possibleHits.size()));
            Point p = possibleHits[randNum]; // get first point
            possibleHits.erase(possibleHits.begin()+randNum); // delete point once used
            row = p.r;
            col = p.c;
            attack = Point(row,col);

            if(findPoint(attack, true, allPosHits)){
                hasVisited = false;
            } else{
                hasVisited = true;
            }
        }
    } while(hasVisited);
    
    return attack;
    
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){
    if(state1){ // if in state 1
        if(shotHit){ // if hit ship
            if(!shipDestroyed){ // if ship wasn't destroyed
                state1 = false; state2 = true; state3 = false;
                
                prevHits.push_back(p);
            } else{ // if destroyed the ship then pick random point
                state1 = true; state2 = false; state3 = false;
                
                //get length of ship destroyed and remove from possibleShipLengths
                int len = game().shipLength(shipId);
                vector<int>::iterator p = find(possibleShipLengths.begin(), possibleShipLengths.end(), len);
                if(p != possibleShipLengths.end()){
                    possibleShipLengths.erase(p);
                }
                maxShipLength = possibleShipLengths[possibleShipLengths.size()-1];
            }
        }else{
            state1 = true;
        }
    } else if(state2){ // if in state 2
        if(shotHit){ // if hit ship
            if(!shipDestroyed){
                Point previousHit = prevHits.front();
                if((previousHit.r + 1 == p.r || previousHit.r - 1 == p.r) && previousHit.c == p.c){ // if its next to it then we know it is the same ship
                    // not necesarrily vertical b/c could have hit another ship
                    // need to check symbol before moving to state 3
                    orientation = VERTICAL;
                    // go to state3
                    state1 = false; state2 = true; state3 = true;
                } else if((previousHit.c + 1 == p.c || previousHit.c - 1 == p.c) && previousHit.r == p.r){
                    orientation = HORIZONTAL;
                    // go to state3
                    state1 = false; state2 = true; state3 = true;
                } else{ // if its not next to it then it might be another ship
                    // stay in state2 and switch prevHit we look at to get possible points around it
                    state1 = false; state2 = true; state3 = false;
                    
                    prevHits.push_back(p);
                    currPrevHitInd++;
                }
            } else{ // if destroyed the ship then switch back to state 1
                state1 = true; state2 = false; state3 = false;
                
                //get length of ship destroyed and remove from possibleShipLengths
                int len = game().shipLength(shipId);
                vector<int>::iterator p = find(possibleShipLengths.begin(), possibleShipLengths.end(), len);
                if(p != possibleShipLengths.end()){
                    possibleShipLengths.erase(p);
                }
                maxShipLength = possibleShipLengths[possibleShipLengths.size()-1];
                prevHits.erase(prevHits.begin()+0); // pop prevHits front ???
                currPrevHitInd = 0;
            }
        } else if(!possibleHits.empty()){ // if there are still points to try
            // stay in state 2
            state1 = false; state2 = true; state3 = false;
            
            currPrevHitInd++;
        } else{ //go back to state 1
            state1 = true; state2 = false; state3 = false;
        }
    } else if(state3){
        if(shotHit){ // if hit ship
            if(!shipDestroyed){
                // stay to state 3
                state1 = false; state2 = false; state3 = true;
                
                prevHits.push_back(p);
            } else{ // if destroyed the ship
                if(!possibleHits.empty()){ // if there are still possible hits
                    // go to state 2
                    state1 = false; state2 = true; state3 = false;
                } else{ // if there aren't any other possibleHits then switch back to state 1
                    state1 = true; state2 = false; state3 = false;
                }
                
                //get length of ship destroyed and remove from possibleShipLengths to update max length
                int len = game().shipLength(shipId);
                vector<int>::iterator p = find(possibleShipLengths.begin(), possibleShipLengths.end(), len);
                if(p != possibleShipLengths.end()){
                    possibleShipLengths.erase(p);
                }
                maxShipLength = possibleShipLengths[possibleShipLengths.size()-1];
                prevHits.erase(prevHits.begin()+0); // pop prevHits front ???
            }
        } else if(!possibleHits.empty()){ // if there are still points to try
            // stay in state 3
            state1 = false; state2 = false; state3 = true;
        } else{ // go to state 1 or 2????
            state1 = true; state2 = false; state3 = false;;
        }
    }
}

void GoodPlayer::recordAttackByOpponent(Point p){
    // MediocrePlayer completely ignores what the opponent does
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

