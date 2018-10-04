#include "function.h"

///CONSTRUCTOR

Node::Node()
{
	const int m = 40;
	for (int i = 0; i < m; i++)
		next[i] = NULL;
}

///DESTRUCTOR

Trie::~Trie()
{
	init();
}

///OTHER FUNCTION

int ascii(char x)
{
	if (x >= 'a' && x <= 'z')
		return x - 'a';
	if (x >= 'A' && x <= 'Z')
		return x - 'A';
	if (x >= '0' && x <= '9')
		return x - '0' + ('z' - 'a') + 1;
	return -1;
}

void toLower(string &s)
{
	int l = s.length();
	for (int i = 0; i < l; i++)
		if (s[i] >= 'A' && s[i] <= 'Z')
			s[i] += ('a' - 'A');
}

bool isNum(string s)
{
	int l = s.length();
	for (int i = 0; i < l; i++)
		if (s[i] < '0' || s[i] > '9')
			return false;
	return true;
}

bool valid(string s)
{
	int i, l = s.size();
	for (i = 0; i<l; i++)
		if (ascii(s[i]) == -1) return 0;
	return 1;
}

int isPunc(char x)
{
	if (x == '.' || x == '?' || x == '!')
		return 1;
	return 0;
}

void check_history(ostream &f, vector<string> history, int len_q) {
	int i;
	for (i = 0; i < len_q; i++) f << history[i] << "\n";
	f << "\n";
}

///SLIT AND INSERT

void Trie::split(int order_file)
{
	int l = text[order_file].length(), tmp;
	string k = "";
	for (int i = 0; i < l; i++)
	{
		tmp = ascii(text[order_file][i]);
		if (k != "" && tmp == -1 && text[order_file][i] != '\'')
		{
			insert(k, i - 1, order_file);
			k = "";
		}
		else if (tmp != -1)
			k += text[order_file][i];
	}
}

void Trie::insert(string key, int pos_word, int order_file)
{
	if (root == NULL)
		root = new Node;
	int l = key.length(), tmp;
	Node* cur = root;
	for (int i = 0; i < l; i++)
	{
		tmp = ascii(key[i]);
		if (cur->next[tmp] == NULL)
		{
			cur->next[tmp] = new Node;
			cur->next[tmp]->pre = cur;
			cur->next[tmp]->h = cur->h + 1;
			cur->next[tmp]->num = tmp;
		}
		cur = cur->next[tmp];
		if (i == (l - 1))
		{
			if (cur->file == NULL || cur->file->order != order_file)
				cur->insertOrderFile(order_file);
			cur->file->insertOrderWord(pos_word);
			cur->occur++;
		}
	}
}

void Node::insertOrderFile(int order_file)
{
	Order_file* cur = new Order_file;
	cur->nextfile = file;
	file = cur;
	cur->order = order_file;
}

void Order_file::insertOrderWord(int pos_word)
{
	Order_word* cur = new Order_word;
	cur->nextword = word;
	word = cur;
	cur->val = pos_word;
}

void Trie::getTitleLength(int x, int n)
{
	title_length[n] = x;
}

void Trie::getContentLength(int x, int n)
{
	content_length[n] = x;
}

///Initialize trie

void Trie::initRecursion(Node* &cur)
{
	if (cur == NULL)
		return;
	for (int i = 0; i < 40; i++)
		initRecursion(cur->next[i]);
	cur->file->init(cur->file);
	delete cur;
	cur = NULL;
}

void Order_file::init(Order_file* &cur)
{
	Order_word* tmp = NULL;
	Order_file* tmp1 = NULL;
	while (cur != NULL)
	{
		tmp = NULL;
		while (cur->word != NULL)
		{
			tmp = cur->word->nextword;
			delete cur->word;
			cur->word = tmp;
		}
		tmp1 = cur->nextfile;
		delete cur;
		cur = tmp1;
	}
}

void Trie::init()
{
	initRecursion(root);
}

void Trie::initOutput()
{
	for (int i = 0; i < n; i++)
		output[i] = "";
}

