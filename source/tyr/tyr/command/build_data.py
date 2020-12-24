# Copyright (c) 2001-2014, Canal TP and/or its affiliates. All rights reserved.
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

from flask_script import Command, Option
from tyr.tasks import build_all_data, build_data
import logging
from navitiacommon import models


class BuildDataCommand(Command):
    """A command used to build all the datasets
    """

    def get_options(self):
        return [
            Option(dest='instance_name', help="name of the instance to build. If non given, build all instances")
        ]

    def run(self, instance_name=None):
        if not instance_name:
            logging.info("Building all data")
            return build_all_data()
        instance = models.Instance.query_existing().filter_by(name=instance_name).first()
        return build_data(instance)
