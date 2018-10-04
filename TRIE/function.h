#pragma once

#include<iostream>
#include<cstdio>
#include<cstring>
#include<string>
#include<string.h>
#include<algorithm>
#include<cstdlib>
#include<fstream>
#include<filesystem>
#include<experimental/filesystem>
#include<time.h>
#include<ctime>
#include<vector>
#define pb push_back

using namespace std;
using namespace std::experimental::filesystem; //Read all file

											   ///STRUCTURE

struct Order_word //Mark the position of the word in the paragraph
{
	int val = -1; //val: The position in file
	Order_word* nextword = NULL;
};

struct Order_file //The order of file
{
	int order = -1;
	Order_word* word = NULL; //root of list of position of words
	Order_file* nextfile = NULL;
	void init(Order_file*&);
	void insertOrderWord(int);
};

struct Node
{
	///DATA
	int num = -1;
	//means what it is. For example: num = 0 means it is 'a', 1 means it is 'b'
	Node* next[40];
	//26 alphabet + 10 number
	Node* pre = NULL;
	//Previous node
	int occur = 0, h = 0;
	//occur: increase number occurence of that word (increase once when reaching the last character)
	//h: is the length of the word count from the beginning to the character ! h = pre->h+1
	Order_file* file = NULL;
	//List of file contain list position of word occurs in n-order file
	//Root of list of file
	///FUNCTION
	Node(); //all next[] = NULL
	void insertOrderFile(int);
};

class Trie
{
private:
	Node* root = NULL;
	string text[3000];
	//Store all the string input
	string file_name[3000];
	//Store string file names in order to minimize pass valuables
	string file_type[3000];
	//Store file types
	int n = 0, title_length[3000], content_length[3000];
	//Number of text, title length, content length
	string output[3000];
	//Use to store OUTPUT sentences
public:
	~Trie();
	///Initialize trie
	void initRecursion(Node*&);
	void init();
	void initOutput();

	///Output sentences
	void storeSentence(int, int);
	void outputInQueryAndOr(ostream &, int);

	///SPLIT AND INSERT INTO TRIE
	void split(int);
	void insert(string, int, int);
	void getTitleLength(int, int);
	void getContentLength(int, int);
	//string: word
	//int: the order of file
	//int: the position of word

	Node* search(string); //return where NODE contain KEY

	///INPUT
	void inputData(string);
	//string: text
	void inputFileName(); //For 0 -> n-1 get filename

	///QUERY
	void queryAND(ostream&, string[], int, int*);  //array of string (inputed keywords),  int: array size, int*: the array to check files satisfy query
	void queryOR(ostream&, string[], int, int*, bool&);	//array of string (inputed keywords),  int: array size, int*: the array to check files satisfy query, bool & check if having satisfied resuld or not
	void queryPRICE(ostream&, string[], int); //AND
	void queryEXACT(ostream&, string[], int); //AND
	void queryFILETYPE(ostream&, string[], int); //OR
	void queryHASHTAG(ostream&, string[], int); //OR
	void queryINTITLE(ostream&, string[], int); //OR
	void queryMINUS(ostream&, string[], bool[], int); //AND
};

///OTHER FUNCTION
int ascii(char);
void toLower(string&);
bool isNum(string);
bool valid(string);
int isPunc(char);
void check_history(ostream&, vector<string>, int);