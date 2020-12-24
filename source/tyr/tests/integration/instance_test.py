# coding: utf-8
# Copyright (c) 2001-2018, Canal TP and/or its affiliates. All rights reserved.
#
# This file is part of Navitia,
#     the software to build cool stuff with public transport.
#
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

from __future__ import absolute_import, print_function, division
from tests.check_utils import api_get, api_post, api_delete, api_put, _dt
from tests.integration.equipment_providers_test import default_equipments_config
import json
import pytest
from navitiacommon import models
from tyr import app


@pytest.fixture
def create_instance():
    with app.app_context():
        instance = models.Instance('fr')
        models.db.session.add(instance)
        models.db.session.commit()
        return instance.id


def check_traveler_profile(profile, params):
    for key, param in params.iteritems():
        assert profile[key] == param


@pytest.fixture
def traveler_profile_params():
    return {
        "last_section_mode": ["walking", "bss", "car"],
        "walking_speed": 5.1,
        "max_car_duration_to_pt": 200,
        "wheelchair": False,
        "max_bss_duration_to_pt": 100,
        "max_walking_duration_to_pt": 300,
        "first_section_mode": ["walking"],
        "bss_speed": 6.2,
        "bike_speed": 8.8,
        "car_speed": 23.11,
        "max_bike_duration_to_pt": 500,
    }


def test_get_instances_empty():
    resp = api_get('/v0/instances/')
    assert resp == []


def test_get_instances(create_instance):
    resp = api_get('/v0/instances/')
    assert len(resp) == 1
    assert resp[0]['name'] == 'fr'
    assert resp[0]['id'] == create_instance


def test_get_instance(create_instance):
    resp = api_get('/v0/instances/fr')
    assert len(resp) == 1
    assert resp[0]['name'] == 'fr'
    assert resp[0]['id'] == create_instance
    resp = api_get('/v0/instances/{}'.format(create_instance))
    assert len(resp) == 1
    assert resp[0]['name'] == 'fr'
    assert resp[0]['id'] == create_instance


def test_update_instances(create_instance):
    params = {
        "journey_order": "arrival_time",
        "max_duration": 200,
        "max_bss_duration_to_pt": 10,
        "max_nb_transfers": 5,
        "bike_speed": 2.2,
        "walking_transfer_penalty": 20,
        "night_bus_filter_base_factor": 300,
        "walking_speed": 1.62,
        "priority": 4,
        "car_speed": 55.55,
        "min_bike": 40,
        "max_walking_duration_to_pt": 300,
        "min_car": 400,
        "min_taxi": 263,
        "max_bike_duration_to_pt": 600,
        "scenario": "new_default",
        "bss_speed": 2.1,
        "min_bss": 40,
        "night_bus_filter_max_factor": 1.5,
        "max_car_duration_to_pt": 800,
        "bss_provider": False,
        "full_sn_geometries": True,
        "max_car_no_park_duration_to_pt": 2691,
        "car_no_park_speed": 2.42,
        "min_nb_journeys": 1,
        "max_nb_journeys": None,
        "min_journeys_calls": 2,
        "max_successive_physical_mode": 3,
        "final_line_filter": True,
        "max_extra_second_pass": 1,
        "additional_time_after_first_section_taxi": 42,
        "additional_time_before_last_section_taxi": 789,
    }

    resp = api_put('/v0/instances/fr', data=json.dumps(params), content_type='application/json')
    for key, param in params.iteritems():
        assert resp[key] == param

    resp = api_put(
        '/v0/instances/{}'.format(create_instance), data=json.dumps(params), content_type='application/json'
    )
    for key, param in params.iteritems():
        assert resp[key] == param


