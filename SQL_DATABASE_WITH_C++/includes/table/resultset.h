#ifndef RESULTSET_H
#define RESULTSET_H
#include <vector>
#include <iostream>

using namespace std;

class ResultSet {
public:
    ResultSet();

    vector<vector<long>> get_result() const;

    void add(const vector<long>& res);

    vector<long> pop();

    bool empty();

    int size();

private:
    vector<vector<long>> result;
};

#endif