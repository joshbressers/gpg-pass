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

import sys
import optparse
import gpgpass_config
import get_password
import set_password
import hashlib
import os

def main():

    # Command line examples
    #
    # -s is always a valid option
    #
    # Display all sites:
    #   gpgpass
    #
    # Show login info for a certain site:
    #   gpgpass somesite.com
    #
    # Add/edit a site's information:
    #   gpgpass -a -r user@example.com site.com

    usage = "usage: %prog [options] site"
    parser = optparse.OptionParser(usage=usage)
    parser.add_option("-s", "--store", dest="password_store",
                      help="Path to the password store", metavar="DIRECTORY")
    parser.add_option("-r", "--recipient", dest="recipient", action="append",
                      help="Recipient to encrypt the site to. May be used multiple times", metavar="NAME")

    (options, args) = parser.parse_args()
    global_config = gpgpass_config.get_config()

    if options.password_store:
        global_config.set('store', options.password_store)

    if global_config.get('store') is None:
        parser.print_help()
        sys.exit(1)


    if len(args) == 0:
        get_password.print_all_sites(global_config.get('store'))
        sys.exit(0)

    if len(args) > 1:
        parser.print_help()
        sys.exit(1)


    global_config.set('site', args[0])

    if options.recipient:
        global_config.set('recipient', options.recipient)
        set_password.add()
    else:
        get_password.get_site()

if __name__ == "__main__":
    main()
