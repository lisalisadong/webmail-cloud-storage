#include <string>
#include <iostream>
#include <sstream>
#include <limits.h>
#include <vector>
#include <map>
#include <unordered_set>
#include "Hasher.h"

#define V_NUM 3

class VNode{
public:
	std::string vId;
	std::string id;
	long hashVal;

public:
	VNode(std::string vId, std::string id, int hashVal) {
		this->vId = vId;
		this->id = id;
		this->hashVal = hashVal;
	}

	VNode() {
		VNode("", "", 0);
	}
};

class ConHash {
public:

	ConHash();

	std::vector<std::pair<std::string, std::string> > addNode(std::string addr);

	bool deleteNode(std::string addr);

	std::string getNode(std::string key);

private:

	std::map<long, VNode> map;

	Hasher hasher;

	int v_num;


private:

	VNode getVirtual(std::string, int i);

};


ConHash::ConHash() {
	v_num = V_NUM;
}

VNode ConHash::getVirtual(std::string addr, int i) {
	std::stringstream ss;
	ss<<i;

	VNode res(addr + "_" + ss.str(), addr, i);

	return res;
}


std::vector<std::pair<std::string, std::string> > ConHash::addNode(std::string addr) {
	std::vector<std::pair<std::string, std::string> > res;

	std::unordered_set<std::string> set;

	/* gnerate virtual nodes */
	for(int i = 0; i < v_num; i++) {
		VNode v = getVirtual(addr, i);

		long hashVal = hasher.getHashVal(v.vId);

		std::pair<std::map<long, VNode>::iterator, bool> p = map.insert(std::pair<long, VNode>(hashVal, v));

		 /* same hash val exists */
		if(!p.second) {
			throw std::exception();
		}

		auto itr = p.first;
		itr++;
		VNode next;
		if(itr == map.end()) {
			next = (++map.begin())->second;
		} else {
			next = itr->second;
		}

		// if next node is the node itself
		if(next.id == v.id) continue;

		// if the physical node of next node has been there
		if(set.find(next.id) != set.end()) continue;


		std::pair<std::string, std::string> tmp(next.vId, v.vId);
		res.push_back(tmp);

		set.insert(next.id);
	}

	return res;
}





























