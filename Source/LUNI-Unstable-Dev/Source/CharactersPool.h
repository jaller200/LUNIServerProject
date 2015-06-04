#pragma once

#include "Common.h"
#include "Character.h"

#include "RakNet\RakNetTypes.h"

#include <unordered_map>

class CharactersPool {
private:
	std::unordered_map< ulong, Ref< Character > > _pool;
public:
	void Insert(const Ref< Character > character, const SystemAddress& sysaddr) {
		_pool.insert(std::make_pair(sysaddr.binaryAddress, character));
	}

	int Count() { return _pool.size(); }

	Ref< Character > Find(const SystemAddress& sysaddr) {
		auto fnd = _pool.find(sysaddr.binaryAddress);
		if (fnd == _pool.end()) return NULL;

		return fnd->second;
	}

	bool Remove(const SystemAddress& sysaddr) {
		using namespace std;
		auto character = Find(sysaddr);
		if (character == NULL) return false;
		_pool.erase(sysaddr.binaryAddress);
		return true;
	}
};