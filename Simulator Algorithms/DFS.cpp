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
pair<int, int>currentGoal;
char goalOri;

char orientation = 'N'; // current orientation
int x = 0, y = 0; // current coordinates
int x2, y2;

//---------------------function signatures--------------------
bool isSafe(int X, int Y);
bool isGoal(int x, int y);
void solve(pair<int, int>current, pair<int, int>previous, int distance, vector<vector<Cell>> mapp, char cameFrom);
void initializeMaze(vector<vector<Cell>> &map);
void exploreCell(vector<vector<Cell>> &map, int x, int y);
void moveTo(char direction);
void log(const std::string &text);
void goBack(vector<vector<Cell>> &mapp);
void backMove(char direction);
void moveToShortestPath(char direction);


//---------------------main()--------------
int main(int argc, char *argv[])
{
    leastDistance[{0,0}]=0;

    vector<vector<Cell>> map(mazeSize, vector<Cell>(mazeSize));
    initializeMaze(map);

    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");
    solve({0, 0}, {-1, -1}, 0, map, 'S');
    goBack(map);
}

void goBack(vector<vector<Cell>> &mapp){
    x2 = currentGoal.first;
    y2 = currentGoal.second;
    switch(goalOri){
        case 'N':
            goalOri='S';
            break;
        case 'S':
            goalOri='N';
            break;
        case 'W':
            goalOri = 'E';
            break;
        default:
            goalOri='W';
    }
    while(x2!=0 || y2!=0){
        backMove(previousWay[{x2, y2}]);
    }

    API::turnRight();
    API::turnRight();
    switch(orientation) {
        case 'N':
            orientation = 'S';
            break;
        case 'S':
            orientation = 'N';
            break;
        case 'W':
            orientation = 'E';
            break;
        default:
            orientation = 'W';
    }
    while(!isGoal(x, y)){
        moveToShortestPath(shortestPath[{x, y}]);
        API::setColor(x, y, 'B');
    }

}

void backMove(char direction){
    switch (goalOri) {
        case 'N':
        {
            if(direction=='N'){
                y2++;
                shortestPath[{x2, y2}] = 'S';
            }else if(direction=='E'){
                goalOri='E';
                x2++;
                shortestPath[{x2, y2}] = 'W';
            }
            else if(direction=='W'){
                goalOri='W';
                x2--;
                shortestPath[{x2, y2}] = 'E';
            }
        }
            break;
        case 'E':
        {
            if(direction=='E'){
                x2++;
                shortestPath[{x2, y2}] = 'W';
            }else if(direction=='S'){
                goalOri='S';
                y2--;
                shortestPath[{x2, y2}] = 'N';
            }
            else if(direction=='N'){
                goalOri='N';
                y2++;
                shortestPath[{x2, y2}] = 'S';
            }
        }
            break;
        case 'S':
        {
            if(direction=='S'){
                y2--;
                shortestPath[{x2, y2}] = 'N';
            }else if(direction=='W'){
                goalOri='W';
                x2--;
                shortestPath[{x2, y2}] = 'E';
            }
            else if(direction=='E'){
                goalOri='E';
                x2++;
                shortestPath[{x2, y2}] = 'W';
            }
        }
            break;
        case 'W':
        {
            if(direction=='W'){
                x2--;
                shortestPath[{x2, y2}] = 'E';
            }else if(direction=='N'){
                goalOri='N';
                y2++;
                shortestPath[{x2, y2}] = 'S';
            }
            else if(direction=='S'){
                goalOri='S';
                y2--;
                shortestPath[{x2, y2}] = 'N';
            }
        }
            break;
        default:
            x2 = 0, y2 = 0;
    }
}

