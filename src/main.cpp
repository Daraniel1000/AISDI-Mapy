#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <iostream>
#include "TreeMap.h"
#include "HashMap.h"

using namespace std;
using namespace chrono;

const int MAX_VALUE = 100000;
const int NEL = 100000;
const int nrtests = 3;

void perfomTest()
{
  vector <pair<int, int>> tab;
  aisdi::HashMap<int, int> hMap;
  aisdi::TreeMap<int, int> tMap;

  duration<double> dl;
  time_point<system_clock> start, stop;
  // Wektor wypełniony losowymi parami, żeby były takie same dane dla obu typów
  for(int i = 0; i < NEL; i++)
  {
    tab.push_back(make_pair((rand() % MAX_VALUE), (rand() % MAX_VALUE)));
  }

  // ADDING ELEMENTS TO HASHMAP
  start = system_clock::now();
  for(auto i: tab)
    hMap[i.first] = i.second;
  stop = system_clock::now();
  dl = stop - start;
  cout<<"Adding "<<NEL<<" elements to map:\nHashMap time: "<<dl.count()<<" s";

  //ADDING ELEMENTS TO TREEMAP
  start = system_clock::now();
  for(auto i: tab)
    tMap[i.first] = i.second;
  stop = system_clock::now();
  dl = stop - start;
  cout<<"\nTreeMap time: "<<dl.count()<<" s\n\n";

  //ITERATING THROUGH HASHMAP
  start = system_clock::now();
  for(auto i: hMap)
    (void)i;
  stop = system_clock::now();
  dl = stop - start;
  cout<<"Iterating through "<<NEL<<" elements to map:\nHashMap time: "<<dl.count()<<" s";

  //ITERATING THROUGH TREEMAP
  start = system_clock::now();
  for(auto i: hMap)
    (void)i;
  stop = system_clock::now();
  dl = stop - start;
  cout<<"\nTreeMap time: "<<dl.count()<<" s\n\n";

  //ACCESSING RANDOM KEYS IN HASHMAP
  start = system_clock::now();
  for(int i = 0; i < NEL; i++)
    (void)tMap[tab[rand() % NEL].first];
  stop = system_clock::now();
  dl = stop - start;
  cout<<"Accessing "<<NEL<<" random keys:\nHashMap time: "<<dl.count()<<" s";

  //ACCESSING RANDOM KEYS IN TREEMAP
  start = system_clock::now();
  for(int i = 0; i < NEL; i++)
    (void)tMap[tab[rand() % NEL].first];
  stop = system_clock::now();
  dl = stop - start;
  cout<<"\nTreeMap time: "<<dl.count()<<" s\n\n";

  //REMOVING ALL ELEMENTS FROM HASHMAP
  start = system_clock::now();
  for(int i = 0; i < NEL/2; i++)
  {
    try
    {
        hMap.remove(hMap.begin());
    }
    catch(std::out_of_range)
    {}
  }
  stop = system_clock::now();
  dl = stop - start;
  cout<<"Removing "<<NEL/2<<" elements:\nHashMap time: "<<dl.count()<<" s";

  //REMOVING ALL ELEMENTS FROM TREEMAP: ORDER DOESN'T MATTER BECAUSE OF BALANCE
  start = system_clock::now();
  for(int i = 0; i < NEL/2; i++)
  {
    try
    {
        tMap.remove(tMap.begin());
    }
    catch(std::out_of_range)
    {}
  }
  stop = system_clock::now();
  dl = stop - start;
  cout<<"\nTreeMap time: "<<dl.count()<<" s\n\n";

}


int main()
{
  srand(time(NULL));
  for(int i=0; i<nrtests; i++)
  {
    cout<<"#################Test "<<i+1<<"#####################\n\n";
    perfomTest();
  }
  return 0;
}
