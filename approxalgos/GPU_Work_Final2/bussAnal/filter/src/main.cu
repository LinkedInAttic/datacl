#include <iostream>
#include "test_fns.hpp"

using namespace std;

#define NELEM 1e8
#define NDICT 5000

typedef long long myType;

int main(int argc, char **argv)
{

srand( time(NULL) );

cout << "-----------------------------------------------------" << endl;

testfilter<myType>(NELEM);

cout << endl;
cout << endl;
cout << "-----------------------------------------------------" << endl;

/*
testfilter_PinnedRegMem<myType>(NELEM);


cout << endl;
cout << endl;
cout << "-----------------------------------------------------" << endl;

testfilter_PinnedMem<myType>(NELEM);

cout << endl;
cout << endl;
cout << "-----------------------------------------------------" << endl;

cout << "-----------------------------------------------------" << endl;
cout << endl;
*/
testfilterIn<myType>(NELEM, NDICT);

cout << endl;
cout << "-----------------------------------------------------" << endl;

/*
cout << "-----------------------------------------------------" << endl;
cout << endl;

testfilterIn_conMem<myType>(NELEM, NDICT);

cout << endl;
cout << "-----------------------------------------------------" << endl;
*/
    return 0;
}