///INPUT

void Trie::inputData(string s)
{
	text[n] = s;
	n++;
}

void Trie::inputFileName()
{
	fstream fi;
	fi.open("file_name.txt", ios::in);
	for (int i = 0; i < n; i++)
	{
		getline(fi, file_name[i]);
		getline(fi, file_type[i]);
		file_type[i].erase(0, 1);
	}
	fi.close();
}

///SEARCH

Node* Trie::search(string key)
{
	if (root == NULL)
		return NULL;
	Node *cur = root;
	int l = key.length(), ord;
	for (int i = 0; i < l; i++)
	{
		ord = ascii(key[i]);
		if (cur->next[ord] == NULL) return NULL;
		else cur = cur->next[ord];
	}
	return cur;
}

///QUERY

void Trie::queryEXACT(ostream &f, string key[], int size_key)
{
	clock_t start = clock();
	int *check = new int[n + 10], p, count = 0;
	for (int i = 0; i < n; i++)
		check[i] = 0;
	Node* res;
	Order_file* fres;
	Order_word* fword;
	bool found = false;
	for (int i = 0; i < size_key; i++)
	{
		int l = key[i].length(), tmp, j;
		string k = "";
		for (j = 0; j < l; j++)
		{
			tmp = ascii(key[i][j]);
			if (k != "" && tmp == -1 && key[i][j] != '\'')
				break;
			else if (tmp != -1)
				k += key[i][j];
		}
		res = search(k);
		fres = res->file;
		while (fres != NULL)
		{
			fword = fres->word;
			while (fword != NULL)
			{
				if (fword->val + (l - j) + 1 <= content_length[fres->order])
				{
					tmp = fword->val + 1;
					for (p = j; p < l; p++)
					{
						if (tolower(text[fres->order][tmp]) != tolower(key[i][p]))
							break;
						tmp++;
					}
					if (p == l)
					{
						check[fres->order]++;
						storeSentence(fres->order, fword->val);
						if (check[fres->order] == size_key)
							found = true;
					}
				}
				if (check[fres->order] > 0)
					break;
				fword = fword->nextword;
			}
			fres = fres->nextfile;
		}
	}
	f << "Running time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "\n";
	if (found == false)
	{
		f << "Cannot find the article sastisfied your request\n";
		return;
	}
	f << "All the keywords inputed appeared in article:\n";
	for (int i = 0; i < n; i++)
		if (check[i] == size_key)
		{
			f << file_name[i] << ": " << output[i] << "\n";
			count++;
		}
	f << "About " << count << " result(s)\n";
	delete[] check;
}

void Trie::queryFILETYPE(ostream &f, string key[], int size_key)
{
	clock_t start = clock();
	int found = 0, *check, count = 0;
	check = new int[n + 10];
	for (int i = 0; i < n; i++)
		check[i] = -1;
	for (int i = 0; i < size_key; i++)
		for (int j = 0; j < n; j++)
			if (key[i] == file_type[j])
			{
				found = 1;
				check[j] = 1;
			}
	f << "Running time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "\n";
	if (found == 0)
	{
		f << "Cannot find the article sastisfied your request\n";
		return;
	}
	else
		f << "All the keywords inputed appeared in article:\n";
	for (int i = 0; i < n; i++)
		if (check[i] == 1)
		{
			f << file_name[i] << "\n";
			count++;
		}
	f << "About " << count << " result(s)\n";
	delete[] check;
}

