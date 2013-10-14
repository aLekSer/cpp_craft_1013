#pragma once
#include <iostream>
#include <fstream>

using namespace std;

class ReversedStr
{
	ifstream fs;
	ofstream of;
	string input;
	string lowerInput;
	string current;
	int count;
public:
	static const string path;
	ReversedStr(string fileName); //read 1st line

	string toLower(string);

	int findNext(); //read next line and output y/n, stop after -1
	int findAll();
	~ReversedStr(void);
};