def test_update_instances_is_free(create_instance):
    params = {"is_free": True}
    resp = api_put(
        '/v0/instances/{}'.format(create_instance), data=json.dumps(params), content_type='application/json'
    )
    assert resp['is_free'] == True
    assert resp['is_open_data'] == False

    params = {"is_free": False}
    resp = api_put(
        '/v0/instances/{}'.format(create_instance), data=json.dumps(params), content_type='application/json'
    )
    assert resp['is_free'] == False
    assert resp['is_open_data'] == False

    params = {"is_open_data": True}
    resp = api_put(
        '/v0/instances/{}'.format(create_instance), data=json.dumps(params), content_type='application/json'
    )
    assert resp['is_free'] == False
    assert resp['is_open_data'] == True

    params = {"is_open_data": False}
    resp = api_put(
        '/v0/instances/{}'.format(create_instance), data=json.dumps(params), content_type='application/json'
    )
    assert resp['is_free'] == False
    assert resp['is_open_data'] == False

    params = {"is_open_data": True, 'is_free': True}
    resp = api_put(
        '/v0/instances/{}'.format(create_instance), data=json.dumps(params), content_type='application/json'
    )
    assert resp['is_free'] == True
    assert resp['is_open_data'] == True


def test_delete_instance_by_id(create_instance):
    resp = api_delete('/v0/instances/{}'.format(create_instance))
    assert resp['id'] == create_instance
    assert resp['discarded'] == True

    # check response to get with different use cases
    resp = api_get('/v0/instances/')
    assert resp == []
    resp = api_get('/v0/instances/fr')
    assert resp == []
    resp = api_get('/v0/instances/{}'.format(create_instance))
    assert resp == []

    # delete by id is idempotent
    resp, status = api_delete('/v0/instances/{}'.format(create_instance), check=False)
    assert status == 200


def test_delete_instance_by_name(create_instance):
    resp = api_delete('/v0/instances/fr')
    assert resp['id'] == create_instance
    assert resp['discarded'] == True
    resp = api_get('/v0/instances/')
    assert resp == []

    # delete by name is not idempotent
    resp, status = api_delete('/v0/instances/fr', check=False)
    assert status == 404


def test_update_invalid_scenario(create_instance):
    params = {"scenario": "foo"}
    resp, status = api_put(
        '/v0/instances/fr', data=json.dumps(params), check=False, content_type='application/json'
    )
    assert status == 400


def test_update_invalid_instance(create_instance):
    params = {"scenario": "foo"}
    resp, status = api_put(
        '/v0/instances/us', data=json.dumps(params), check=False, content_type='application/json'
    )
    assert status == 404


def test_get_non_existant_profile(create_instance):
    """
    by default there is no traveler profile created for an instance
    """
    _, status = api_get('/v0/instances/fr/traveler_profiles/standard', check=False)
    assert status == 404


def test_create_empty_traveler_profile(create_instance):
    """
    we have created a profile with all default value, totally useless...
    """
    api_post('/v0/instances/fr/traveler_profiles/standard')
    api_get('/v0/instances/fr/traveler_profiles/standard')


def test_create_traveler_profile(create_instance, traveler_profile_params):
    resp = api_post(
        '/v0/instances/fr/traveler_profiles/standard',
        data=json.dumps(traveler_profile_params),
        content_type='application/json',
    )

    check_traveler_profile(resp, traveler_profile_params)
    resp = api_get('/v0/instances/fr/traveler_profiles/standard')
    check_traveler_profile(resp[0], traveler_profile_params)


def test_update_traveler_profile(create_instance, traveler_profile_params):

    api_post('/v0/instances/fr/traveler_profiles/standard')

    resp = api_put(
        '/v0/instances/fr/traveler_profiles/standard',
        data=json.dumps(traveler_profile_params),
        content_type='application/json',
    )
    check_traveler_profile(resp, traveler_profile_params)

    resp = api_get('/v0/instances/fr/traveler_profiles/standard')
    check_traveler_profile(resp[0], traveler_profile_params)


