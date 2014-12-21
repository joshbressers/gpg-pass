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

from __future__ import print_function

import sys
import os
import hashlib
import gpgme
import StringIO
import getpass
import optparse

def main():

    usage = "usage: %prog [options] site"
    parser = optparse.OptionParser(usage=usage)
    parser.add_option("-s", "--store", dest="password_store",
                      help="Path to the password store", metavar="DIRECTORY")
    parser.add_option("-r", "--recipient", dest="recipient", action="append",
                      help="Recipient to encrypt the site to. May be used multiple times", metavar="NAME")

    password_store = None

    (options, args) = parser.parse_args()

    if len(args) != 1:
        parser.print_help()
        sys.exit(1)

    if os.environ.has_key('GPGPASS_DATABASE'):
        password_store = os.environ['GPGPASS_DATABASE']

    # Let the command line switch override the environment variable
    if options.password_store:
        password_store = options.password_store

    if password_store is None:
        parser.print_help()
        sys.exit(1)

    if len(options.recipient) == 0:
        parser.print_help()
        sys.exit(1)

    site = args[0]
    new_entry = True

    site_data = {
                    'site'      : None,
                    'username'  : None,
                    'password'  : None
                }

    scrambled_site = hashlib.sha256(site).hexdigest()

    encrypted_file = os.path.join(password_store, scrambled_site)

    # If this file exists, let's update it
    if os.path.isfile(encrypted_file):
        new_entry = False

    gpg_ctx = gpgme.Context()

    if new_entry == False:
        fh = open(encrypted_file, 'r')
        output = StringIO.StringIO()
        signatures = gpg_ctx.decrypt_verify(fh, output)

        output.seek(0)
        for i in output.readlines():
            data = i.strip().split(': ')
            site_data[data[0]] = data[1]

        fh.close()

        print("Updating %s" % site)
    else:
        print("Adding %s" % site)

    if site_data['username'] is None:
        print("Username: ", end="")
    else:
        print("Username: [%s] " % site_data['username'], end="")

    username = sys.stdin.readline().strip()
    if username == '':
        username = site_data['username']

    if site_data['password'] is None:
        print("Password: ", end="")
    else:
        print("Password: [return for no change] ", end="")

    # We want a blank password prompt
    password = getpass.getpass("")
    if password == '':
        password = site_data['password']

    site_data['site'] = site
    site_data['username'] = username
    site_data['password'] = password

    plaintext_data = "%s: %s\n%s: %s\n%s: %s" % ('site', site_data['site'],
                                                 'username', site_data['username'],
                                                 'password', site_data['password'])

    keys = []
    for i in options.recipient:
        keys.append(gpg_ctx.get_key(i))

    plaintext = StringIO.StringIO(plaintext_data)
    ciphertext = StringIO.StringIO()
    gpg_ctx.armor = True
    gpg_ctx.encrypt(keys, gpgme.ENCRYPT_ALWAYS_TRUST, plaintext, ciphertext)

    ciphertext.seek(0)

    fh = open(encrypted_file, 'w')
    fh.writelines(ciphertext.readlines())
    fh.close

if __name__ == "__main__":
    main()

