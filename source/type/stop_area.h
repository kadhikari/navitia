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
#include <type/type_interfaces.h>
#include <type/fwd_type.h>
#include <type/geographical_coord.h>

namespace navitia {
namespace type {

struct StopArea : public Header, Nameable, hasProperties, HasMessages {
    const static Type_e type = Type_e::StopArea;
    GeographicalCoord coord;
    std::string additional_data;
    std::vector<navitia::georef::Admin*> admin_list;
    bool wheelchair_boarding = false;
    std::string label;
    // name of the time zone of the stop area
    // the name must respect the format of the tz db, for example "Europe/Paris"
    std::string timezone;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int);

    std::vector<StopPoint*> stop_point_list;
    Indexes get(Type_e type, const PT_Data& data) const;
    bool operator<(const StopArea& other) const;
};

}  // namespace type
}  // namespace navitia
