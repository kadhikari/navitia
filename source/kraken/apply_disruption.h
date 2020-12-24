/* Copyright © 2001-2014, Canal TP and/or its affiliates. All rights reserved.

This file is part of Navitia,
    the software to build cool stuff with public transport.

Hope you'll enjoy and contribute to this project,
    powered by Canal TP (www.canaltp.fr).
Help us simplify mobility and open public transport:
    a non ending quest to the responsive locomotion way of traveling!

LICENCE: This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

Stay tuned using
twitter @navitia
IRC #navitia on freenode
https://groups.google.com/d/forum/navitia
www.navitia.io
*/

#pragma once

#include "type/pt_data.h"
#include "type/message.h"
#include "type/chaos.pb.h"
#include "type/gtfs-realtime.pb.h"
#include "type/meta_data.h"
#include <memory>

namespace navitia {

boost::posix_time::time_period execution_period(const boost::gregorian::date& date, const nt::VehicleJourney& vj);

/**
 * WARNING: this method can add, remove or transform PT-Ref objects in PT_Data
 * After using it, make sure to rebuild:
 * - RAPTOR (probably through Data.build_raptor)
 * - AUTOCOMPLETE on PT-Ref (probably through PT_Data.build_autocomplete)
 */
void apply_disruption(const type::disruption::Disruption&, navitia::type::PT_Data&, const navitia::type::MetaData&);

void delete_disruption(const std::string& disruption_id, nt::PT_Data& pt_data, const nt::MetaData& meta);
}  // namespace navitia
