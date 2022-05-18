//
//  main.cpp
//  Project_3
//
//  Created by Setareh Khoylow on 5/4/22.
//

#include "Game.h"
#include "Player.h"
#include "Board.h"
#include <iostream>
#include <string>

using namespace std;

bool addStandardShips(Game& g)
{
    return g.addShip(5, 'A', "aircraft carrier")  &&
           g.addShip(4, 'B', "battleship")  &&
           g.addShip(3, 'D', "destroyer")  &&
           g.addShip(3, 'S', "submarine")  &&
           g.addShip(2, 'P', "patrol boat");
}

int main()
{
    // Test Good Player placeShips
//    Game g(10, 10);
//    addStandardShips(g);
//    Player* p1 = createPlayer("good", "Good Grace", g);
//    Board b(g);
//
//    p1->placeShips(b);
//    b.display(false);
    
    // Test Board and Game functionality
//    Game g(10,10);
//    addStandardShips(g);
//    Board b(g);
//    Point p1(1,1);
//    Point p2(7,7);
//    Point p3(3,1);
//    Point p4(0,9);
//    Point p5(7,0);
//    assert(b.placeShip(p1, 0, HORIZONTAL) == true);
//    assert(b.placeShip(p4, 1, VERTICAL) == true);
//    assert(b.placeShip(p2, 4, VERTICAL) == true);
//    assert(b.placeShip(p3, 2, HORIZONTAL) == true);
//    assert(b.placeShip(p5, 3, HORIZONTAL) == true);
//
//    assert(b.unplaceShip(p3, 3, HORIZONTAL) == false); //wrong ID
//    assert(b.unplaceShip(p3, 2, VERTICAL) == false); //wrong direction
//    assert(b.unplaceShip(p3, 2, HORIZONTAL) == true);
//
//
//    bool shotHit = false;
//    bool shipDestroyed = false;
//    int shipId = 0;
//    b.attack(p5, shotHit, shipDestroyed, shipId);
//    assert(shotHit == true && shipDestroyed == false && shipId == 0); // id remains unchanged because it hasn't sunk the ship
//    b.attack(Point(7,1), shotHit, shipDestroyed, shipId);
//    b.attack(Point(7,2), shotHit, shipDestroyed, shipId);
//    assert(shotHit == true && shipDestroyed == true && shipId == 3);
//    b.attack(Point(1,1), shotHit, shipDestroyed, shipId);
//    b.attack(Point(1,2), shotHit, shipDestroyed, shipId);
//    b.attack(Point(1,3), shotHit, shipDestroyed, shipId);
//    b.attack(Point(1,4), shotHit, shipDestroyed, shipId);
//    b.attack(Point(1,5), shotHit, shipDestroyed, shipId);
//    assert(shotHit == true && shipDestroyed == true && shipId == 0);
//    b.attack(Point(0,9), shotHit, shipDestroyed, shipId);
//    b.attack(Point(1,9), shotHit, shipDestroyed, shipId);
//    b.attack(Point(2,9), shotHit, shipDestroyed, shipId);
//    b.attack(Point(3,9), shotHit, shipDestroyed, shipId);
//    assert(shotHit == true && shipDestroyed == true && shipId == 1);
//    b.attack(Point(7,7), shotHit, shipDestroyed, shipId);
//    b.attack(Point(8,7), shotHit, shipDestroyed, shipId);
//    assert(shotHit == true && shipDestroyed == true && shipId == 4);
//
//    b.display(false);
//    b.display(true);
        
    // Test play function in Game
    const int NTRIALS = 100;

    cout << "Select one of these choices for an example of the game:" << endl;
    cout << "  1.  A mini-game between two mediocre players" << endl;
    cout << "  2.  A mediocre player against a human player" << endl;
    cout << "  3.  A " << NTRIALS
         << "-game match between a Mediocre and an Awful player, with no pauses"
         << endl;
    cout << "  4.  A Human player against another Human player" << endl;
    cout << "  5.  A " << NTRIALS
         << "-game match between a Mediocre and a Good player, with no pauses"
         << endl;
    cout << "  6.  A " << NTRIALS
         << "-game match between a Good and an Awful player, with no pauses"
         << endl;
    cout << "Enter your choice: ";
    string line;
    getline(cin,line);
    if (line.empty())
    {
        cout << "You did not enter a choice" << endl;
    }
    else if (line[0] == '1')
    {
        Game g(2, 3);
        g.addShip(2, 'R', "rowboat");
        Player* p1 = createPlayer("mediocre", "Popeye", g);
        Player* p2 = createPlayer("mediocre", "Bluto", g);
        cout << "This mini-game has one ship, a 2-segment rowboat." << endl;
        g.play(p1, p2);
        delete p1;
        delete p2;
    }
    else if (line[0] == '2')
    {
        Game g(10, 10);
        addStandardShips(g);
        Player* p1 = createPlayer("mediocre", "Mediocre Midori", g);
        Player* p2 = createPlayer("human", "Shuman the Human", g);
        g.play(p1, p2);
        delete p1;
        delete p2;
    }
    else if (line[0] == '3')
    {
        int nMediocreWins = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("awful", "Awful Audrey", g);
            Player* p2 = createPlayer("mediocre", "Mediocre Mimi", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p2)
                nMediocreWins++;
            delete p1;
            delete p2;
        }
        cout << "The mediocre player won " << nMediocreWins << " out of "
             << NTRIALS << " games." << endl;
          // We'd expect a mediocre player to win most of the games against
          // an awful player.  Similarly, a good player should outperform
          // a mediocre player.
    }
    else if (line[0] == '4')
    {
        Game g(10, 10);
        addStandardShips(g);
        Player* p1 = createPlayer("human", "Player 1", g);
        Player* p2 = createPlayer("human", "Player 2", g);
        g.play(p1, p2);
        delete p1;
        delete p2;
    }
    else if (line[0] == '5')
    {
        int nGoodWins = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("good", "Good Grace", g);
            Player* p2 = createPlayer("mediocre", "Mediocre Mimi", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p1)
                nGoodWins++;
            delete p1;
            delete p2;
        }
        cout << "The good player won " << nGoodWins << " out of "
             << NTRIALS << " games." << endl;
          // We'd expect a mediocre player to win most of the games against
          // an awful player.  Similarly, a good player should outperform
          // a mediocre player.
    }
    else if (line[0] == '6')
    {
        int nGoodWins = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("good", "Good Grace", g);
            Player* p2 = createPlayer("awful", "Awful Amy", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p1)
                nGoodWins++;
            delete p1;
            delete p2;
        }
        cout << "The good player won " << nGoodWins << " out of "
             << NTRIALS << " games." << endl;
          // We'd expect a mediocre player to win most of the games against
          // an awful player.  Similarly, a good player should outperform
          // a mediocre player.
    }
    else
    {
       cout << "That's not one of the choices." << endl;
    }
}

