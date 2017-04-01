#include <iostream>
#include <fstream>
#include <map>
#include <thread>
#include <vector>
#include <algorithm>
#include <set>
#include <mutex>
#include <chrono>
#include <atomic>
#include <future>

using namespace std;
mutex mtx;


//for(x: lm)
//gm[x.first] += x.sp
//

int finder(const vector<string>& myVector, future<int> &f,  future<int> &f1, std::map<string, int>& m)
{
    map<string, int> localMp;
    int start = f.get();
    int end = f1.get();
    for (int i = start; i < end; i++) {
        {
            lock_guard<mutex> lg(mtx);
            ++localMp[myVector[i]];
        }
    }
    lock_guard<mutex> lg(mtx);

    for(map<string, int> :: iterator i = localMp.begin(); i != localMp.end(); i++){
        m[i->first] += i-> second;

    }

    return end;

}

int main() {

    int x;
    int y;
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


    promise <int> p;
    promise <int> p1;
    future <int> f = p.get_future();
    future <int> f1 = p1.get_future();


    future<int> fu = async(launch::async, finder, cref(myVector),ref(f), ref(f1), ref(m));//child thread to parent thread
    //future<int> fu1 = async(launch::async, finder, cref(myVector),3, 5, ref(m));
    p.set_value(0);
    p1.set_value(5);

    x = fu.get();//wait until child process finish and return value from it
    //y = fu1.get();


    cout<<x<<endl;
    cout<<m.size()<<endl;
    for(map<string, int> :: iterator i = m.begin(); i != m.end(); i++){
        cout <<    i -> first << ": " << i-> second << endl;

    }


}