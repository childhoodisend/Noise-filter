#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <deque>
using namespace std;

static const size_t SIGNAL_SIZE = 496151;
static const size_t SAMPLE_RATE = 44100;
static const double TIME = (double) 1 / SAMPLE_RATE;
static const unsigned QUANTITY = 500;
static const int16_t THRESH_HOLD = 500;

int16_t max_of_deque(const deque<int16_t> &d){
    int32_t maximum = INT16_MIN;
    for(auto i: d){
        maximum = max(maximum, abs(i));
    }
    return maximum;
}

vector<int16_t> apply_max_filter(const vector<int16_t> &source_signal,const int16_t win_len){
    vector<int16_t> _p;
    deque<int16_t > current_win_len(win_len);
    int32_t maximum = INT32_MIN;


    for(size_t i = 0; i < win_len; ++i) {
        current_win_len.push_back(source_signal[i]);
        maximum = max(abs(source_signal[i]), maximum);
    }
    _p.push_back(maximum);

    for(size_t i = win_len; i < source_signal.size(); ++i) {
        current_win_len.pop_front();
        current_win_len.push_back(source_signal[i]);
        maximum = max_of_deque(current_win_len);
        _p.push_back(maximum);
    }

    return _p;
}
vector<int16_t> apply_sma_filter(const vector<int16_t> &source_signal,const int16_t win_len){
    vector<int16_t> _p;
    int16_t sum = 0;
    for (size_t i = 0; i < win_len; ++i) {
        sum += abs(source_signal[i]);
    }
    _p.push_back(sum / win_len);
    for (size_t i = win_len; i < SIGNAL_SIZE; ++i) {
        sum -= abs(source_signal[i - win_len]);
        sum += abs(source_signal[i]);
        _p.push_back(sum / win_len);
    }
    return _p;
}


vector<int16_t> init() {
    ifstream input;
    input.open("1-10.bin", ios_base::binary);
    vector<int16_t> source_signal(SIGNAL_SIZE);
    if (input.is_open()) {
        int counter = 0;
        while (!input.eof()) {
            int16_t num;
            input.read(reinterpret_cast<char *>(&num), sizeof(int16_t));
            source_signal[counter] = num;
            counter++;
        }
        input.close();
    } else
        throw invalid_argument("Can't open file");

    return source_signal;
}

void save_to_file(const vector<int16_t> &sliced_signal, const string& filename) {
    ofstream output;
    output.open(filename + ".bin", ios_base::binary | ios_base::out);
    for (auto sig: sliced_signal) {
        output.write(reinterpret_cast<char *>(&sig), sizeof(int16_t));
    }
}
template<typename T>
void print(const vector<T> &vec) {
    for (auto i: vec)
        cout << i << endl;
}

vector<int16_t> apply_noise_filter(const vector<int16_t> &source_signal, int16_t win_len, int16_t thresh_hold) {
    if (thresh_hold == 0) throw invalid_argument("thresh hold == 0");

    vector<int16_t> sliced_signal;
    vector<int16_t> _p = apply_max_filter(source_signal, win_len);
    cout << "_p     size -> " << _p.size() << endl;

    auto begin = source_signal.begin();
    auto end = begin;
    advance(end, win_len);
    for(auto i :_p){
        if(abs(i) > thresh_hold) {
            while (begin != end){
                sliced_signal.push_back(*begin);
                begin++;
            }
            if(end != source_signal.end())
                end++;
            else
                break;
        }
        else {
            begin++;
            end++;
        }
    }


    return sliced_signal;
}

vector<int16_t> apply_noise_filter_naive(vector<int16_t> &source_signal, int16_t win_len, int16_t thresh_hold) {
    if (thresh_hold == 0) throw invalid_argument("thresh hold == 0");

    vector<int16_t> sliced_signal_naive;

    for(auto i : source_signal){
        if(abs(i) > thresh_hold)
            sliced_signal_naive.push_back(i);
    }

    return sliced_signal_naive;
}


int main() {
    try {
        vector<int16_t> source_signal = init();
        cout << "source size       -> "<< source_signal.size() << endl;

        auto sliced_signal = apply_noise_filter(source_signal, QUANTITY,THRESH_HOLD);
        cout << "sliced size       -> "<< sliced_signal.size() << endl;

        auto sliced_signal_naive = apply_noise_filter_naive(sliced_signal,QUANTITY,THRESH_HOLD);
        cout << "sliced_naive size -> "<< sliced_signal.size() << endl;

        save_to_file(sliced_signal, "sliced_signal");
        save_to_file(sliced_signal_naive, "sliced_signal_naive");
    }
    catch (invalid_argument &e) {
        cerr << e.what() << endl;
    }

    return EXIT_SUCCESS;
}