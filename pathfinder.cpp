/*
    pathfinder.cpp
        
    Method implementations for the pathfinder class.
    
    assignment: CSCI 262 Project - Pathfinder        

    author: Samuel Flemington
*/

#include <iostream>
#include "pathfinder.h"
#include <fstream>
#include <math.h>
using namespace std;

// constructor - optional, in case you need to initialize anything
pathfinder::pathfinder() {

}
const int EAST = 0, NORTH = 1, SOUTH = 2;
// public run() method - invokes private methods to do everything;
// returns the total elevation change cost of the best optimal path.
int pathfinder::run(string data_file_name, string save_file_name, bool use_recursion) {

    _use_recursion = use_recursion;
    if (!_read_data(data_file_name)) {
        cout << "Error reading map data from \"" << data_file_name << "\"!" << endl;
        return -1;
    }

    _draw_map();

    int best_cost = _draw_paths();

    _save_image(save_file_name);

    return best_cost;
}

/*******************/
/* PRIVATE METHODS */
/*******************/

// _read_data - read and store width, height, and elevation data from the
// provided data file; return false if something goes wrong
bool pathfinder::_read_data(string data_file_name) {
    // open input file
    ifstream fin(data_file_name);
    if (fin.fail()){
        return false;
    }
    // read in and store width and height
    string temp;
    // throw away unneeded text in temp variable
    fin >> temp;
    fin >> _width;
    fin >> temp;
    fin >> _height;

    // read in and store elevation data
    int value = 0;
    vector<int> column (_height, 0);
    for (int i = 0; i < _width; i++) {
        _elevations.push_back(column);
    }
    for (int j = 0; j < _height; j++){
        for (int i = 0; i < _width; i++){
            fin >> value;
            _elevations.at(i).at(j) = value;
        }
    }

    // close input file
    fin.close();

    // return true if everything worked
    return true;
}

// _draw_map - draw the elevation data as grayscale values on our Picture
// object.
void pathfinder::_draw_map() {
    // determine min/max elevation values in map
    int max = _elevations.at(0).at(0);
    int min = max;

    for (int i = 0; i < _width; i++){
        for (int j = 0; j < _height; j++){
            int value = _elevations.at(i).at(j);
            if (value > max){
                max = value;
            }else if (value < min){
                min = value;
            }
        }
    }
    // from min, max, determine a scaling factor for data to shift
    // elevations into 0 - 255 range
    double diff = max-min;
    double factor = 255./diff;

    // for each elevation point, determine its grayscale value
    for (int i = 0; i < _width; i++) {
        for (int j = 0; j < _height; j++) {
            int r, g, b;
            // interpolation equations by Carlos Barcellos (via stackoverflow)
            int col1r = 25, col1g = 115, col1b = 30, col2r = 250, col2g = 70, col2b= 10;
            int val = _elevations.at(i).at(j) - min;
            double frac = (val*factor)/255.;
            r = round((col2r - col1r)*frac) + col1r;
            g = round((col2g - col1g)*frac) + col1g;
            b = round((col2b - col1b)*frac) + col1b;

            _image.set(j, i, r, g, b);
        }
    }
}

// _draw_paths() - find and draw optimal paths from west to east using either recursion or dynamic programming
// return the best optimal path cost
int pathfinder::_draw_paths() {
    if (_use_recursion) {
        // Recursive mode
        vector<int> pathCosts;
        for (int i = 0; i < _height; i++) {
            pathCosts.push_back(0);
            pathCosts.at(i) = _optCostToEast(0, i, pathCosts, i);
        }
        int min = pathCosts.at(0);
        for (int i = 0; i < _height; i++) {
            if (pathCosts.at(i) < min) {
                min = pathCosts.at(i);
            }
        }
        return min;
    }else{
        // Dynamic Mode
        vector< vector< int > > costs(_width, vector<int> (_height));
        vector< vector< int > > dirs(_width, vector<int> (_height));

        // Fill last column with 0
        for (int i = 0; i < _height; i++){
            costs.at(_width-1).at(i) = 0;
        }
        // Loop through each coord on column and compute min cost to arrive at location
        for (int i = _width-2; i >= 0; i--){
            for (int j = 0; j < _height; j++){
                vector<int> cheapest = _computeCostDynamic(i, j, costs);
                costs.at(i).at(j) = cheapest.at(0);
                dirs.at(i).at(j) = cheapest.at(1);
            }
        }

        // Now draw all optimal paths in blue
        int mincost = 99999;
        int mincostY = 0;
        for (int y = 0; y < _height; y++){
            if (costs.at(0).at(y) < mincost){
                mincost = costs.at(0).at(y);
                mincostY = y;
            }
            _draw_path(y, costs, dirs, 'b');
        }
        // draw best path
        _draw_path(mincostY, costs, dirs, 'y');
        return mincost;
    }
}
void pathfinder::_draw_path(int startY, vector<vector<int> > &costs, vector<vector<int> >&dirs, char col){
    // draw the optimal single path from this starting y position on the westmost edge
    // get rgb values
    int r = 0, g = 0, b = 0;
    if (col == 'b'){
        b = 255;
    }else if (col == 'y'){
        r = 255;
        g = 255;
    }
    // Set pixel values
    int y = startY;
    for (int x = 0; x < _width; x++){
        _image.set(y, x, r, g, b);
        switch(dirs.at(x).at(y)){
            case NORTH:
                y -= 1;
                break;
            case SOUTH:
                y += 1;
        }
    }
}
vector <int> pathfinder::_computeCostDynamic(int x, int y, vector<vector< int > > &costs){
    // returns {cost, direction}
    vector<int> output = {9999999, EAST};
    int costNorth = 9999999;
    int costSouth = 9999999;
    int costEast = abs(_elevations.at(x+1).at(y) - _elevations.at(x).at(y)) + costs.at(x+1).at(y);
    if (y < _height-1){ // if can go south
        costSouth = abs(_elevations.at(x+1).at(y+1) - _elevations.at(x).at(y))+ costs.at(x+1).at(y+1);
    } // if can go north
    if (y > 0){
        costNorth = abs(_elevations.at(x+1).at(y-1) - _elevations.at(x).at(y))+ costs.at(x+1).at(y-1);
    }

    int costBest = min(costEast, min(costNorth, costSouth));
    output.at(0) = costBest;

    if (costBest == costSouth){
        output.at(1) = SOUTH;
    }else if (costBest == costNorth){
        output.at(1) = NORTH;
    }

    return output;
}

// _save_image - we'll do this one for you :)
void pathfinder::_save_image(string save_file_name) {
    _image.save(save_file_name);
}

int pathfinder::_optCostToEast(int x, int y, vector<int> &pc, int pathStart){
    // can go right
    int x2 = x + 1, y2;
    int costRight, costUp, costDown;
    if (x < _width-1){
        y2 = y;
        costRight = abs(_elevations.at(x).at(y) - _elevations.at(x2).at(y2)) + _optCostToEast(x2, y2, pc, pathStart);
    }else{
        return 0;
    }
    // can go up+right
    if (y > 0){
        y2 = y - 1;
        costUp = abs(_elevations.at(x).at(y) - _elevations.at(x2).at(y2)) + _optCostToEast(x2, y2, pc, pathStart);
    }else{
        costUp = 9999999;
    }
    // can go down+right
    if (y < _height - 1){
        y2 = y + 1;
        costDown = abs(_elevations.at(x).at(y) - _elevations.at(x2).at(y2)) + _optCostToEast(x2, y2, pc, pathStart);
    }else{
        costDown = 9999999;
    }

    return min(min(costRight, costUp), costDown);
}