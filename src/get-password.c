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

char *read_gpg_data(char *filename) {
    /* Read the GPG data from a file */

    int fd;
    ssize_t len;
    ssize_t buffer_len = 1024;
    char *buffer;

    gpgme_ctx_t ctx;
    gpgme_error_t err;
    gpgme_data_t in, out;

    buffer = malloc(buffer_len);
    if (buffer == 0) {
        puts("malloc error");
        exit(1);
    }

    /* This is needed or the library segfaults */
    gpgme_check_version (NULL);

    /* New context */
    err = gpgme_new(&ctx);
    if (err) {
         fprintf (stderr, "gpgme_new(): %s: %s\n",
                  gpgme_strsource (err), gpgme_strerror (err));
         exit (1);
    }

    err = gpgme_data_new(&out);
    if (err) {
         fprintf (stderr, "gpg_data_new(): %s: %s\n",
                  gpgme_strsource (err), gpgme_strerror (err));
         exit (1);
    }

    /* Open our file for reading */
    fd = open(filename, O_RDONLY);
    err = gpgme_data_new_from_fd(&in, fd);
    if (err) {
         fprintf (stderr, "gpgme_data_new_from_fd(): %s: %s\n",
                  gpgme_strsource (err), gpgme_strerror (err));
         exit (1);
    }

    err = gpgme_op_decrypt(ctx, in, out);
    if (err) {
         fprintf (stderr, "gpgme_op_decrypt(): %s: %s\n",
                  gpgme_strsource (err), gpgme_strerror (err));
         exit (1);
    }

    /* We have to "seek" our output buffer back to zero */
    gpgme_data_seek(out, 0, SEEK_SET);

    len = gpgme_data_read(out, buffer, buffer_len);
    buffer[len] = '\0';

    /* gpgme cleanup */
    gpgme_data_release(in);
    gpgme_data_release(out);
    gpgme_release(ctx);

    return buffer;
}

int main(int argc, char *argv[]) {

    char *data;

    data = read_gpg_data(argv[1]);
    printf("%s\n", data);

    exit(0);
}
