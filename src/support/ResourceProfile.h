#ifndef GAME_RESOURCE_PROFILE_H
#define GAME_RESOURCE_PROFILE_H
#include <cstdint>
#include <string>
#include <vector>
#include <Corrade/Utility/Resource.h>
#include "Common.h"

namespace resource {
	struct resource_profile {
		resource::RsrcId id;
		uint16_t nameStrPtr;
		uint16_t aliasStrPtr;
		uint16_t packgroupPtr;
	};
#if 1
	constexpr size_t what = sizeof(resource_profile);
	constexpr size_t field0Ptr = offsetof(resource_profile, id);
#endif

	class ResourceMgr {
	public:
		ResourceMgr();
		bool addResource(RsrcId id, const char* name, const char* alias, const char* packgroup);
		const char* getName(RsrcId id) const;
		const char* getAlias(RsrcId id)const;
		resource_profile operator[](RsrcId id) const;
		const char* getNameWithProfile(resource_profile p) const;
		const char* getAliasWithProfile(resource_profile p) const;
		const char* getPackgroupWithProfile(resource_profile p) const;
	private:
		std::vector<resource_profile> ids_;
		std::vector<std::string> nameStr_;
		std::vector<std::string> aliasStr_;
		std::vector<std::string> packgroupStr_;
	};
}
#endif // GAME_RESOURCE_PROFILE_H
