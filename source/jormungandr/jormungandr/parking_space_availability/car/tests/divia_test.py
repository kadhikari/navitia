# encoding: utf-8
# Copyright (c) 2001-2018, Canal TP and/or its affiliates. All rights reserved.
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

import pytest
from mock import MagicMock

from jormungandr.parking_space_availability.car.divia import DiviaProvider
from jormungandr.parking_space_availability.car.parking_places import ParkingPlaces

poi = {
    'properties': {
        'operator': 'divia',
        'ref': '42'
    },
    'poi_type': {
        'name': 'Parking',
        'id': 'poi_type:public_parking'
    }
}

def car_park_space_availability_support_poi_test():
    """
    DIVIA car provider support
    """
    provider = DiviaProvider("fake.url", {'Divia'}, 'toto', 42)
    assert provider.support_poi(poi)

    invalid_poi = {}
    assert not provider.support_poi(invalid_poi)

def car_park_space_get_information_test():

    parking_places = ParkingPlaces(available=4,
                                   occupied=3)

    provider = DiviaProvider("fake.url", {'Divia'}, 'toto', 42)
    divia_response = """
    {
        "records":[
            {
                "fields": {
                    "numero_parking": "42",
                    "nombre_places_libres": 4,
                    "nombre_places": 7
                }
            }
        ]
    }
    """
    import json
    provider._call_webservice = MagicMock(return_value=json.loads(divia_response))
    info = provider.get_informations(poi)
    assert info == parking_places
    assert not hasattr(info, "available_PRM")
    assert not hasattr(info, "occupied_PRM")

    provider._call_webservice = MagicMock(return_value=None)
    assert provider.get_informations(poi) is None

    divia_response = """
    {
        "records":[
            {
                "fields": {
                }
            }
        ]
    }
    """

    provider._call_webservice = MagicMock(return_value=json.loads(divia_response))
    assert provider.get_informations(poi) is None

    divia_response = """
    {
        "records":[
        ]
    }
    """
    provider._call_webservice = MagicMock(return_value=json.loads(divia_response))
    assert provider.get_informations(poi) is None

    divia_response = """
    {
        "records":[
            {
                "fields": {
                    "numero_parking": "42"
                }
            }
        ]
    }
    """
    empty_parking = ParkingPlaces(available=None,
                                  occupied=None,
                                  available_PRM=None,
                                  occupied_PRM=None)
    provider._call_webservice = MagicMock(return_value=json.loads(divia_response))
    assert provider.get_informations(poi) == empty_parking
