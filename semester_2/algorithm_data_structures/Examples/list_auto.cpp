#include <string>
#include <list>
#include <iostream>

using namespace std;

int main() {
	list<string> a;
	string x;
	while (true) {
		cin >> x;
		if (x.compare("ZZ") == 0) break;
		a.push_back(x);
	}

	for (auto iter = a.begin(); iter != a.end(); iter++) {
		cout << *iter  << endl;
	}
}

