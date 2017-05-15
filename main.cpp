
//#include <boost/algorithm/string/replace.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <sstream>
#include <future>


using namespace std;
//g++ read.cpp -pthread -std=c++11
using words_counter_t = map<string, int>;
words_counter_t m;


void printMap(const words_counter_t &m) {
    for (auto elem : m) {
        cout << elem.first << " " << elem.second << "\n";
    }
}


vector<string> open_read(string path) {
    ifstream myfile;
    vector<string> words;
    string word;
    myfile.open(path);
    if (!myfile.is_open()) {
        cerr << "Error" << endl;
        return words;
    }
    string formated_word;
    while (myfile >> word) {
        formated_word = word;  //format_word(word);
        words.push_back(formated_word);
        //        ++checkM[word];       // check map with only main thread
    }
    myfile.close();
    return words;
}

//void write_to_file(const words_counter_t &m, string path) {
//    ofstream myfile;
//    myfile.open(path);
//    for (auto elem : m) {
//        myfile << elem.first << "    " << elem.second << "\n";
//    }
//    myfile.close();
//}

words_counter_t mapper(int start, int end, const vector<string> &words) {
    words_counter_t mp;
    for (int i = start; i < end; i++)
    {
        ++mp[words[i]];
    }
    return mp;

}

void reducer( words_counter_t &master, const words_counter_t& mp){
    for (auto w: mp) {
        master[w.first] += w.second;
    }
}


void worker2(int l, int r, const vector<string> &words, promise<words_counter_t> p){
    auto res = mapper(l, r, words);
    p.set_value(res);
}
bool is_ready(future<words_counter_t> const& f)
{
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

int main(int argc, char *argv[]) {  // input_file, threads, output_file
    vector<string> words;
    vector< future<words_counter_t> > result_futures;
    if (!argv[1]) words = open_read("data.txt");
    else words = open_read(argv[1]);
    vector<thread> rthreads;
    istringstream ss(argv[2]);

    //ss >> x;
    cout << "Spawning " << 2 << " workers" << endl;
    for (int i = 0; i < 5; ++i) {
        size_t a = (words.size()) / 5 * i;
        size_t b = (words.size()) / 5 * (i + 1);
//        cout << (i + 1) << " interval from " << a << " to " << b - 1 << " word" << endl;
        promise<words_counter_t> rg;
        result_futures.push_back(rg.get_future());
        rthreads.push_back(thread(worker2, a, b - 1, cref(words), move(rg)));
    }

    vector<words_counter_t> results;


    //chrono::microseconds span(1000);
    //for(size_t i = 0; i<result_futures.size(); ++i){
        //result_futures[i].wait_for(span);
    //}


    for(size_t i = 0; i<result_futures.size(); ++i)
    {
        if(is_ready(result_futures[i])) {

            reducer(m, result_futures[i].get());
        }
    }

    for (int i = 0; i < 5; ++i) {
        rthreads[i].join();
    }


    printMap(m);
    return 0;
}

