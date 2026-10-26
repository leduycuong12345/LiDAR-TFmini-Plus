#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    vector<string> names = {"C++", "VS Code"};

    for (const string& name : names) {
        cout << "Hello " << name << endl;
    }

    return 0;
}