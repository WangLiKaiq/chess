#include "test.h"
#include <iostream>
#include <algorithm>
#include <vector>
using std::vector;
using std::pair;
using std::make_pair;
using std::for_each;
using std::cout;
using std::endl;
void testOrder() {
	vector<pair<pair<int, int>, int>> order;
	pair<pair<int, int>, int> data1(make_pair(make_pair(1, 1), 2));
	order.push_back(data1);
	pair<pair<int, int>, int> data2(make_pair(make_pair(1, 1), 3));
	pair<pair<int, int>, int> data3(make_pair(make_pair(1, 1), 4));
	pair<pair<int, int>, int> data4(make_pair(make_pair(1, 1), 5));
	pair<pair<int, int>, int> data5(make_pair(make_pair(1, 1), 6));
	pair<pair<int, int>, int> data6(make_pair(make_pair(1, 1), 7));
	order.push_back(data2);
	order.push_back(data3);
	order.push_back(data4);
	order.push_back(data5);
	order.push_back(data6);
	sort(order.begin(), order.end(), [](pair<pair<int, int>, int>& order1, pair<pair<int, int>, int>& order2) {return order1.second > order2.second; });
	for_each(order.begin(), order.end(), [](pair<pair<int, int>, int> temp) {cout << temp.first.first << "\t" << temp.first.second << "\t" << temp.second << endl; });
}
