/*
 * This file is part of gpg-pass.
 *
 * gpg-pass is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gpg-pass is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gpg-pass.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <gpgme.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {

    int fd;
    ssize_t len;
    char buffer[1024];

    gpgme_ctx_t ctx;
    gpgme_error_t err;
    gpgme_data_t in, out;

    /* This is needed or the library segfaults */
    gpgme_check_version (NULL);

    /* New context */
    err = gpgme_new(&ctx);
    if (err) {
         fprintf (stderr, "%s: gpgme_new(): %s: %s\n",
                  argv[0], gpgme_strsource (err), gpgme_strerror (err));
         exit (1);
    }

    err = gpgme_data_new(&out);
    if (err) {
         fprintf (stderr, "%s: gpg_data_new(): %s: %s\n",
                  argv[0], gpgme_strsource (err), gpgme_strerror (err));
         exit (1);
    }

    /* Open our file for reading */
    char *filename;
    filename = argv[1];
    fd = open(filename, O_RDONLY);
    err = gpgme_data_new_from_fd(&in, fd);
    if (err) {
         fprintf (stderr, "%s: gpgme_data_new_from_fd(): %s: %s\n",
                  argv[0], gpgme_strsource (err), gpgme_strerror (err));
         exit (1);
    }

    err = gpgme_op_decrypt(ctx, in, out);
    if (err) {
         fprintf (stderr, "%s: gpgme_op_decrypt(): %s: %s\n",
                  argv[0], gpgme_strsource (err), gpgme_strerror (err));
         exit (1);
    }

    /* We have to "seek" our output buffer back to zero */
    gpgme_data_seek(out, 0, SEEK_SET);

    len = gpgme_data_read(out, buffer, sizeof(buffer));
    buffer[len] = '\0';
    if (len == 0) {
        puts("Nothing read");
    } else {
        printf("%s\n", buffer);
    }

    gpgme_data_release(in);
    gpgme_data_release(out);
    gpgme_release(ctx);
    exit(0);
}
