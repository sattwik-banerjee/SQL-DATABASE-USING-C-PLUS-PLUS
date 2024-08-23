#include "../table/resultset.h"
#include <vector>
#include <iostream>

using namespace std;
// this is the result set class that stores vectors of record numers
ResultSet::ResultSet() 
{
    result.clear();
}

vector<vector<long>> ResultSet::get_result() const  // returns whatever is in the vector result
{
    return result;
}

void ResultSet::add(const vector<long>& res) // same as push_back into a vector
{
    result.push_back(res);
}

vector<long> ResultSet::pop() // same as popping a vector
{
    vector<long> last = result.back();
    result.pop_back();
    return last;
}

bool ResultSet::empty() // same as vector empty() function
{
    return(result.empty());
}

int ResultSet::size()
{
    return result.size();
}