void Trie::queryPRICE(ostream &f, string key[], int size_key)
{
	clock_t start = clock();
	int *check = new int[n + 10], count = 0;
	bool found = false;
	for (int i = 0; i < n; i++)
		check[i] = 0;
	Node* res;
	Order_file* fres;
	Order_word* fword;
	for (int i = 0; i < size_key; i++)
	{
		res = search(key[i]);
		if (res == NULL)
		{
			f << "Running time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "\n";
			f << "Cannot find the article sastisfied your request\n";
			return;
		}
		if (isNum(key[i]))
		{
			fres = res->file;
			while (fres != NULL)
			{
				fword = fres->word;
				while (fword != NULL)
				{
					if (fword->val - res->h >= 0 && text[fres->order][fword->val - res->h] == '$')
					{
						check[fres->order]++;
						storeSentence(fres->order, fword->val);
						if (check[fres->order] == size_key)
							found = true;
						break;
					}
					fword = fword->nextword;
				}
				fres = fres->nextfile;
			}
		}
		else
		{
			fres = res->file;
			while (fres != NULL)
			{
				check[fres->order]++;
				storeSentence(fres->order, fres->word->val);
				if (check[fres->order] == size_key)
					found = true;
				fres = fres->nextfile;
			}
		}
	}
	f << "Running time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "\n";
	if (found == false)
	{
		f << "Cannot find the article sastisfied your request\n";
		return;
	}
	f << "All the keywords inputed appeared in article:\n";
	for (int i = 0; i < n; i++)
		if (check[i] == size_key)
		{
			f << file_name[i] << ": " << output[i] << "\n";
			count++;
		}
	f << "About " << count << " result(s)\n";
	delete[] check;
}

void Trie::queryHASHTAG(ostream &f, string key[], int size_key)
{
	clock_t start = clock();
	int *check = new int[n + 10], count = 0;
	bool found = false;
	for (int i = 0; i < n; i++)
		check[i] = 0;
	Node* res;
	Order_file* fres;
	Order_word* fword;
	for (int i = 0; i < size_key; i++)
	{
		res = search(key[i]);
		if (res != NULL)
		{
			fres = res->file;
			while (fres != NULL)
			{
				fword = fres->word;
				while (fword != NULL)
				{
					if (fword->val - res->h >= 0 && text[fres->order][fword->val - res->h] == '#')
					{
						check[fres->order]++;
						found = true;
						storeSentence(fres->order, fword->val);
						break;
					}
					fword = fword->nextword;
				}
				fres = fres->nextfile;
			}
		}
	}
	f << "Running time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "\n";
	if (found == false)
	{
		f << "Cannot find the article sastisfied your request\n";
		return;
	}
	f << "All the keywords inputed appeared in article:\n";
	for (int i = 0; i < n; i++)
		if (check[i] > 0)
		{
			f << file_name[i] << ": " << output[i] << "\n";
			count++;
		}
	f << "About " << count << " result(s)\n";
	delete[] check;
}

void Trie::queryINTITLE(ostream &f, string key[], int size_key)
{
	clock_t start = clock();
	int *check, found = 0, count = 0;
	check = new int[n + 10];
	Node* res = NULL;
	Order_file* fres;
	Order_word* fword;
	for (int i = 0; i < n; i++)
		check[i] = 0;
	for (int i = 0; i < size_key; i++)
	{
		res = search(key[i]);
		if (res != NULL)
		{
			fres = res->file;
			while (fres != NULL)
			{
				fword = fres->word;
				while (fword != NULL)
				{
					if (fword->val <= title_length[fres->order])
					{
						check[fres->order]++;
						storeSentence(fres->order, fword->val);
						found = 1;
						break;
					}
					fword = fword->nextword;
				}
				fres = fres->nextfile;
			}
		}
	}
	f << "Running time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "\n";
	if (found == 0)
	{
		f << "Cannot find the article sastisfied your request\n";
		return;
	}
	f << "All the keywords inputed appeared in article:\n";
	for (int i = 0; i < n; i++)
		if (check[i] > 0)
		{
			f << file_name[i] << ": " << output[i] << "\n";
			count++;
		}
	f << "About " << count << " result(s)\n";
	delete[] check;
}

void Trie::queryAND(ostream &f, string key[], int l, int *queryAndOr) {
	vector <bool> check(n, true);
	clock_t start = clock();
	int i, j;
	for (i = 0; i<l; i++) {
		Node *tmp = search(key[i]);
		if (tmp != NULL) {
			vector <bool> compare(n, false);
			j = 0;
			Order_file *cur = tmp->file;
			while (cur != NULL) {
				compare[cur->order] = true;
				storeSentence(cur->order, cur->word->val);
				cur = cur->nextfile;
			}
			for (j = 0; j < n; j++)	check[j] = check[j] & compare[j];
		}
		else 
			return;
	}
	for (i = 0; i < n; i++)
		if (check[i])
			queryAndOr[i] = 2;
}

