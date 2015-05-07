#pragma once

#include "Common.h"
#include "User.h"
#include "RakNet\RakNetTypes.h"
#include <unordered_map>

//use that for managing online users
class UsersPool {
	private:
		std::unordered_map< ulong, Ref< User > > _pool;
	public:
		void Insert(const Ref< User >& usr, const SystemAddress& sysaddr) {
			_pool.insert(std::make_pair(sysaddr.binaryAddress, usr));
		}

		int Count() { return _pool.size(); }

		//returns null if user is not found
		Ref< User > Find(const SystemAddress& sysaddr) {
			auto fnd = _pool.find(sysaddr.binaryAddress);
			if (fnd == _pool.end()) return NULL; //not found
			return fnd->second;
		}

		//returns true if user is removed
		bool Remove(const SystemAddress& sysaddr) {
			using namespace std;
			auto usr = Find(sysaddr);
			if (usr == NULL || (--usr->numredir) > 0) return false;
			_pool.erase(sysaddr.binaryAddress);
			return true;
		}
};