#include "layout.hpp"

namespace eto {
void Layout::add_member(Member member) { m_members.push_back(std::move(member)); }
} // namespace eto
