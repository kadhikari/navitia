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

#include "type/vehicle_journey.h"
#include "type/indexes.h"
#include "type/pt_data.h"
#include "type/serialization.h"
#include "type/concerns_base_at_period.h"
#include "type/type_utils.h"

namespace navitia {
namespace type {

template <class Archive>
void VehicleJourney::save(Archive& ar, const unsigned int) const {
    ar& name& uri& route& physical_mode& company& validity_patterns& idx& stop_time_list& realtime_level&
        vehicle_journey_type& odt_message& _vehicle_properties& next_vj& prev_vj& meta_vj& shift& dataset;
}
template <class Archive>
void VehicleJourney::load(Archive& ar, const unsigned int) {
    ar& name& uri& route& physical_mode& company& validity_patterns& idx& stop_time_list& realtime_level&
        vehicle_journey_type& odt_message& _vehicle_properties& next_vj& prev_vj& meta_vj& shift& dataset;

    // due to circular references we can't load the vjs in the dataset using only boost::serialize
    // so we need to save the vj in it's dataset
    if (dataset) {
        dataset->vehiclejourney_list.insert(this);
    }
}
SPLIT_SERIALIZABLE(VehicleJourney)

template <class Archive>
void DiscreteVehicleJourney::serialize(Archive& ar, const unsigned int) {
    ar& boost::serialization::base_object<VehicleJourney>(*this);
}
SERIALIZABLE(DiscreteVehicleJourney)

template <class Archive>
void FrequencyVehicleJourney::serialize(Archive& ar, const unsigned int) {
    ar& boost::serialization::base_object<VehicleJourney>(*this);

    ar& start_time& end_time& headway_secs;
}
SERIALIZABLE(FrequencyVehicleJourney)

bool VehicleJourney::operator<(const VehicleJourney& other) const {
    if (this->route != other.route) {
        return *this->route < *other.route;
    }
    return this->uri < other.uri;
}

std::vector<boost::shared_ptr<disruption::Impact>> VehicleJourney::get_impacts() const {
    std::vector<boost::shared_ptr<disruption::Impact>> result;
    // considering which impact concerns this vj
    for (const auto& impact : meta_vj->get_impacts()) {
        // checking if impact concerns the period where this vj is valid (base-schedule centric)
        auto vp_functor = [&](const unsigned) {
            return false;  // we don't need to carry on when we find a day concerned
        };
        if (concerns_base_at_period(*this, realtime_level, impact->application_periods, vp_functor, false)) {
            result.push_back(impact);
        }
    }
    return result;
}

Indexes VehicleJourney::get(Type_e type, const PT_Data& data) const {
    Indexes result;
    switch (type) {
        case Type_e::Route:
            result.insert(route->idx);
            break;
        case Type_e::Company:
            result.insert(company->idx);
            break;
        case Type_e::PhysicalMode:
            result.insert(physical_mode->idx);
            break;
        case Type_e::ValidityPattern:
            result.insert(base_validity_pattern()->idx);
            break;
        case Type_e::MetaVehicleJourney:
            result.insert(meta_vj->idx);
            break;
        case Type_e::Dataset:
            if (dataset) {
                result.insert(dataset->idx);
            }
            break;
        case Type_e::Impact:
            return data.get_impacts_idx(get_impacts());
        default:
            break;
    }
    return result;
}

VehicleJourney::~VehicleJourney() {}
FrequencyVehicleJourney::~FrequencyVehicleJourney() {}
DiscreteVehicleJourney::~DiscreteVehicleJourney() {}
int32_t VehicleJourney::utc_to_local_offset() const {
    const auto* vp = validity_patterns[realtime_level];
    if (!vp) {
        throw navitia::recoverable_exception("vehicle journey " + uri + " not valid, no validitypattern on "
                                             + get_string_from_rt_level(realtime_level));
    }
    return meta_vj->tz_handler->get_utc_offset(*vp);
}

const VehicleJourney* VehicleJourney::get_corresponding_base() const {
    auto shifted_vj = get_base_canceled_validity_pattern();
    for (const auto& vj : meta_vj->get_base_vj()) {
        // if the validity pattern intersects
        if ((shifted_vj.days & vj->base_validity_pattern()->days).any()) {
            return vj.get();
        }
    }
    return nullptr;
}

uint32_t VehicleJourney::earliest_time() const {
    const auto& st = navitia::earliest_stop_time(stop_time_list);
    return std::min(st.arrival_time, st.boarding_time);
}

Indexes MetaVehicleJourney::get(Type_e type, const PT_Data& data) const {
    Indexes result;
    switch (type) {
        case Type_e::VehicleJourney:
            for_all_vjs([&](const VehicleJourney& vj) {
                result.insert(vj.idx);  // TODO use bulk insert ?
            });
            break;
        case Type_e::Impact:
            return data.get_impacts_idx(get_impacts());
        default:
            break;
    }
    return result;
}
bool FrequencyVehicleJourney::is_valid(int day, const RTLevel rt_level) const {
    if (day < 0)
        return false;
    return validity_patterns[rt_level]->check(day);
}

ValidityPattern VehicleJourney::get_vp_of_sp(const StopPoint& sp,
                                             RTLevel rt_level,
                                             const boost::posix_time::time_period& period) const {
    ValidityPattern vp_for_stop_time{validity_patterns[rt_level]->beginning_date};

    auto pass_by_the_sp = [&](const nt::StopTime& stop_time) {
        if (stop_time.stop_point->uri != sp.uri) {
            return;
        }
        const auto& beginning_date = validity_patterns[rt_level]->beginning_date;
        for (size_t i = 0; i < validity_patterns[rt_level]->days.size(); ++i) {
            if (!validity_patterns[rt_level]->days.test(i)) {
                // the VJ doesn't run at day i
                continue;
            }
            auto circulating_day = beginning_date + boost::gregorian::days{static_cast<int>(i)};
            auto arrival_time_utc = stop_time.get_arrival_utc(circulating_day);
            if (period.contains(arrival_time_utc)) {
                auto days = (circulating_day - vp_for_stop_time.beginning_date).days();
                vp_for_stop_time.add(days);
            }
        }
    };
    boost::for_each(stop_time_list, pass_by_the_sp);
    return vp_for_stop_time;
}

ValidityPattern VehicleJourney::get_vp_for_section(const std::set<StopPoint*>& section,
                                                   RTLevel rt_level,
                                                   const boost::posix_time::time_period& period) const {
    ValidityPattern vp_for_section{validity_patterns[rt_level]->beginning_date};

    auto pass_in_the_section = [&](const nt::StopTime& stop_time) {
        // Return if not in the section
        if (!section.count(stop_time.stop_point)) {
            return;
        }
        const auto& beginning_date = validity_patterns[rt_level]->beginning_date;
        for (size_t i = 0; i < validity_patterns[rt_level]->days.size(); ++i) {
            if (!validity_patterns[rt_level]->days.test(i)) {
                // the VJ doesn't run at day i
                continue;
            }
            auto circulating_day = beginning_date + boost::gregorian::days{static_cast<int>(i)};
            auto arrival_time_utc = stop_time.get_arrival_utc(circulating_day);
            if (period.contains(arrival_time_utc)) {
                auto days = (circulating_day - vp_for_section.beginning_date).days();
                vp_for_section.add(days);
            }
        }
    };
    boost::for_each(stop_time_list, pass_in_the_section);
    return vp_for_section;
}

std::set<StopPoint*> VehicleJourney::get_sections_stop_points(const StopArea* start_stop,
                                                              const StopArea* end_stop) const {
    /*
     * Identify all the smallest sections starting with start_stop and
     * ending with end_stop. Then we return the set of stop points
     * corresponding to the identified sections.
     *
     * For example, if we want the section A B:
     *    A B C D E A B
     *    ***       ***
     * we return {A, B} (not everything)
     *
     * Note:
     *
     * We are checking if the journey pass first by the start_point of
     * the line_section and the end_point. We can have start_point ==
     * end_point.
     *
     * We perform the check on the base_vj stop_times, if it exists,
     * in order to be able to apply multiple line_section disruption.
     *
     * If we have a line A :
     *
     *      s1----->s2----->s3----->s4----->s5
     *
     * if you first impact [s2, s3] then in another disruption impact
     * [s2, s4], you need the base stops
     *
     * There is also the temporal problem.
     * For example if we have two disruptions :
     *  - s4/s5 from the 1st to the 10th
     *  - s3/s4 from the 5th to the 15th
     *
     * from the 5th to the 10th the adapted vjs will not pass by s3-s4, since they're stopping
     * at s3. But the disruption s3/s4 impact s3, and the vj should not stop here anymore.
     * We will have 3 differents adapted vj then :
     *  - s1/s2/s3 from the 1st to the 4th,
     *  - s1/s2 from the 5th to the 10th,
     *  - s1/s2/s5 from the 11th to the 15th
     * */
    std::set<StopPoint*> res;
    boost::optional<uint16_t> start_idx;
    const auto* base_vj = this->get_corresponding_base();
    const auto* vj = base_vj ? base_vj : this;
    for (const auto& st : vj->stop_time_list) {
        // The line section is using stop_areas so we make sure we have one
        if (!st.stop_point || !st.stop_point->stop_area) {
            continue;
        }

        // we want the shortest section, thus, we set the idx each
        // time we see it
        if (st.stop_point->stop_area->idx == start_stop->idx) {
            start_idx = st.order();
        }

        // we want the shortest section, thus, we finish the section
        // as soon as we see the end stop
        if (start_idx && st.stop_point->stop_area->idx == end_stop->idx) {
            for (uint16_t i = *start_idx, last = st.order(); i <= last; ++i) {
                res.insert(vj->stop_time_list[i].stop_point);
            }

            // the section is finished, we are no more in a section
            start_idx = boost::none;
        }
    }
    return res;
}

boost::posix_time::time_period VehicleJourney::execution_period(const boost::gregorian::date& date) const {
    uint32_t first_departure = std::numeric_limits<uint32_t>::max();
    uint32_t last_arrival = 0;
    for (const auto& st : stop_time_list) {
        if (st.pick_up_allowed() && first_departure > st.boarding_time) {
            first_departure = st.boarding_time;
        }
        if (st.drop_off_allowed() && last_arrival < st.alighting_time) {
            last_arrival = st.alighting_time;
        }
    }
    namespace bt = boost::posix_time;
    return bt::time_period(bt::ptime(date, bt::seconds(first_departure)), bt::ptime(date, bt::seconds(last_arrival)));
}

bool VehicleJourney::has_datetime_estimated() const {
    for (const StopTime& st : this->stop_time_list) {
        if (st.date_time_estimated()) {
            return true;
        }
    }
    return false;
}

bool VehicleJourney::has_zonal_stop_point() const {
    for (const StopTime& st : this->stop_time_list) {
        if (st.stop_point->is_zonal) {
            return true;
        }
    }
    return false;
}

bool VehicleJourney::has_odt() const {
    for (const StopTime& st : this->stop_time_list) {
        if (st.odt()) {
            return true;
        }
    }
    return false;
}

bool VehicleJourney::has_boarding() const {
    std::string physical_mode;
    if (this->physical_mode != nullptr)
        physical_mode = this->physical_mode->name;
    if (!physical_mode.empty()) {
        boost::to_lower(physical_mode);
        return (physical_mode == "boarding");
    }
    return false;
}

bool VehicleJourney::has_landing() const {
    std::string physical_mode;
    if (this->physical_mode != nullptr)
        physical_mode = this->physical_mode->name;
    if (!physical_mode.empty()) {
        boost::to_lower(physical_mode);
        return (physical_mode == "landing");
    }
    return false;
}

ValidityPattern VehicleJourney::get_base_canceled_validity_pattern() const {
    ValidityPattern base_canceled_vp = *validity_patterns[realtime_level];
    base_canceled_vp.days >>= shift;
    return base_canceled_vp;
}

std::string VehicleJourney::get_direction() const {
    if (!this->stop_time_list.empty()) {
        const auto& st = this->stop_time_list.back();
        if (st.stop_point != nullptr) {
            for (const auto* admin : st.stop_point->admin_list) {
                if (admin->level == 8) {
                    return st.stop_point->name + " (" + admin->name + ")";
                }
            }
            return st.stop_point->name;
        }
    }
    return "";
}

}  // namespace type
}  // namespace navitia
