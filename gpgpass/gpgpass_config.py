#!/usr/bin/env python
#
# This file is part of gpg-pass.
#
# gpg-pass is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# gpg-pass is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with gpg-pass.  If not, see <http://www.gnu.org/licenses/>.

import os

class Config:
    "Class to contain the various configuration derectives."

    config = {  'store' : None,
                'site'  : None,
                'recipient' : []
             }

    def __init__(self):

        if os.environ.has_key('GPGPASS_DATABASE'):
            self.config['store'] = os.environ['GPGPASS_DATABASE']

    def get(self, name):
        "Get the configuration value 'name'"
        return self.config[name]

    def set(self, name, value):
        "Set the configuration value to 'name'"
        self.config[name] = value


configuration = Config()

def get_config():
    return configuration
