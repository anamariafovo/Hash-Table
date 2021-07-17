#include<iostream>

#include "ADS_set.h"

int main(){


	ADS_set<int> a{14,1,23,64,8,10};
	ADS_set<int> b{4,7,11,3,37};
	ADS_set<int> c{15,2,53,4};
	ADS_set<int> d{7,9,12};
	ADS_set<int> e{7};
	ADS_set<int> f{4,7,1,5,3,6,0,8,10,2,9};
	ADS_set<int> g{6,12,5};


//test dump + iterator

	a.dump();
	for(ADS_set<int>::const_iterator it = a.begin(); it != a.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;
	std::cout << '\n';
	
//test operator=

	a = b;	
	a.dump();
	std::cout << '\n';

//test empty

	std::cout << a.empty() << '\n';
	std::cout << '\n';

//test erase

	a.erase(7);
	a.dump();
	std::cout << '\n';
	
//test more here...

	return 0;
}