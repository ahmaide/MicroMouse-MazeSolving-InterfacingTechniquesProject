#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <stack>
#include <map>
#include "API.h"
#include "Cell.h"

using namespace std;
int mazeSize = 16; // 16X16
int centerX = mazeSize / 2;
int centerY = mazeSize / 2;
map<pair<int, int>, int> leastDistance;
map<pair<int, int>, char> previousWay;
map<pair<int, int>, char> shortestPath;
int shortest = 9999;

char orientation = 'N'; // current orientation
int x = 0, y = 0; // current coordinates

//---------------------function signatures--------------------
bool isSafe(int X, int Y);
bool isGoal(int x, int y);
void solve(vector<vector<Cell>> mapp);
void initializeMaze(vector<vector<Cell>> &map);
void exploreCell(vector<vector<Cell>> &map, int x, int y);
void moveTo(char direction);
bool check(char dir, vector<vector<Cell>> mapp);
void log(const std::string &text);

//---------------------main()--------------
int main(int argc, char *argv[])
{
    leastDistance[{0,0}]=0;

    vector<vector<Cell>> map(mazeSize, vector<Cell>(mazeSize));
    initializeMaze(map);

    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");
    log("Startttttttttttting");
    solve(map);
}
void log(const std::string &text)
{
    std::cerr << text << std::endl;
}
//---------------------- function defintions-------------------------------
bool isSafe(int X, int Y)
{
    if (X < 0 || X > mazeSize - 1)
        return false;
    if (Y < 0 || Y > mazeSize - 1)
        return false;
    return true;
}

bool isGoal(int x, int y)
{

    return ((x == 7 || x == 8) && (y == 7 || y == 8));
}

bool check(char dir, vector<vector<Cell>> mapp){
    switch (dir) {
        case 'l':
        {
            if(orientation=='N'){
                if(isSafe(x-1, y) && !mapp[x][y].hasWestWall())
                {
                    x--;
                    orientation = 'W';
                    return true;

                }
                else
                    return false;
            }
            else if(orientation=='S'){
                if(isSafe(x+1, y) && !mapp[x][y].hasEastWall()){
                    x++;
                    orientation = 'E';
                    return true;
                }

                else
                    return false;
            }
            else if(orientation=='E'){
                if(isSafe(x, y+1) && !mapp[x][y].hasNorthWall()){
                    y++;
                    orientation = 'N';
                    return true;
                }

                else
                    return false;
            }
            else{
                if(isSafe(x, y-1) && !mapp[x][y].hasSouthWall()){
                    y--;
                    orientation = 'S';
                    return true;
                }

                else
                    return false;
            }
        }
        break;
        case 'f':
        {
            if(orientation=='N'){
                if(isSafe(x, y+1) && !mapp[x][y].hasNorthWall()){
                    y++;
                    return true;
                }

                else
                    return false;
            }
            else if(orientation=='S'){
                if(isSafe(x, y-1) && !mapp[x][y].hasSouthWall()){
                    y--;
                    return true;
                }

                else
                    return false;
            }
            else if(orientation=='E'){
                if(isSafe(x+1, y) && !mapp[x][y].hasEastWall()){
                    x++;
                    return true;
                }

                else
                    return false;
            }
            else{
                if(isSafe(x-1, y) && !mapp[x][y].hasWestWall()){
                    x--;
                    return true;
                }

                else
                    return false;
            }
        }
            break;
        case 'r':
        {
            if(orientation=='N'){
                if(isSafe(x+1, y) && !mapp[x][y].hasEastWall()){
                    x++;
                    orientation = 'E';
                    return true;
                }

                else
                    return false;
            }
            else if(orientation=='S'){
                if(isSafe(x-1, y) && !mapp[x][y].hasWestWall()){
                    x--;
                    orientation = 'W';
                    return true;
                }
                else
                    return false;
            }
            else if(orientation=='E'){
                if(isSafe(x, y-1) && !mapp[x][y].hasSouthWall()){
                    y--;
                    orientation = 'S';
                    return true;
                }

                else
                    return false;
            }
            else{
                if(isSafe(x, y+1) && !mapp[x][y].hasNorthWall()){
                    y++;
                    orientation = 'N';
                    return true;
                }
                else
                    return false;
            }
        }
            break;
        default:
            return false;
    }
}

