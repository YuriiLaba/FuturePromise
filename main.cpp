
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <sstream>
#include <future>
#include <algorithm>


using namespace std;
//g++ read.cpp -pthread -std=c++11
using words_counter_t = map<string, int>;
words_counter_t m;
mutex myMutex;


void printMap(const words_counter_t &m) {
    for (auto elem : m) {
        cout << elem.first << " : " << elem.second << "\n";
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
        for (size_t i = 0, len = word.size(); i < len; i++)
        {
            auto to = begin(word);
            for (auto from : word)
                if (!ispunct(from))
                    *to++ = from;
            word.resize(distance(begin(word), to));
        }

        transform(word.begin(), word.end(), word.begin(), ::tolower);
        words.push_back(word);
    }
    myfile.close();
    return words;
}

void write_to_file(const words_counter_t &m, string path) {
    ofstream myfile;
    myfile.open(path);
    for (auto elem : m) {
        myfile << elem.first << "    " << elem.second << "\n";
    }
    myfile.close();
}

words_counter_t mapper(int l, int r, const vector<string> &words) {
    words_counter_t mp;
    for (; l <= r; ++l) {
        ++mp[words[l]];
    }
    return mp;

}

void reducer( words_counter_t &master, const words_counter_t& mp){
    for (auto w: mp) {
        master[w.first] += w.second;
    }
}


void worker2(int l, int r, const vector<string> &words,
             promise<words_counter_t> p){
    auto res = mapper(l, r, words);
    p.set_value(res);
}


vector<int> SplitVector(const vector<string>& vec, int n) {

    vector<int> outVec;
    int length = int(vec.size())/ n;
    int count = 0;
    int sum = 0;

    outVec.push_back(0);
    while(count != n - 1){
        sum += length;
        outVec.push_back(sum);
        //cout<<outVec[count]<<endl;
        count++;
    }
    outVec.push_back(int(vec.size()));
    return outVec;
}

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D& d) {
    return std::chrono::duration_cast<chrono::microseconds>(d).count();
}

int main(int argc, char *argv[]) {  // input_file, threads, output_file

    string input_data[4], infile, out_by_a, out_by_n;
    int threads_n;
    ifstream myFile;
    myFile.open("data_input.txt");

    for(int i = 0; i < 4; i++)
        myFile >> input_data[i];
    myFile.close();

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < input_data[i].length(); j++) {
            if (input_data[i][j] == '=')
                input_data[i][j] = ' ';
        }
        stringstream ss(input_data[i]);
        string temp;
        int k = 0;
        while (ss >> temp) {
            if (k != 0) {
                stringstream lineStream(temp);
                string s;
                getline(lineStream,s,',');
                s.erase(remove( s.begin(), s.end(), '\"' ), s.end());
                input_data[i] = s;
            }
            k++;
        }
    }

    infile = input_data[0];
    out_by_a = input_data[1];
    out_by_n = input_data[2];
    threads_n = stoi(input_data[3]);


    vector<string> words;
    vector< future<words_counter_t> > result_futures;
    words = open_read(infile);


    vector<int> list_of_words_amount = SplitVector(words, threads_n);

    vector<thread> rthreads;

    auto start = get_current_time_fenced();

    for (int i = 0; i < list_of_words_amount.size()-1; ++i) {
        promise<words_counter_t> rg;
        result_futures.push_back(rg.get_future());
        rthreads.push_back(thread(worker2, list_of_words_amount[i], list_of_words_amount[i+1], cref(words), move(rg)));
    }

    vector<words_counter_t> results;
    for(size_t i = 0; i<result_futures.size(); ++i)
    {
        reducer(m, result_futures[i].get());
    }

    for (int i = 0; i < threads_n; ++i) {
        rthreads[i].join();
    }
    auto finish = get_current_time_fenced();
    auto total = finish - start;
    cout << "Time: " << to_us(total) << endl;

//    printMap(checkM);
//    cout << "==================================" << endl;

    printMap(m);

    return 0;
}

