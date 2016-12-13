#include <string>
#include <iostream>
#include <sstream>
#include <limits.h>
#include <vector>
#include <map>
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
};

class ConHash {
public:

	ConHash();

	std::vector<std::string> addNode(std::string addr);

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


std::vector<std::string> ConHash::addNode(std::string addr) {
	std::vector<std::string> res;

	/* gnerate virtual nodes */
	for(int i = 0; i < v_num; i++) {
		VNode v = getVirtual(addr, i);

		long hashVal = hasher.getHashVal(v.vId);

		std::pair<std::map<long, VNode>::iterator, bool> p = map.insert(std::pair<long, VNode>(hashVal, v));

		/* same hash val exists */
		if(!p->second) {
			throw std::exception("Failed to add the node.");
		}

		std::map<long, VNode>::iterator itr = p->first;
		itr++;
		Vnode next;
		if(itr == map.end()) {
			next = (++map.begin())->second;
		} else {
			next = itr->second;
		}



	}

	return res;
}





























