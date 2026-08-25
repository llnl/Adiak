// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: MIT

#include "adiak.h"
#include "adiak_tool.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef USE_MPI
#include <mpi.h>
#endif

static void print_nameval(const char *name, adiak_value_t *value, adiak_datatype_t *t, adiak_record_info_t *, void *)
{
    static const char* whitespace = "                    ";
    size_t len = strlen(name);
    size_t pad = (len > 20 ? 20 : len);
    printf("%s%s : ", name, whitespace+pad);
    adiak_print_value(value, t);
    puts("");
}

int main(int argc, char* argv[])
{
    int use_mpi = 0;

    int argp = 1;
    for ( ; argp < argc; ++argp) {
        if (strncmp(argv[argp], "--", 2) != 0)
            break;
        if (strcmp(argv[argp], "--version") == 0) {
            printf("%d.%d.%d\n", ADIAK_VERSION, ADIAK_MINOR_VERSION, ADIAK_POINT_VERSION);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[argp], "--mpi") == 0)
            use_mpi = 1;
    }

    void* comm_ptr = NULL;

#if USE_MPI
    MPI_Comm comm = MPI_COMM_NULL;
    if (use_mpi) {
        MPI_Init(&argc, &argv);
        comm = MPI_COMM_WORLD;
        comm_ptr = &comm;
    }
#endif

    adiak_init(comm_ptr);
    adiak_collect_all();

    if (argp < argc) {
        for ( ; argp < argc; ++argp) {
            adiak_datatype_t* t = NULL;
            adiak_value_t* val = NULL;
            adiak_record_info_t* info = NULL;
            const char* name = argv[argp];
            int ret = adiak_get_nameval_with_info(name, &t, &val, &info);
            if (ret < 0)
                fprintf(stderr, "adiak: value \"%s\" not found\n", name);
            else
                print_nameval(name, val, t, info, NULL);
        }
    } else {
        adiak_list_namevals_with_info(1, adiak_category_all, print_nameval, NULL);
    }

    adiak_fini();

#if USE_MPI
    int mpi_initialized = 0;
    MPI_Initialized(&mpi_initialized);
    if (mpi_initialized)
        MPI_Finalize();
#endif
    return EXIT_SUCCESS;
}