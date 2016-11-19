#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

using namespace std;

class order {
	int m_price;
	int m_quantity;
	string m_order_id;
public:
	order(int p, int q, string o_id) :m_price(p), m_quantity(q), m_order_id(o_id) {}
	int getPrice() { return m_price; }
	int getQuantity() { return m_quantity; }
	void setQuantity(int qty) { m_quantity = qty; }
	string getOrderId() { return m_order_id; }
};

void trade_sell(const string order_id, const int &price, int &quantity, list<order> &buy_list) {

	for (list<order>::iterator it = buy_list.begin(); it != buy_list.end() && quantity != 0; ) {
		if (it->getPrice() >= price)
		{
			int quan_diff = it->getQuantity() - quantity;

			cout << "TRADE " << it->getOrderId() << ' ' << it->getPrice() << ' ';
			if (quan_diff == 0)
			{
				cout << it->getQuantity() << " " << order_id << ' ' << price << ' ' << quantity << endl;
				it = buy_list.erase(it);
				quantity = 0;
			}
			else if (quan_diff > 0)
			{
				//only incoming quantity is traded, the rest saved back to node
				cout << quantity << ' ' << order_id << ' ' << price << ' ' << quantity << endl;
				it->setQuantity(quan_diff);
				quantity = 0;
			}
			else
			{
				//only front quantity is traded, the rest saved back to the variable quantity
				cout << it->getQuantity() << ' ' << order_id << ' ' << price << ' ' << it->getQuantity() << endl;
				quantity = quantity - it->getQuantity();
				it = buy_list.erase(it);
			}
		}
		else ++it;
	}

}

void trade_buy(const string order_id, const int &price, int &quantity, list<order> &sell_list) {
	
	for (list<order>::iterator it = sell_list.begin(); it != sell_list.end() && quantity != 0; ) {
		if (it->getPrice() <= price) 
		{
			int quan_diff = it->getQuantity() - quantity;

			cout << "TRADE " << it->getOrderId() << ' ' << it->getPrice() << ' ';
			if (quan_diff == 0)
			{
				cout << it->getQuantity() << ' ' << order_id << ' ' << price << ' ' << quantity << endl;
				it = sell_list.erase(it);
				quantity = 0;
			}
			else if (quan_diff > 0)
			{
				//only incoming quantity is traded, the rest saved back to node
				cout << quantity << ' ' << order_id << ' ' << price << ' ' << quantity << endl;
				it->setQuantity(quan_diff);
				quantity = 0;
			}
			else
			{
				//only front quantity is traded, the rest saved back to the variable quantity
				cout << it->getQuantity() << ' ' << order_id << ' ' << price << ' ' << it->getQuantity() << endl;
				quantity = quantity - it->getQuantity();
				it = sell_list.erase(it);
			}
		}
		else ++it;
	}

}

bool CancelOrder(list<order> &buy_list, list<order> &sell_list, const string &order_id) {
	bool cancelled = false;
	for (list<order>::iterator it = buy_list.begin(); it != buy_list.end(); ++it) {
		if (it->getOrderId() == order_id) {
			buy_list.erase(it);
			cancelled = true;
			break;//order_id is unique so return
		}
	}
	
	for (list<order>::iterator it = sell_list.begin(); it != sell_list.end(); ++it) {
		if (it->getOrderId() == order_id) {
			sell_list.erase(it);
			cancelled = true;
			break;
		}
	}
	return cancelled;
}

int main() {
	list<order> buy_list;
	list<order> sell_list;

	string in_str;
	while (getline(cin, in_str))
	{
		stringstream ss(in_str);
		string command;
		ss >> command;

		if (command == "BUY" || command == "SELL" || command == "MODIFY") {
			int price = 0;
			int quantity = 0;
			string order_id;
			string type;
			
			if (command != "MODIFY")
			{
				ss >> type;
				ss >> price;
				ss >> quantity;
				ss >> order_id;
			}
			else
			{
				ss >> order_id;
				command.clear();
				ss >> command;
				ss >> price;
				ss >> quantity;
				//cancel previous order, modify only if previous order exist
				if (!CancelOrder(buy_list, sell_list, order_id)) command.clear();
				type = "GFD";

			}
						
			if (command == "BUY")
			{
				// trade the buy operation with already present sell operations, if not add into buy list 
				trade_buy(order_id, price, quantity, sell_list);
				if (quantity != 0 && type != "IOC")  buy_list.emplace_back(price, quantity, order_id);
			}
			else if (command == "SELL")
			{
				// trade the sell operation with already present buy operations, if not add into sell list
				trade_sell(order_id, price, quantity, buy_list);
				if (quantity != 0 && type != "IOC") sell_list.emplace_back(price, quantity, order_id);
			}
		}
		else if (command == "CANCEL") {
			string order_id;
			ss >> order_id;

			CancelOrder(buy_list, sell_list, order_id);
			
		}else if (command == "PRINT") {
			map<int, int> buy_map;
			map<int, int> sell_map;
			
			//Collect buy elements
			for (list<order>::iterator it = buy_list.begin(); it != buy_list.end(); ++it) {
				buy_map[it->getPrice()] += it->getQuantity();
			}
			
			//Collect sell elements
			for (list<order>::iterator it = sell_list.begin(); it != sell_list.end(); ++it) {
				sell_map[it->getPrice()] += it->getQuantity();
			}

			//Map already sorted, so do reverse iterator to print in decreasing order
			cout << "SELL:" << endl;
			for (map<int, int>::reverse_iterator it = sell_map.rbegin(); it != sell_map.rend(); ++it)
				std::cout << it->first << " " << it->second << endl;

			cout << "BUY:" << endl;
			for (map<int, int>::reverse_iterator it = buy_map.rbegin(); it != buy_map.rend(); ++it)
				std::cout << it->first << " " << it->second << endl;
		}
	}

	return 0;
}