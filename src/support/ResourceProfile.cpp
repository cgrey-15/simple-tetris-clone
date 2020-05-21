#include "ResourceProfile.h"
#include <algorithm>
#include <limits>

resource::ResourceMgr::ResourceMgr() : ids_{}, nameStr_{}, aliasStr_{}{}

namespace {
	auto binsearch = [](const resource::resource_profile a, const resource::resource_profile b) {return a.id < b.id; };
}

bool resource::ResourceMgr::addResource(RsrcId id, const char* name, const char* alias, const char* packgroup) {
	if (std::binary_search(std::begin(ids_), std::end(ids_), resource_profile{ id }, ::binsearch) ) {
		return false;
	}
	nameStr_.emplace_back(name);
	aliasStr_.emplace_back(alias);
	packgroupStr_.emplace_back(packgroup);

	auto a1 = static_cast<uint16_t>((nameStr_.size() - 1));
	auto a2 = static_cast<uint16_t>((aliasStr_.size() - 1));
	auto a3 = static_cast<uint16_t>((packgroupStr_.size() - 1));
	ids_.push_back({ id, a1, a2, a3 });
	if (!(id > ids_.back().id)) {
		std::sort(std::begin(ids_), std::end(ids_), [](resource_profile a, resource_profile b) {return a.id < b.id; });
	}
	return true;
}

const char* resource::ResourceMgr::getName(RsrcId id) const {
	auto iter = (std::lower_bound(std::begin(ids_), std::end(ids_), resource_profile{ id }, ::binsearch));
	if (iter != std::end(ids_) && iter->id == id) {
		return nameStr_[iter->nameStrPtr].c_str();
	}
	else {
		return nullptr;
	}
}

const char* resource::ResourceMgr::getAlias(RsrcId id) const {
	auto iter = (std::lower_bound(std::begin(ids_), std::end(ids_), resource_profile{ id }, ::binsearch));
	if (iter != std::end(ids_) && iter->id == id) {
		return aliasStr_[iter->aliasStrPtr].c_str();
	}
	else {
		return nullptr;
	}
}

resource::resource_profile resource::ResourceMgr::operator[](RsrcId id) const {
	auto iter = (std::lower_bound(std::begin(ids_), std::end(ids_), resource_profile{ id }, ::binsearch));
	constexpr auto nulV = std::numeric_limits<RsrcId>::max();
	if (iter != std::end(ids_) && iter->id == id) {
		return *iter;
	}
	else {
		return { nulV, nulV, nulV };
	}
}

const char* resource::ResourceMgr::getNameWithProfile(resource_profile p) const {
	return nameStr_[p.nameStrPtr].c_str();
}

const char* resource::ResourceMgr::getAliasWithProfile(resource_profile p) const {
	return aliasStr_[p.aliasStrPtr].c_str();
}
const char* resource::ResourceMgr::getPackgroupWithProfile(resource_profile p) const {
	return packgroupStr_[p.packgroupPtr].c_str();
}