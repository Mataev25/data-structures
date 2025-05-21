#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

class clientNode {
private:
	string ip_adress;
	string region;
	size_t money_spent;
	clientNode* next;

public:
	clientNode() : ip_adress("0.0.0.0"), region("Russia"), money_spent(0),
	               next(nullptr){}

	clientNode(string IP, string rgn, size_t mnst, clientNode* nxt=nullptr):
		ip_adress(IP), region(rgn), money_spent(mnst), next(nxt){}
	
	void setIP(string IP) { ip_adress = IP; }
	void setRGN(string rgn) { region = rgn; }
	void setMNST(size_t mnst) { money_spent = mnst; }
	void setNext(clientNode* nxt) {next = nxt;}

	string getIP() { return ip_adress; }
	string getRGN() { return region; }
	size_t getMNST() { return money_spent; }
	clientNode* getNext() { return next; }
	

};

class clientList {
private:
	clientNode* first;
	unordered_map<string, int> regionCount;

	void updataRGC(const string& region) {
		regionCount[region]++;
	}
	//массив указателей 
	vector<clientNode*> getNodePointers() {
		vector<clientNode*> pointers;
		clientNode* cur = first;
		while (cur) {
			pointers.push_back(cur);
			cur = cur->getNext();
		}
		return pointers;
	}
	void modiList(vector<clientNode*>& pointers) {
		if (pointers.empty()) {
			first = nullptr;
			return;
		}
		first = pointers[0];
		for (size_t i = 0; i < pointers.size()-1; i++) {
			pointers[i]->setNext(pointers[i + 1]);
		}
		pointers.back()->setNext(nullptr);
	}
	template<typename Compare>
	void merge_sort(vector<clientNode*>& arr, Compare compare) {
		if (arr.size() <= 1) return;
		size_t mid = arr.size() / 2;
		vector<clientNode*> left(arr.begin(), arr.begin() + mid);
		vector<clientNode*> right(arr.begin() + mid, arr.end());

		merge_sort(left, compare);
		merge_sort(right, compare);

		size_t k = 0, i = 0, j = 0;
		for (; i < left.size() && j < right.size();) {
			if (compare(left[i], right[j]))
				arr[k++] = left[i++];
			else
				arr[k++] = right[j++];
		}
		while (i < left.size()) arr[k++] = left[i++];
		while (j < right.size()) arr[k++] = right[j++];
	}
	bool compareIP(const string& ip1, const string& ip2) {
		vector<int> oct1, oct2;
		stringstream ss1(ip1), ss2(ip2);
		string octets;
		while (getline(ss1, octets, '.'))
			oct1.push_back(stoi(octets));
		while (getline(ss2, octets, '.'))
			oct2.push_back(stoi(octets));
		for (size_t i = 0; i < 4; i++) {
			if (oct1[i] != oct2[i])
				return oct1[i] < oct2[i];
		}
		return false;
	}
public:
	clientList():first(nullptr){}
	~clientList() {
		clientNode* cur = first;
		while (cur) {
			clientNode* nt = cur->getNext();
			delete cur;
			cur = nt;
		}
	}

	void additem(string IP, string rgn, size_t mnst);
	void sortIP();
	void sortMoney();
	void sortRegion();
	void print();
	void readFile(const string& filename);
};

void clientList::additem(string IP, string rgn, size_t mnst) {
	clientNode* newclientNode = new clientNode(IP, rgn, mnst);
	newclientNode->setNext(first);
	first = newclientNode;
	updataRGC(rgn);
}

void clientList::sortIP() {
	auto point = getNodePointers();
	merge_sort(point, [this](clientNode* a, clientNode* b) {
		return compareIP(a->getIP(), b->getIP());
		});
	modiList(point);
}

void clientList::sortMoney() {
	auto point = getNodePointers();
	merge_sort(point, [](clientNode* a, clientNode* b) {
		return a->getMNST() > b->getMNST();
		});
	modiList(point);
}

void clientList::sortRegion() {
	auto point = getNodePointers();
	merge_sort(point, [this](clientNode* a, clientNode* b) {
		return regionCount[a->getRGN()] > regionCount[b -> getRGN()];
		});
	modiList(point);
}

void clientList::print() {
	clientNode* cur = first;
	while (cur) {
		cout << cur->getIP() << ", " << cur->getRGN()
			 << ", " << cur->getMNST() << '\n';
		cur = cur->getNext();
	}
}

void clientList::readFile(const string& filename) {
	ifstream f(filename);
	if (!f.is_open())
		throw runtime_error("Couldn't open the file " + filename);
	string line;
	while (getline(f, line)) {
		stringstream ss(line);
		string ip, region;
		size_t money;
		if (ss >> ip >> region >> money)
			additem(ip, region, money);
		else
			cerr << "Format error " << line << '\n';
	}
	f.close();
}




int main() {
	setlocale(LC_ALL, "rus");
	clientList a;
	a.readFile("ClientData.txt");

	a.sortIP();
	cout << "The data is sorted by IP address in ascending order\n" << "-------------\n";
	a.print();

	a.sortMoney();
	cout << "\nThe data is sorted by the amount of money spent in descending order\n" << "-------------\n";
	a.print();

	a.sortRegion();
	cout << "\nThe data is sorted by region popularity in descending order\n" << "-------------\n";
	a.print();
}