def test_delete_traveler_profile(create_instance):
    api_post('/v0/instances/fr/traveler_profiles/standard')

    _, status = api_delete('/v0/instances/fr/traveler_profiles/standard', check=False, no_json=True)
    assert status == 204

    _, status = api_get('/v0/instances/fr/traveler_profiles/standard', check=False)
    assert status == 404


def test_update_instances_with_invalid_scenario(create_instance):
    params = {
        "min_tc_with_bss": 5,
        "journey_order": "arrival_time",
        "max_duration": 200,
        "max_bss_duration_to_pt": 10,
        "max_nb_transfers": 5,
        "bike_speed": 2.2,
        "walking_transfer_penalty": 20,
        "night_bus_filter_base_factor": 300,
        "walking_speed": 1.62,
        "max_duration_fallback_mode": "bike",
        "priority": 4,
        "car_speed": 55.55,
        "min_tc_with_car": 100,
        "min_tc_with_bike": 100,
        "min_bike": 40,
        "max_walking_duration_to_pt": 300,
        "min_car": 400,
        "min_taxi": 263,
        "max_bike_duration_to_pt": 600,
        "max_duration_criteria": "duration",
        "scenario": "stif",
        "bss_speed": 2.1,
        "min_bss": 40,
        "night_bus_filter_max_factor": 1.5,
        "max_car_duration_to_pt": 800,
        "bss_provider": False,
        "full_sn_geometries": True,
        "max_car_no_park_duration_to_pt": 2691,
        "car_no_park_speed": 2.42,
        "taxi_speed": 2.77,
        "min_nb_journeys": 1,
        "max_nb_journeys": None,
        "min_journeys_calls": 2,
        "max_successive_physical_mode": 3,
        "final_line_filter": True,
        "max_extra_second_pass": 1,
    }

    resp, status = api_put(
        '/v0/instances/fr', data=json.dumps(params), check=False, content_type='application/json'
    )
    assert status == 400

    resp = api_get('/v0/instances/')
    assert resp[0]['scenario'] == 'new_default'


def test_update_max_nb_crowfly_by_mode(create_instance):
    resp = api_get('/v0/instances/fr')
    assert resp[0]['max_nb_crowfly_by_mode']['car'] == 5000
    assert resp[0]['max_nb_crowfly_by_mode']['walking'] == 5000
    assert resp[0]['max_nb_crowfly_by_mode']['bike'] == 5000
    assert resp[0]['max_nb_crowfly_by_mode']['bss'] == 5000
    assert resp[0]['max_nb_crowfly_by_mode']['taxi'] == 5000

    params = {"max_nb_crowfly_by_mode": {'car': 4242, 'walking': 4141, 'taxi': 2323}}
    resp, status = api_put(
        '/v0/instances/fr', data=json.dumps(params), check=False, content_type='application/json'
    )
    assert status == 200
    assert resp['max_nb_crowfly_by_mode']['car'] == 4242
    assert resp['max_nb_crowfly_by_mode']['walking'] == 4141
    assert resp['max_nb_crowfly_by_mode']['bike'] == 5000
    assert resp['max_nb_crowfly_by_mode']['bss'] == 5000
    assert resp['max_nb_crowfly_by_mode']['taxi'] == 2323


def test_update_autocomplete_backend(create_instance):
    resp = api_get('/v0/instances/fr')
    assert resp[0]['autocomplete_backend'] == 'kraken'

    params = {'autocomplete_backend': 'bragi'}
    resp = api_put('/v0/instances/fr', data=json.dumps(params), content_type='application/json')
    assert resp['autocomplete_backend'] == 'bragi'

    resp = api_get('/v0/instances/fr')
    assert resp[0]['autocomplete_backend'] == 'bragi'


