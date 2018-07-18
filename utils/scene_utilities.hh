#pragma once
#include "config/universe.hh"
#include "light_control/scheduling.hh"

namespace utils
{
///
/// Set the universe to black full brightness
///
light_control::schedule_entry clear_universe(const config::universe& universe);

}