void moveToShortestPath(char direction){
    switch (orientation) {
        case 'N':
        {
            if(direction=='N'){
                API::moveForward();
                y++;
            }else if(direction=='E'){
                API::turnRight();
                orientation='E';
                API::moveForward();
                x++;
            }
            else if(direction=='W'){
                API::turnLeft();
                orientation='W';
                API::moveForward();
                x--;
            }
        }
            break;
        case 'E':
        {
            if(direction=='E'){
                API::moveForward();
                x++;
            }else if(direction=='S'){
                API::turnRight();
                orientation='S';
                API::moveForward();
                y--;
            }
            else if(direction=='N'){
                API::turnLeft();
                orientation='N';
                API::moveForward();
                y++;
            }
        }
            break;
        case 'S':
        {
            if(direction=='S'){
                API::moveForward();
                y--;
            }else if(direction=='W'){
                API::turnRight();
                orientation='W';
                API::moveForward();
                x--;
            }
            else if(direction=='E'){
                API::turnLeft();
                orientation='E';
                API::moveForward();
                x++;
            }
        }
            break;
        case 'W':
        {
            if(direction=='W'){
                API::moveForward();
                x--;
            }else if(direction=='N'){
                API::turnRight();
                orientation='N';
                API::moveForward();
                y++;
            }
            else if(direction=='S'){
                API::turnLeft();
                orientation='S';
                API::moveForward();
                y--;
            }
        }
            break;
        default:
            x = 0, y = 0;
    }
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

bool isGoal(int X, int Y)
{

    if ((X == 7 || X == 8) && (Y == 7 || Y == 8)){
        currentGoal = {X, Y};
        goalOri = orientation;
        return true;
    }
    else
        return false;
}

void solve(pair<int, int>current, pair<int, int>previous, int distance, vector<vector<Cell>> mapp, char cameFrom){
    API::setColor(current.first, current.second, 'G');
    if(isGoal(current.first, current.second)  && distance<shortest){
        shortest = distance;
    }
    exploreCell(mapp, current.first, current.second);
    pair<int, int> next = {current.first, current.second+1};
    bool go[4] = {false, false, false, false};
    if(next!=previous && !mapp[current.first][current.second].hasNorthWall() && isSafe(next.first, next.second) &&
       (leastDistance.find(next)==leastDistance.end() || leastDistance[next]>distance+1) && shortest>distance+1){
        leastDistance[next] = distance+1;
        API::setText(next.first, next.second, to_string(distance+1));
        go[0]=true;
    }

    next = {current.first, current.second-1};
    if(next!=previous && !mapp[current.first][current.second].hasSouthWall() && isSafe(next.first, next.second) &&
       (leastDistance.find(next)==leastDistance.end() || leastDistance[next]>distance+1) && shortest>distance+1){
        leastDistance[next] = distance+1;
        API::setText(next.first, next.second, to_string(distance+1));
        go[1]=true;
    }
    next = {current.first+1, current.second};
    if(next!=previous && !mapp[current.first][current.second].hasEastWall() && isSafe(next.first, next.second) &&
       (leastDistance.find(next)==leastDistance.end() || leastDistance[next]>distance+1) && shortest>distance+1){
        leastDistance[next] = distance+1;
        API::setText(next.first, next.second, to_string(distance+1));
        go[2]=true;
    }
    next = {current.first-1, current.second};
    if(next!=previous && !mapp[current.first][current.second].hasWestWall() && isSafe(next.first, next.second) &&
       (leastDistance.find(next)==leastDistance.end() || leastDistance[next]>distance+1 && shortest>distance+1)){
        leastDistance[next] = distance+1;
        API::setText(next.first, next.second, to_string(distance+1));
        go[3]=true;
    }


    /// ----------------------------------------------------------------------------
    next = {current.first, current.second+1};
    if(go[0]){
        moveTo('N');
        leastDistance[next] = distance+1;
        previousWay[next] = 'S';
        solve(next, current, distance+1, mapp, 'S');
    }
    next = {current.first, current.second-1};
    if(go[1]){
        moveTo('S');
        leastDistance[next] = distance+1;
        previousWay[next] = 'N';
        solve(next, current, distance+1, mapp, 'N');
    }
    next = {current.first+1, current.second};
    if(go[2]){
        moveTo('E');
        leastDistance[next] = distance+1;
        previousWay[next] = 'W';
        solve(next, current, distance+1, mapp, 'W');
    }
    next = {current.first-1, current.second};
    if(go[3]){
        moveTo('W');
        leastDistance[next] = distance+1;
        previousWay[next] = 'E';
        solve(next, current, distance+1, mapp, 'E');
    }
    if(previous.first!=-1)
        moveTo(cameFrom);
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
            API::setText(i, j, to_string(-1));
            map[i][j].setFloodFillCost(floodFillCost);
        }
    }

    map[0][0].setSouthWall(true); // leads to errors
}

void exploreCell(vector<vector<Cell>> &map, int x, int y)
{
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