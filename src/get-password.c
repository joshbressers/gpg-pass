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
#include <getopt.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "sha256.h"

char *read_gpg_data(char *store, char *filename) {
    /* Read the GPG data from a file */

    int fd;
    ssize_t len;
    ssize_t buffer_len = 1024;
    char *buffer;
    char *file_path;
    ssize_t file_path_size = 0;

    gpgme_ctx_t ctx;
    gpgme_error_t err;
    gpgme_data_t in, out;

    buffer = malloc(buffer_len);
    if (buffer == 0) {
        puts("malloc error");
        exit(1);
    }

    file_path_size = strlen(store) + strlen(filename) + 1;
    file_path = malloc(file_path_size + 1);
    if (buffer == 0) {
        puts("malloc error");
        exit(1);
    }

    snprintf(file_path, file_path_size + 1, "%s/%s", store, filename);

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
    fd = open(file_path, O_RDONLY);
    err = gpgme_data_new_from_fd(&in, fd);
    if (err) {
         fprintf (stderr, "gpgme_data_new_from_fd(): %s: %s\n",
                  gpgme_strsource (err), gpgme_strerror (err));
         exit (1);
    }

    err = gpgme_op_decrypt(ctx, in, out);
    if (err) {
        /* Sometimes decryption will fail */
        free(buffer);
        buffer = NULL;
        goto DONE;
    }

    /* We have to "seek" our output buffer back to zero */
    gpgme_data_seek(out, 0, SEEK_SET);

    len = gpgme_data_read(out, buffer, buffer_len);
    buffer[len] = '\0';

    /* gpgme cleanup */
    gpgme_data_release(in);
    gpgme_data_release(out);
    gpgme_release(ctx);

DONE:

    return buffer;
}

void print_all_sites(char *store) {
    /* list the names of all sites */

    DIR *dir;
    struct dirent *ent;
    char *input_data;
    int i;

    if ((dir = opendir(store)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                input_data = read_gpg_data(store, ent->d_name);
                /* Skip things not encrypted to us */
                if (input_data == NULL) continue;

                /* XXX: This is without a doubt a horrible hack. It assumes
                 * a very specific file format. It needs to be fixed
                 */
                for (i = 6; !isspace(input_data[i]); i++) {
                        /* Print the site name stored in the file */
                        printf("%c", input_data[i]);
                }
                printf("\n");
            }
        }
        closedir(dir);

    } else {
        printf("Could not open directory %s\n", store);
        exit(1);
    }
}

void print_help() {
    puts("This is some help");
    exit(1);
}

int main(int argc, char *argv[]) {

    int c;
    char *gpg_data;
    char *store = NULL;
    char site_name[256];

    uint8_t sha_output[32];
    context_sha256_t ctx;

    static struct option long_options[] = {
        {"store", required_argument, NULL, 's'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "s:", long_options, NULL)) != -1) {
        switch(c) {
            case 's':
                store = optarg;
                break;
            case '?':
                /* fallthrough */
            default:
                print_help();
        }
    }

    if (store == NULL)
        print_help();

    if (argv[optind] == NULL) {
        print_all_sites(store);
    } else {

        sha256_starts(&ctx);
        sha256_update(&ctx, (uint8_t *) argv[optind], strlen(argv[optind]));
        sha256_finish(&ctx, sha_output);

        for (c = 0; c < 32; c++) {
            sprintf(site_name + (c * 2), "%02x", sha_output[c]);
        }
        site_name[c*2] = '\0';

        gpg_data = read_gpg_data(store, site_name);
        if (gpg_data) {
            printf("%s\n", gpg_data);
        } else {
            puts("No Data");
        }
    }

    exit(0);
}