void Trie::queryOR(ostream &f, string key[], int l, int *queryAndOr, bool &found) {
	int i, j;
	for (i = 0; i<l; i++) {
		Node *tmp = search(key[i]);
		if (tmp != NULL) {
			j = 0;
			Order_file *cur = tmp->file;
			while (cur != NULL) {
				queryAndOr[cur->order] = 1;
				storeSentence(cur->order, cur->word->val);
				cur = cur->nextfile;
			}
			found = true;
		}
	}
}

void Trie::queryMINUS(ostream &f, string key[], bool isminus[], int l) {
	vector <int> check(n, 0);
	clock_t start = clock();
	bool found = false;
	int i, j, count = 0;
	for (i = 0; i < l; i++) {
		Node *tmp = search(key[i]);
		if (tmp != NULL) {
			if (isminus[i] == false) {
				Order_file *cur = tmp->file;
				while (cur != NULL) {
					check[cur->order]++;
					storeSentence(cur->order, cur->word->val);
					if (check[cur->order] == l)
						found = true;
					cur = cur->nextfile;
				}
			}
			else {
				vector <bool> compare(n, false);
				j = 0;
				Order_file *cur = tmp->file;
				while (cur != NULL) {
					compare[cur->order] = true;
					cur = cur->nextfile;
				}
				for (j = 0; j < n; j++)
					if (compare[j] == false)
					{
						check[j]++;
						if (check[j] == l)
							found = true;
					}
			}
		}
		else {
			if (isminus[i] == false) {
				f << "Running time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "\n";
				f << "Cannot find the article sastisfied your request\n";
				return;
			}
			else for (j = 0; j < n; j++)
			{
				check[j]++;
				if (check[j] == l)
					found = true;
			}
		}
	}
	f << "Running time: " << (double)(clock() - start) / CLOCKS_PER_SEC << "\n";
	if (found == false) 
	{
		
		f << "Cannot find the article sastisfied your request\n";
		return;
	}
	f << "The keywords inputed appeared in article:\n";
	for (i = 0; i < n; i++)
		if (check[i] == l)
		{
			f << file_name[i] << ": " << output[i] << "\n";
			count++;
		}
	f << "About " << count << " result(s)\n";
}

void Trie::storeSentence(int f_order, int w_order)
{
	string k = "";
	int i = w_order;
	//Go right
	while (1)
	{
		if (isPunc(text[f_order][i]) == 1 && text[f_order][i + 1] == ' ')
		{
			k = k + text[f_order][i] + " ";
			break;
		}
		k += text[f_order][i++];
	}
	i = w_order - 1;
	while (1)
	{
		if (text[f_order][i] == ' ' && isPunc(text[f_order][i - 1]) == 1)
			break;
		k = text[f_order][i--] + k;
	}

	string tmp = k, sum = "";
	toLower(tmp);
	sum = tmp + "|";
	tmp = output[f_order];
	toLower(tmp);
	sum += tmp;
	int lensum = sum.length(), l = 0, r = 0, lenk = k.length(), *z;
	z = new int[lensum + 10];
	for (int i = 1; i < lensum; i++)
		z[i] = 0;
	z[0] = lensum;
	for (int i = 1; i < lensum; i++)
	{
		if (i <= r)
			z[i] = min(z[i - l], r - i + 1);
		while (i + z[i] < lensum && sum[i + z[i]] == sum[z[i]])
			z[i]++;
		if (z[i] == lenk)
			return;
		if (i + z[i] - 1 > r)
		{
			l = i;
			r = i + z[i] - 1;
		}
	}

	output[f_order] += k;
	delete[]z;
}

void Trie::outputInQueryAndOr(ostream &f, int x)
{
	f << file_name[x] << ": " << output[x] << "\n";
}