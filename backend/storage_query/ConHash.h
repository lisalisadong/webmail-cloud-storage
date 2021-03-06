#include <string>
#include <iostream>
#include <sstream>
#include <limits.h>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include "utils.h"
#include "logger.h"

#define V_NUM 1

Logger logger;

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

	std::vector<std::string> getNodes(std::string key);

	std::vector<std::string> getAllNodes();

	std::string getUpServers();

	std::string getDownServers();

	void notifyUp(std::string addr);

	void notifyDown(std::string addr);

	std::string getReplica(std::string addr);

public:

	std::map<long, VNode> map;

	int v_num;

	std::unordered_set<std::string> upServers;

	std::unordered_set<std::string> downServers;

private:

	Logger logger;

	VNode getVirtual(std::string, int i);

};


ConHash::ConHash() {
	logger.log_config("ConHash");
	v_num = V_NUM;
}

VNode ConHash::getVirtual(std::string addr, int i) {
	std::stringstream ss;
	ss<<i;

	VNode res(addr + "_" + ss.str(), addr, i);

	return res;
}


// return all the virtual nodes that's clockwisely next to the node added
std::vector<std::pair<std::string, std::string> > ConHash::addNode(std::string addr) {
	// return the nodes that new nodes should ask for data.
	std::vector<std::pair<std::string, std::string> > res;

	// add to working server set
	upServers.insert(addr);
	downServers.erase(addr);

	std::unordered_set<std::string> ids;

	/* gnerate virtual nodes */
	for(int i = 0; i < v_num; i++) {
		VNode v = getVirtual(addr, i);

		long hashVal = get_hash_val(v.vId);

		// logger.log_trace("hash value of" + v.vId + " is: " + hashVal);

		/* same hash val exists */
		if(map.find(hashVal) == map.end()) {
			// throw std::exception();
			map.insert(std::pair<long, VNode>(hashVal, v));
		}

		std::map<long, VNode>::iterator itr = map.upper_bound(hashVal);

		if(itr == map.end()) {
			itr = map.begin();
		}

		VNode next = itr->second;

		logger.log_trace("next node is: " + next.id);

		// if next node is the node itself
		if(next.id == v.id) continue;

		// if the physical node of next node has been there
		if(ids.find(next.id) != ids.end()) continue;


		std::pair<std::string, std::string> tmp(next.vId, v.vId);
		res.push_back(tmp);

		ids.insert(next.id);
	}
	std::cout << "===========================Node " << addr << " added.============================" << std::endl;

	return res;
}

std::vector<std::string> ConHash::getNodes(std::string key) {
	/* No node yet */
	if(map.size() == 0) {
		throw std::exception();
	}

	std::vector<std::string> res;
	//============first hash====================
	long hashVal = get_hash_val(key);

	// logger.log_trace("Hash value of " + key + " is: " + hashVal);

	std::map<long, VNode>::iterator itr = map.upper_bound(hashVal);

	if(itr == map.end()) {
		itr = map.begin();
	}

	std::string primary = itr->second.id;

	std::cout << "The primary is: " << primary << std::endl;

	if(downServers.find(itr->second.id) == downServers.end()) {
		std::cout << "primary added" << std::endl;
		res.push_back(primary);
	}

	//=============get replica==================
	std::string replica = getReplica(key);

	std::cout << "The replica is: " << replica << std::endl;

	if(primary != replica && downServers.find(replica) == downServers.end()) {
		std::cout << "replcia added" << std::endl;
		res.push_back(replica);
	}

	return res;
}

// returns all the nodes.
std::vector<std::string> ConHash::getAllNodes() {
	std::vector<std::string> res;
	for(std::string server: upServers) {
		res.push_back(server);
	}

	for(std::string server: downServers) {
		res.push_back(server);
	}
	return res;
}

void ConHash::notifyUp(std::string addr) {
	upServers.insert(addr);

	downServers.erase(addr);
}

void ConHash::notifyDown(std::string addr) {
	upServers.erase(addr);

	downServers.insert(addr);

	// std::cout << "Down servers: " << std::endl;
	// for(std::string d: downServers) {
	// 	std::cout << d << std::endl;
	// }
}

std::string ConHash::getReplica(std::string key) {
	if(map.size() == 0) return "";
	long hashVal = get_hash_val(key);
	
	std::map<long, VNode>::iterator itr = map.upper_bound(hashVal);

	if(itr == map.end()) {
		itr = map.begin();
	}
	itr++;
	if(itr == map.end()) {
		itr = map.begin();
	}

	return itr->second.id;

}

std::string ConHash::getUpServers() {
	std::string ret;
	for (auto s : upServers) {
		ret += serialize(s);
	}
	return ret;
}

std::string ConHash::getDownServers() {
	std::string ret;

	
	for (auto s : downServers) {
		ret += serialize(s);
	}
	return ret;
}





























