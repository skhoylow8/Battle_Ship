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
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

class Timer
{
  public:
    Timer()
    {
        start();
    }
    void start()
    {
        m_time = std::chrono::high_resolution_clock::now();
    }
    double elapsed() const
    {
        std::chrono::duration<double,std::milli> diff =
                          std::chrono::high_resolution_clock::now() - m_time;
        return diff.count();
    }
  private:
    std::chrono::high_resolution_clock::time_point m_time;
};

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

void HumanPlayer::recordAttackResult(Point /*p*/, bool /*validShot*/, bool /*shotHit*/, bool /*shipDestroyed*/, int /*shipId*/){
    // HumanPlayer completely ignores the result of any attack
}

void HumanPlayer::recordAttackByOpponent(Point /*p*/){
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

void MediocrePlayer::recordAttackResult(Point p, bool /*validShot*/, bool shotHit, bool shipDestroyed, int /*shipId*/){
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

void MediocrePlayer::recordAttackByOpponent(Point /*p*/){
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
    void addSurroundingPoints(Point prevPoint);
    void removeSurroundingPoints(Point prevPoint, std::string direction);
    void getDiagonalPoints(Point prevPoint);

  private:
    bool state1;
//    bool state2;
    int maxShipLength; // state 2 will use this info to add specific points
    Point previousHit;
//    vector<Point> prevHits;
    vector<Point> possibleHits;
    vector<int> possibleShipLengths;
    vector<Point> diagPoints; // use diagonal points when in state 1
    vector<Point> allPosHits;
};

GoodPlayer::GoodPlayer(std::string nm, const Game& g)
    : Player(nm, g), state1(true), maxShipLength(0)
{
    for(int r = 0; r < g.rows(); r++){
       for(int c = 0; c < g.cols(); c++){
           Point p = Point(r, c);
           if(g.rows()/2 == r && g.cols()/2 == c){
               diagPoints.push_back(p);
               // might need to get extra point if board has odd cols/rows
           } else{
               allPosHits.push_back(p);
           }
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
    Timer timer;
    timer.start();
    
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
    
    cerr << "GoodPlayer::placeShips()" << timer.elapsed() << endl;
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

void GoodPlayer::addSurroundingPoints(Point prevPoint){
    // using previousHit we are going to go up/down and left/right based on ship's length
    int r = prevPoint.r;
    int c = prevPoint.c;
    
    for(int u = r-1; u >= 0 && u > r-maxShipLength; u--){ // get four points above
        Point p(u, c);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            possibleHits.push_back(p);
    }
    for(int d = r+1; d < game().rows() && d < r+maxShipLength; d++){ // get four points below
        Point p(d, c);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            possibleHits.push_back(p);
    }
    for(int l = c-1; l >= 0 && l > c-maxShipLength; l--){ // get four points left
        Point p(r, l);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            possibleHits.push_back(p);
    }
    for(int rig = c+1; rig < game().cols() && rig < c+maxShipLength; rig++){ // get four points right
        Point p(r, rig);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits)) // make sure its not in allPosHits and possibleHits
            possibleHits.push_back(p);
    }
}

void GoodPlayer::removeSurroundingPoints(Point prevPoint, std::string direction){
    int r = prevPoint.r;
    int c = prevPoint.c;
    
    if(direction == "up"){
        for(vector<Point>::iterator p = possibleHits.begin(); p != possibleHits.end(); p++){
            Point currP = *p;
            if(c == currP.c && r > currP.r){
                allPosHits.push_back(currP);
                p = possibleHits.erase(p);
                p--;
            }
        }
    } else if(direction == "down"){
        for(vector<Point>::iterator p = possibleHits.begin(); p != possibleHits.end(); p++){
            Point currP = *p;
            if(c == currP.c && r < currP.r){
                allPosHits.push_back(currP);
                p = possibleHits.erase(p);
                p--;
            }
        }
    } else if(direction == "left"){
        for(vector<Point>::iterator p = possibleHits.begin(); p != possibleHits.end(); p++){
            Point currP = *p;
            if(c > currP.c && r == currP.r){
                allPosHits.push_back(currP);
                p = possibleHits.erase(p);
                p--;
            }
        }
    } else if(direction == "right"){
        for(vector<Point>::iterator p = possibleHits.begin(); p != possibleHits.end(); p++){
            Point currP = *p;
            if(c < currP.c && r == currP.r){
                allPosHits.push_back(currP);
                p = possibleHits.erase(p);
                p--;
            }
        }
    }
}

void GoodPlayer::getDiagonalPoints(Point prevPoint){
    int row = prevPoint.r;
    int col = prevPoint.c;
    while(row-1 >= 0 && col-1 >= 0){ // gets upper left points diagonal from center
        Point p(row-1, col-1);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            diagPoints.push_back(p);
        row--; col--;
    }

    row = prevPoint.r;
    col = prevPoint.c;
    while(row-1 >= 0 && col+1 < game().cols()){ // gets upper right points diagonal from center
        Point p(row-1, col+1);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            diagPoints.push_back(p);
        row--; col++;
    }

    row = prevPoint.r;
    col = prevPoint.c;
    while(row+1 < game().rows() && col-1 >= 0){ // gets lower left points diagonal from center
        Point p(row+1, col-1);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            diagPoints.push_back(p);
        row++; col--;
    }

    row = prevPoint.r;
    col = prevPoint.c;
    while(row+1 < game().rows() && col+1 < game().cols()){ // gets lower right points diagonal from center
        Point p(row+1, col+1);
        if(findPoint(p, true, allPosHits) && !findPoint(p, false, possibleHits))
            diagPoints.push_back(p);
        row++; col++;
    }
}

Point GoodPlayer::recommendAttack(){
    Timer timer;
    timer.start();
    
    int row = 0, col = 0;
    Point attack = Point(row,col);
//    bool hasVisited = false;
    
//    do {
        if(state1){ // if in state 1
            if(diagPoints.empty()){
                if(possibleHits.empty()){
                    int randNum = randInt(int(allPosHits.size())); // get random point from existing possible points so we don't get repeating points
                    attack = allPosHits[randNum];
                    vector<Point>::iterator it = allPosHits.begin() + randNum;
                    allPosHits.erase(it);
                } else{
                    int randNum = randInt(int(possibleHits.size()));
                    attack = possibleHits[randNum]; // get first point
                    vector<Point>::iterator it = possibleHits.begin()+randNum;
                    possibleHits.erase(it); // delete point once used
                }
            } else{ // if there are still diagonal points to try get a random one
                int randNum = randInt(int(diagPoints.size()));
                attack = diagPoints[randNum];
                vector<Point>::iterator it = diagPoints.begin() + randNum;
                diagPoints.erase(it);
            }
        } else{ // if in state 2
            // pop off vector to get new rand point
            if(possibleHits.empty()){
                int randNum = randInt(int(diagPoints.size()));
                attack = diagPoints[randNum];
                vector<Point>::iterator it = diagPoints.begin() + randNum;
                diagPoints.erase(it);
            } else{
                int randNum = randInt(int(possibleHits.size()));
                attack = possibleHits[randNum]; // get first point
                vector<Point>::iterator it = possibleHits.begin()+randNum;
                possibleHits.erase(it); // delete point once used
            }
        }
//    } while(hasVisited);
    
    cerr << "GoodPlayer::recommendAttack(): " << timer.elapsed() << endl;
    
    return attack;
    
}

void GoodPlayer::recordAttackResult(Point p, bool /*validShot*/, bool shotHit, bool shipDestroyed, int shipId){
    Timer timer;
    timer.start();
    
    if(state1){ // if in state 1
        if(shotHit){ // if hit ship
            if(!shipDestroyed){ // if ship wasn't destroyed
                state1 = false;
                
                previousHit = p;
//                prevHits.push_back(p); // might not need prevHits
                addSurroundingPoints(p);
            } else{ // if destroyed the ship then pick random point
                state1 = true;
                
                //get length of ship destroyed and remove from possibleShipLengths
                int len = game().shipLength(shipId);
                vector<int>::iterator p = find(possibleShipLengths.begin(), possibleShipLengths.end(), len);
                if(p != possibleShipLengths.end()){
                    possibleShipLengths.erase(p);
                }
                if(!possibleShipLengths.empty())
                    maxShipLength = possibleShipLengths[possibleShipLengths.size()-1];
            }
        }else{
            state1 = true;
//            getDiagonalPoints(p);
        }
    } else { // if in state 2
        if(shotHit){ // if hit ship
            if(!shipDestroyed){
                // stay in state 2
                state1 = false;
//                addSurroundingPoints(p);
            } else{ // if destroyed the ship then switch back to state 1
                state1 = true;
                
                //get length of ship destroyed and remove from possibleShipLengths
                int len = game().shipLength(shipId);
                vector<int>::iterator p = find(possibleShipLengths.begin(), possibleShipLengths.end(), len);
                if(p != possibleShipLengths.end()){
                    possibleShipLengths.erase(p);
                }
                if(!possibleShipLengths.empty())
                    maxShipLength = possibleShipLengths[possibleShipLengths.size()-1];
//                prevHits.erase(prevHits.begin()+0); // pop prevHits front ???
            }
        } else if(!possibleHits.empty()){ // if there are still points to try
            // stay in state 2
            state1 = false;
            
            if(previousHit.c - 1 == p.c){
                removeSurroundingPoints(previousHit, "left");
            } else if(previousHit.c + 1 == p.c){
                removeSurroundingPoints(previousHit, "right");
            } else if(previousHit.r - 1 == p.r){
                removeSurroundingPoints(previousHit, "up");
            } else if (previousHit.r + 1 == p.r){
                removeSurroundingPoints(previousHit, "down");
            }
            
        } else{ //go back to state 1
            state1 = true;
        }
    }
    cerr << "GoodPlayer::recordAttackResult(): " << timer.elapsed() << endl;
}

void GoodPlayer::recordAttackByOpponent(Point /*p*/){
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

