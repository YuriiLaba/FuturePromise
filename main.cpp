#include<iostream>
#include <fstream>
#include <map>
#include <thread>
#include <vector>
#include <set>
#include <future>

using namespace std;


map<string, int> calcInterval(const vector<string>& myVector, int start, int end)
{
    map<string, int> localMp;

    for (int i = start; i < end; i++) {
        {
            ++localMp[myVector[i]];
        }
    }
    return localMp;
}
void reduce(const map<string, int>& m, map<string, int> &master){

    for(auto w: m){
        master[w.first]+=w.second;
    }

}

void wrapCalcInterval(const vector<string>& myVector,int start, int end,  promise<map<string,int>> result){
    auto r = calcInterval(myVector, start, end);
    result.set_value(r);

}

int main() {

    vector<string> myVector;
    vector<pair<string, int>> VectorOfPair;
    map<string, int> m;

    ifstream myReadFile;
    string data;
    myReadFile.open("data.txt");
    string output;
    if (!myReadFile.is_open()) {
        cerr << "Error opening file ";
        return -1; // exit(-1);
    }
    while (myReadFile >> output) {//check state
        for (size_t i = 0, len = output.size(); i < len; i++) {
            if (ispunct(output[i])) {
                output.erase(i--, 1);
                len = output.size();
            }
        }
        myVector.push_back(output);
    }
    myReadFile.close();

    promise pr1, pr2;
    future f1, f2;
    f1 = pr1.get_future();
    f2 = pr2.get_future();
    thread(wrapCalcInterval, myVector, 0, 10, pr1);
    thread(wrapCalcInterval, myVector, 10, 20, pr2);
    reduce(m, );
}