void solve(vector<vector<Cell>> mapp){
    while(1){
        exploreCell(mapp, x, y);
        if(check('l', mapp)){
            API::turnLeft();
            API::moveForward();

        }
        else{
            if(check('f', mapp)){
                API::moveForward();
            }
            else{
                if(check('r', mapp)){
                    API::turnRight();
                    API::moveForward();
                }
                else{
                    API::turnRight();
                    API::turnRight();
                    switch (orientation) {
                        case 'N':
                            orientation = 'S';
                            break;
                        case 'E':
                            orientation = 'W';
                            break;
                        case 'S':
                            orientation = 'N';
                            break;
                        case 'W':
                            orientation = 'E';
                            break;
                        default:
                            orientation = 'N';

                    }
                }
            }
        }
    }
}

void initializeMaze(vector<vector<Cell>> &map)
{

    for (int i = 0; i < mazeSize; i++)
    {
        map[i].reserve(mazeSize);
    }
    for (int i = 0; i < mazeSize; i++)
    {
        for (int j = 0; j < mazeSize; j++)
        {
            map[i][j].setcellAddress(Coordinates{i, j});
            int minX = std::min(std::abs(i - centerX), std::abs(i - (centerX - 1)));

            int minY = std::min(std::abs(j - centerY), std::abs(j - (centerY - 1)));
            int floodFillCost = minX + minY;
            API::setText(i, j, to_string(0));
            map[i][j].setFloodFillCost(floodFillCost);
        }
    }

    map[0][0].setSouthWall(true); // leads to errors
}

void exploreCell(vector<vector<Cell>> &map, int x, int y)
{
    API::setColor(x, y, 'G');
    // map[x][y].setVisited(); // first visit is straight, then right, then left, then back
    if (map[x][y].getHasBeenExplored())
        return;

    map[x][y].sethasBeenExplored(true);

    switch (orientation)
    {
        case 'N':

            map[x][y].setNorthWall(API::wallFront());

            map[x][y].setEastWall(API::wallRight());
            map[x][y].setWestWall(API::wallLeft());

            break;
        case 'S':

            map[x][y].setSouthWall(API::wallFront());
            map[x][y].setEastWall(API::wallLeft());
            map[x][y].setWestWall(API::wallRight());
            break;
        case 'W':

            map[x][y].setWestWall(API::wallFront());
            map[x][y].setSouthWall(API::wallLeft());
            map[x][y].setNorthWall(API::wallRight());
            break;
        case 'E':

            map[x][y].setEastWall(API::wallFront());
            map[x][y].setNorthWall(API::wallLeft());
            map[x][y].setSouthWall(API::wallRight());
            break;
        default:
            int a = 0;
    }
}

void moveTo(char direction){
    switch (orientation) {
        case 'N':
        {
            if(direction=='N'){
                API::moveForward();
            }else if(direction=='E'){
                API::turnRight();
                orientation='E';
                API::moveForward();
            }
            else if(direction=='W'){
                API::turnLeft();
                orientation='W';
                API::moveForward();
            }
            else{
                API::turnRight();
                API::turnRight();
                orientation='S';
                API::moveForward();
            }
        }
            break;
        case 'E':
        {
            if(direction=='E'){
                API::moveForward();
            }else if(direction=='S'){
                API::turnRight();
                orientation='S';
                API::moveForward();
            }
            else if(direction=='N'){
                API::turnLeft();
                orientation='N';
                API::moveForward();
            }
            else{
                API::turnRight();
                API::turnRight();
                orientation='W';
                API::moveForward();
            }
        }
            break;
        case 'S':
        {
            if(direction=='S'){
                API::moveForward();
            }else if(direction=='W'){
                API::turnRight();
                orientation='W';
                API::moveForward();
            }
            else if(direction=='E'){
                API::turnLeft();
                orientation='E';
                API::moveForward();
            }
            else{
                API::turnRight();
                API::turnRight();
                orientation='N';
                API::moveForward();
            }
        }
            break;
        case 'W':
        {
            if(direction=='W'){
                API::moveForward();
            }else if(direction=='N'){
                API::turnRight();
                orientation='N';
                API::moveForward();
            }
            else if(direction=='S'){
                API::turnLeft();
                orientation='S';
                API::moveForward();
            }
            else{
                API::turnRight();
                API::turnRight();
                orientation='E';
                API::moveForward();
            }
        }
            break;
        default:
            x = 0, y = 0;
    }
}