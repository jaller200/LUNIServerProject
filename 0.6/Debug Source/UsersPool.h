#pragma once

#include "Common.h"
#include "User.h"

#include "RakNet\RakNetTypes.h"

#include <unordered_map>

// Use that for managing online users
class UsersPool {
	private:
		std::unordered_map< ulong, Ref< User > > _pool; // This is an unordered_map of the online Users
	public:
		// This inserts a new user into the user pool
		void Insert(const Ref< User >& usr, const SystemAddress& sysaddr) {
			_pool.insert(std::make_pair(sysaddr.binaryAddress, usr));
		}

		// This returns the count with the number of users online
		int Count() { return _pool.size(); }

		// Searches the user pool for a specific user (by their SystemAddress)
		// Returns null if user is not found
		Ref< User > Find(const SystemAddress& sysaddr) {
			auto fnd = _pool.find(sysaddr.binaryAddress); // Search for user...
			if (fnd == _pool.end()) return NULL; // User not found
			return fnd->second; // If a user is found, return the user
		}

		// Remove a user from the user pool
		// Returns true if user is removed
		bool Remove(const SystemAddress& sysaddr) {
			using namespace std;
			auto usr = Find(sysaddr);
			if (usr == NULL || (--usr->numredir) > 0) return false;
			_pool.erase(sysaddr.binaryAddress);
			return true;
		}
};