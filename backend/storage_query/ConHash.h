#include <string>
#include <iostream>
#include <sstream>
#include <limits.h>
#include <vector>
#include <map>
#include <unordered_set>
#include "Hasher.h"

#define V_NUM 1

class VNode{
public:
	std::string vId;
	std::string id;
	long hashVal;

public:
	VNode(std::string vId, std::string id, long hashVal) {
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

	std::vector<std::string> getReplicaNode(std::string addr);

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

	std::unordered_set<std::string> ids;

	/* gnerate virtual nodes */
	for(int i = 0; i < v_num; i++) {
		VNode v = getVirtual(addr, i);

		long hashVal = hasher.getHashVal(v.vId);

		std::cout << "hash value of" << v.vId << " is: " << hashVal << std::endl;

		/* same hash val exists */
		if(map.find(hashVal) != map.end()) {
			throw std::exception();
		}

		map.insert(std::pair<long, VNode>(hashVal, v));
		std::map<long, VNode>::iterator itr = map.upper_bound(hashVal);

		if(itr == map.end()) {
			itr = map.begin();
		}

		VNode next = itr->second;

		std::cout << "next node is: " << next.id << std::endl;

		// if next node is the node itself
		if(next.id == v.id) continue;

		// if the physical node of next node has been there
		if(ids.find(next.id) != ids.end()) continue;


		std::pair<std::string, std::string> tmp(next.vId, v.vId);
		res.push_back(tmp);

		ids.insert(next.id);
	}

	return res;
}

std::string ConHash::getNode(std::string key) {
	/* No node yet */
	if(map.size() == 0) {
		throw std::exception();
	}

	long hashVal = hasher.getHashVal(key);

	std::cout << "Hash value of " << key << " is: " << hashVal << std::endl;

	std::map<long, VNode>::iterator itr = map.upper_bound(hashVal);

	if(itr == map.end()) {
		itr = map.begin();
	}

	std::cout << key << " is in node: " << itr->second.vId << std::endl;

	return itr->second.id;
}

std::vector<std::string> ConHash::getReplicaNode(std::string addr) {
	std::vector<std::string> res;

	if(map.size() == 0) return res;

	long hashVal = hasher.getHashVal(addr);
	std::map<long, VNode>::iterator itr = map.upper_bound(hashVal);

	if(itr == map.end()) {
		itr = map.begin();
	}

	if(itr->second.id != addr) {
		std::cout << "Replica node for " << addr << ": " << itr->second.id << std::endl;
		res.push_back(itr->second.id);
	}

	itr++;
	if(itr == map.end()) {
		itr = map.begin();
	}

	if(itr->second.id != addr && (res.size() == 0 || res[0] != itr->second.id)) {
		std::cout << "Replica node for " << addr << ": " << itr->second.id << std::endl;
		res.push_back(itr->second.id);
	}

	return res;
}

































