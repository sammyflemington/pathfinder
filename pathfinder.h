#ifndef _PATHFINDER_H
#define _PATHFINDER_H

/*
    pathfinder.h
        
    class definition for the pathfinder class.

    assignment: CSCI 262 Project - Pathfinder        

    author: <your name here>
*/

#include <string>
#include <vector>

#include "picture.h"

using namespace std;

/******************************************************************************
   class pathfinder

   Given access to a source of topographical data (a file specifying width and
   height and listing elevation data for the resulting grid), a pathfinder
   object can:
     - read in the data
     - create an image showing the data as a grayscale image, with gray level 
       representing relative elevation
     - compute and overlay the optimal path from all western starting points
     - compute and overlay the single best optimal path from west to east
     - save the image data

******************************************************************************/

class pathfinder {
public:
    // constructor - optional, in case you need to initialize anything
    pathfinder();

    // our only public method: do everything and return the best path cost
    int run(string data_file_name, string save_file_name, bool use_recursion);

private:
    // TODO: put your internal data, methods, etc. here; the ones shown 
    //  below are suggested, but you can modify/add as desired (more methods
    //  are probably a good idea!)

    int _height, _width;                     // height and width of map
    bool _use_recursion;
    vector<vector<int>> _elevations;         // raw data from file
    Picture _image;                          // image to save at the end
    int _optCostToEast(int x, int y, vector<int> &pc, int pathStart);
    vector <int> _computeCostDynamic(int x, int y, vector<vector< int > > &costs);
    void _draw_path(int startY, vector<vector<int> > &costs, vector<vector<int> >&dirs, char col);
    bool _read_data(string data_file_name);  // read in raw data
    void _draw_map();                        // draw the grayscale elevation map
    int _draw_paths();                       // find optimal paths, returning the most optimal cost
    void _save_image(string save_file_name); // save the image as a .png file
};

#endif
