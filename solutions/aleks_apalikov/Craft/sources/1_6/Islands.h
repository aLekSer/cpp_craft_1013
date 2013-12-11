#pragma once
#include <fstream>
#include <vector>

using namespace std;

enum {
	water = 0, 
	land = 1,
	forbidden = 4
};

void make_water(size_t pos_x, size_t pos_y, vector<vector<int>>& map); // all lower points
class Islands
{
	ifstream fs;
	ofstream of;
	size_t rows;
	size_t columns;
	vector<vector<int>> Map;
	void Trace();
public:
	Islands(const string& f = "Input3.txt");
	int Count();
	~Islands(void);
};
