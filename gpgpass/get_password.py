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
import os
import hashlib
import gpgme
import StringIO
import optparse

def print_all_sites(store):
    # This comes from
    # http://stackoverflow.com/questions/3207219/how-to-list-all-files-of-a-directory-in-python
    onlyfiles = [ f for f in os.listdir(store) if os.path.isfile(os.path.join(store,f)) ]

    for i in onlyfiles:
        try:
            output = decrypt_file(os.path.join(store, i))
            for j in output.readlines():
                if j.startswith("site: "):
                    print j.strip().split(": ")[1]
        except gpgme.GpgmeError as e:
            print "Couldn't read %s" % i
            next

def decrypt_file(encrypted_file):
    gpg_ctx = gpgme.Context()

    fh = open(encrypted_file, 'r')
    output = StringIO.StringIO()
    signatures = gpg_ctx.decrypt_verify(fh, output)
    output.seek(0)
    fh.close()

    return output

def main():

    usage = "usage: %prog [options] site"
    parser = optparse.OptionParser(usage=usage)
    parser.add_option("-s", "--store", dest="password_store",
                      help="Path to the password store", metavar="DIRECTORY")

    (options, args) = parser.parse_args()

    if not options.password_store:
        parser.print_help()
        sys.exit(1)

    password_store = options.password_store

    if len(args) == 0:
        print_all_sites(password_store)
        sys.exit(0)

    if len(args) > 1:
        parser.print_help()
        sys.exit(1)


    site = args[0]
    scrambled_site = hashlib.sha256(site).hexdigest()

    encrypted_file = os.path.join(password_store, scrambled_site)

    if not os.path.isfile(encrypted_file):
        print "No entry for %s found\n" % site
        sys.exit(1)

    try:
        output = decrypt_file(encrypted_file)
    except gpgme.GpgmeError as e:
        print "Error: %s" % e.strerror
        sys.exit(1)

    for i in output.readlines():
        print i.strip()

    sys.exit(0)


if __name__ == "__main__":
    main()