def test_update_additional_time_for_taxi(create_instance):
    resp = api_get('/v0/instances/fr')
    assert resp[0]['additional_time_after_first_section_taxi'] == 300
    assert resp[0]['additional_time_before_last_section_taxi'] == 300

    params = {'additional_time_after_first_section_taxi': 42, 'additional_time_before_last_section_taxi': 3637}
    resp = api_put('/v0/instances/fr', data=json.dumps(params), content_type='application/json')
    assert resp['additional_time_after_first_section_taxi'] == 42
    assert resp['additional_time_before_last_section_taxi'] == 3637

    resp = api_get('/v0/instances/fr')
    assert resp[0]['additional_time_after_first_section_taxi'] == 42
    assert resp[0]['additional_time_before_last_section_taxi'] == 3637


def test_update_forgotten_attributs_in_backend(create_instance):
    resp = api_get('/v0/instances/fr')
    assert resp[0]['max_additional_connections'] == 2
    assert resp[0]['successive_physical_mode_to_limit_id'] == 'physical_mode:Bus'
    assert resp[0]['car_park_provider'] == True

    params = {
        'max_additional_connections': 3,
        'successive_physical_mode_to_limit_id': 'physical_mode:Train',
        'car_park_provider': False,
    }
    resp = api_put('/v0/instances/fr', data=json.dumps(params), content_type='application/json')
    assert resp['max_additional_connections'] == 3
    assert resp['successive_physical_mode_to_limit_id'] == 'physical_mode:Train'
    assert resp['car_park_provider'] == False

    resp = api_get('/v0/instances/fr')
    assert resp[0]['max_additional_connections'] == 3
    assert resp[0]['successive_physical_mode_to_limit_id'] == 'physical_mode:Train'
    assert resp[0]['car_park_provider'] == False


def test_update_taxi_speed(create_instance):
    resp = api_get('/v0/instances/fr')
    assert resp[0]['taxi_speed'] == 11.11

    params = {'taxi_speed': 53.23}
    resp = api_put('/v0/instances/fr', data=json.dumps(params), content_type='application/json')
    assert resp['taxi_speed'] == 53.23

    resp = api_get('/v0/instances/fr')
    assert resp[0]['taxi_speed'] == 53.23


def test_equipments_instance_association(create_instance, default_equipments_config):
    """
    Test the association between an instance and equipments providers
    Note: the fixture 'default_equipments_config' defines 2 providers :'sytral' & 'sytral2'
    """

    # 'Unknown' doesn't exist in db, return error message and no update performed
    params = {'equipment_details_providers': ['Unknown']}

    resp, status = api_put(
        '/v0/instances/fr', data=json.dumps(params), content_type='application/json', check=False
    )
    assert status == 400
    assert 'message' in resp
    assert resp['message'] == "Couldn't set equipment providers - Provider 'Unknown' isn't present in db"

    # The equipments provider 'sytral' is associated to the instance 'fr'
    params = {'equipment_details_providers': ['sytral']}

    resp = api_put('/v0/instances/fr', data=json.dumps(params), content_type='application/json')
    assert 'equipment_details_providers' in resp
    assert len(resp['equipment_details_providers']) == 1
    assert resp['equipment_details_providers'][0]['id'] == 'sytral'

    # Only 'sytral2' is associated to the instance 'fr' after update
    params = {'equipment_details_providers': ['sytral2']}

    resp = api_put('/v0/instances/fr', data=json.dumps(params), content_type='application/json')
    assert 'equipment_details_providers' in resp
    assert len(resp['equipment_details_providers']) == 1
    assert resp['equipment_details_providers'][0]['id'] == 'sytral2'


def test_update_min_taxi(create_instance):
    resp = api_get('/v0/instances/fr')
    assert resp[0]['min_taxi'] == 4 * 60

    params = {'min_taxi': 7 * 60}
    resp = api_put('/v0/instances/fr', data=json.dumps(params), content_type='application/json')
    assert resp['min_taxi'] == 7 * 60

    resp = api_get('/v0/instances/fr')
    assert resp[0]['min_taxi'] == 7 * 60
