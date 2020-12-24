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
#include "ed/data.h"
#include <utils/logger.h>
#include "ed/connectors/conv_coord.h"

namespace ed {
namespace connectors {

struct PoiParserException : public navitia::exception {
    PoiParserException(const std::string& message) : navitia::exception(message) {}
    PoiParserException(const PoiParserException&) = default;
    PoiParserException& operator=(const PoiParserException&) = default;
    virtual ~PoiParserException() noexcept;
};

class PoiParser {
private:
    std::string path;  ///< Chemin vers les fichiers
    log4cplus::Logger logger;
    ed::connectors::ConvCoord conv_coord = ConvCoord(Projection());

    void fill_poi_type();
    void fill_poi();
    void fill_poi_properties();

public:
    ed::Georef data;

    PoiParser(const std::string& path);
    void fill();
};
}  // namespace connectors
}  // namespace ed
