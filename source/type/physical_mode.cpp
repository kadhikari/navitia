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

#include "type/physical_mode.h"
#include "type/indexes.h"
#include "type/pt_data.h"
#include "type/serialization.h"

namespace navitia {
namespace type {

template <class Archive>
void PhysicalMode::serialize(Archive& ar, const unsigned int) {
    ar& idx& name& uri& co2_emission& vehicle_journey_list;
}
SERIALIZABLE(PhysicalMode)

Indexes PhysicalMode::get(Type_e type, const PT_Data& data) const {
    Indexes result;
    switch (type) {
        case Type_e::VehicleJourney:
            for (const auto* vj : data.vehicle_journeys) {
                if (vj->physical_mode != this) {
                    continue;
                }
                result.insert(vj->idx);
            }
            break;
        default:
            break;
    }
    return result;
}

}  // namespace type
}  // namespace navitia
