//GROUP 04
//Member name: Bui Quoc Huy, Luu Duc Tuan, Nguyen Mach Thanh Vy, Thai My Huong
//PROJECT: SEARCH ENGINE
///QUERY DONE
//QUERY AND
//QUERY OR
//QUERY INTITLE (+OR)
//QUERY PLUS. Ex: Peanut Butter +and Jam
//QUERY FILETYPE
//QUERY PRICE (+AND)
//QUERY HASHTAG (+OR)
//QUERY EXACT (+AND)
//QUERY MINUS (+AND). Ex: Manchester -united

#include "function.h"

int main()
{
	clock_t start = clock();
	Trie content, stopword;
	string title_name, content_name, file_name, tmp;
	int n = 0;
	fstream fi, fo;
	bool isminus[100], found;
	string key[100], keyInput = "", recursion = "", k;
	int size_key, l, numtmp, findChar, checkExact, len_k = 0, len_q = 0, count;
	vector <string> history;
	int *queryAndOr;

	//INPUT DATA + OUTPUT FILENAME
	fo.open("file_name.txt", ios::out);
	for (auto& p : directory_iterator("data")) //Data files put in folder "data"
	{
		fo << p.path().filename() << "\n";
		fo << p.path().extension() << "\n";
		fi.open(p.path(), ios::in); //p.path() : file name
		getline(fi, title_name);
		title_name = ". " + title_name + ". ";
		content.getTitleLength(title_name.length(), n);
		content_name = title_name;
		while (getline(fi, tmp))
			content_name += tmp + ". ";
		content.getContentLength(content_name.length(), n);
		fi.close();
		content.inputData(content_name);
		content.split(n);
		n++;
	}
	fo.close();

	//CHECK ARRAY QUERY AND + OR
	queryAndOr = new int[n + 10];
	for (int i = 0; i < n; i++)
		queryAndOr[i] = 0;

	//GET FILENAME
	content.inputFileName();

	//Check time data
	cout << "\t \t \t \t \t \t SEARCH ENGINE GROUP 4\n";
	printf("Loading time: %.2fs\n", (double)(clock() - start) / CLOCKS_PER_SEC);

	//Loading stopword
	fi.open("stopwords.txt", ios::in);
	keyInput = "";
	while (getline(fi, tmp))
		keyInput += tmp + " ";
	fi.close();
	stopword.inputData(keyInput);
	stopword.split(0);

	//CLASSIFY QUERY

	cout << "exit <- Close program\n";
	cout << "INPUT KEY: ";
	getline(cin, keyInput);
	toLower(keyInput);
	while (keyInput != "exit")
	{
		for (int i = 0; i < n; i++)
			queryAndOr[i] = 0;
		if (len_q < 10) 
		{
			len_q++;
			history.pb(keyInput);
		}
		else {
			history.erase(history.begin());
			history.pb(keyInput);
		}
		//SPLIT
		memset(isminus, false, sizeof(isminus));
		found = false;
		content.initOutput();
		size_key = 0;
		keyInput = " " + keyInput;
		keyInput += " ";
		l = keyInput.length();
		for (int i = 0; i < l; i++)
		{
			numtmp = ascii(keyInput[i]);
			if (i + 1 < l && keyInput[i] == '-' && k == "" && ascii(keyInput[i+1]) != -1)
			{
				isminus[size_key] = true;
				found = true;
			}
			len_k = k.length();
			if ((k != "" && numtmp == -1 && keyInput[i] != '\'') && (stopword.search(k) != NULL && i - len_k - 1 >= 0 && keyInput[i - len_k - 1] != '+' && keyInput[i - len_k - 1] != '-'))
				k = "";
			if (k != "" && numtmp == -1 && keyInput[i] != '\'')
			{
				key[size_key++] = k;
				k = "";
			}
			else if (numtmp != -1)
				k += keyInput[i];
		}
		findChar = keyInput.find("#"); //OR
		if (findChar != -1)
		{
			cout << "QUERY HASHTAG\n";
			content.queryHASHTAG(cout, key, size_key);
		}
		else
		{
			findChar = keyInput.find("$"); //AND
			if (findChar != -1)
			{
				cout << "QUERY PRICE\n";
				content.queryPRICE(cout, key, size_key);
			}
			else
			{
				if (found == true)
				{
					cout << "QUERY MINUS\n";
					content.queryMINUS(cout, key, isminus, size_key);
				}
				else
				{
					findChar = keyInput.find("\"");
					if (findChar != -1)
					{
						size_key = 0;
						checkExact = 0;
						k = "";
						for (int i = 0; i < l; i++)
						{
							numtmp = ascii(keyInput[i]);
							if (checkExact != 0 && keyInput[i] == '\"')
							{
								checkExact = 0;
								key[size_key++] = k;
								k = "";
							}
							else if (checkExact != 0)
								k += keyInput[i];
							else if (keyInput[i] == '\"')
								checkExact = 1;
							else if (k != "" && numtmp == -1 && keyInput[i] != '\'')
							{
								key[size_key++] = k;
								k = "";
							}
							else if (numtmp != -1)
								k += keyInput[i];
						}
						cout << "QUERY EXACT\n";
						content.queryEXACT(cout, key, size_key);
					}
					else
						if (size_key > 0 && key[0] == "filetype")
						{
							cout << "QUERY FILETYPE\n";
							content.queryFILETYPE(cout, key, size_key);
						}
						else
							if (size_key > 0 && key[0] == "intitle")
							{
								cout << "QUERY INTITLE\n";
								content.queryINTITLE(cout, key, size_key);
							}
							else
							{
								found = false;
								count = 0;
								start = clock();
								cout << "QUERY AND + OR\n";
								content.queryOR(cout, key, size_key, queryAndOr, found);
								content.queryAND(cout, key, size_key, queryAndOr);
								if (found == false)
									cout << "Running time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "\n" << "Cannot find the article sastisfied your request\n";
								else
								{
									cout << "Running time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "\n";
									for (int i = 0; i < n; i++)
										if (queryAndOr[i] == 2)
										{
											content.outputInQueryAndOr(cout, i);
											count++;
										}
									for (int i = 0; i < n; i++)
										if (queryAndOr[i] == 1)
										{
											content.outputInQueryAndOr(cout, i);
											count++;
										}
									cout << "About " << count << " result(s)\n";
								}
							}
				}
			}
		}
		cout << "ENTER TO CONTINUE ...\n";
		cin.get();
		system("CLS");
		cout << "\t \t \t \t \t \t SEARCH ENGINE GROUP 4\n";
		cout << "exit <- Close program\n";
		cout << "HISTORY SEARCH\n";
		check_history(cout, history, len_q);
		cout << "INPUT KEY: ";
		getline(cin, keyInput);
		toLower(keyInput);
	}
	return 0;
}