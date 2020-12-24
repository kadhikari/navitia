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

#include "type/pb_converter.h"
#include "routing/routing.h"
#include "routing/get_stop_times.h"
#include "routing/raptor.h"

namespace navitia {
namespace timetables {

typedef std::vector<DateTime> vector_datetime;
typedef std::vector<routing::datetime_stop_time> vector_dt_st;
typedef std::pair<boost::optional<routing::datetime_stop_time>, boost::optional<routing::datetime_stop_time>>
    first_and_last_stop_time;

void departure_board(PbCreator& pb_creator,
                     const std::string& filter,
                     const boost::optional<const std::string> calendar_id,
                     const std::vector<std::string>& forbidden_uris,
                     const pt::ptime datetime,
                     const uint32_t duration,
                     const uint32_t depth,
                     const int count,
                     const int start_page,
                     const type::RTLevel rt_level,
                     const size_t items_per_route_point);

bool between_opening_and_closing(const time_duration& me, const time_duration& opening, const time_duration& closing);

time_duration length_of_time(const time_duration& duration_1, const time_duration& duration_2);

bool line_closed(const time_duration& duration,
                 const time_duration& opening,
                 const time_duration& closing,
                 const pt::ptime& date);

/**
 * @brief Find the new request date time to invoke routing::get_stop_times function,
 * in order to catch the first and last datetime.
 *
 * If the next stop time < opening date time :
 *   - for the first :  new request date time = (opening date time - 1 sec)
 *                      The day before with clockwise mode
 *   - for the last  :  new request date time = (opening date time - 1 sec)
 *                      The current day with anticlockwise mode
 *
 * else
 *   - for the first :  new request date time = (opening date time - 1 sec)
 *                      The current day with clockwise mode
 *   - for the last  :  new request date time = (opening date time - 1 sec)
 *                      The day after with anticlockwise mode
 */
std::pair<DateTime, DateTime> get_daily_opening_time_bounds(const routing::datetime_stop_time& next_stop_time,
                                                            const uint32_t opening_time);

/**
 * @brief Return first and last date time
 *
 * @note The algorithm is only based on opening time and next stop time.
 */
first_and_last_stop_time get_first_and_last_stop_time(const routing::datetime_stop_time& next_stop_time,
                                                      const pt::time_duration& opening_time,
                                                      const std::vector<routing::JppIdx>& journey_pattern_points,
                                                      const DateTime max_datetime,
                                                      const type::Data& data,
                                                      const type::RTLevel rt_level,
                                                      const int utc_offset);

}  // namespace timetables
}  // namespace navitia
