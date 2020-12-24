# Copyright (c) 2001-2017, Canal TP and/or its affiliates. All rights reserved.
#
# This file is part of Navitia,
#     the software to build cool stuff with public transport.
#
# Hope you'll enjoy and contribute to this project,
#     powered by Canal TP (www.canaltp.fr).
# Help us simplify mobility and open public transport:
#     a non ending quest to the responsive locomotion way of traveling!
#
# LICENCE: This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# Stay tuned using
# twitter @navitia
# IRC #navitia on freenode
# https://groups.google.com/d/forum/navitia
# www.navitia.io

from __future__ import absolute_import, print_function, unicode_literals, division
from jormungandr.interfaces.v1.serializer.pt import PlaceSerializer
from jormungandr.interfaces.v1.serializer.time import DateTimeField
from jormungandr.interfaces.v1.serializer.jsonschema import JsonStrField, Field
import serpy


class CellLatSchema(serpy.Serializer):
    # This Class is not used as a serializer, but here only to get the schema
    min_lat = Field(schema_type=float)
    max_lat = Field(schema_type=float)
    center_lat = Field(schema_type=float)


class LineHeadersSchema(serpy.Serializer):
    # This Class is not used as a serializer, but here only to get the schema
    cell_lat = CellLatSchema()


class CellLonSchema(serpy.Serializer):
    # This Class is not used as a serializer, but here only to get the schema
    center_lon = Field(schema_type=float)
    max_lon = Field(schema_type=float)
    min_lon = Field(schema_type=float)


class LinesSchema(serpy.Serializer):
    # This Class is not used as a serializer, but here only to get the schema
    duration = Field(schema_type=int, many=True)
    cell_lon = CellLonSchema()


class HeatMatrixSchema(serpy.Serializer):
    # This Class is not used as a serializer, but here only to get the schema
    line_headers = LineHeadersSchema(many=True)
    lines = LinesSchema(many=True)


class HeatMapSerializer(serpy.Serializer):
    heat_matrix = JsonStrField(schema_type=HeatMatrixSchema)
    origin = PlaceSerializer(label='from')
    to = PlaceSerializer(attr='destination', label='to')
    requested_date_time = DateTimeField()
