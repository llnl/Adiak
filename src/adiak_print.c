// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: MIT

#include "adiak.h"
#include "adiak_tool.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

void adiak_print_value(adiak_value_t *val, adiak_datatype_t *t)
{
   if (!t)
      printf("ERROR");
   switch (t->dtype) {
      case adiak_type_unset:
         printf("UNSET");
         break;
      case adiak_long:
         printf("%ld", val->v_long);
         break;
      case adiak_ulong:
         printf("%lu", (unsigned long) val->v_long);
         break;
      case adiak_longlong:
         printf("%lld", val->v_longlong);
         break;
      case adiak_ulonglong:
         printf("%llu", (unsigned long long) val->v_longlong);
         break;
      case adiak_int:
         printf("%d", val->v_int);
         break;
      case adiak_uint:
         printf("%u", (unsigned int) val->v_int);
         break;
      case adiak_double:
         printf("%f", val->v_double);
         break;
      case adiak_date: {
         char datestr[512];
         signed long seconds_since_epoch = (signed long) val->v_long;
         struct tm *loc = localtime(&seconds_since_epoch);
         strftime(datestr, sizeof(datestr), "%a, %d %b %Y %T %z", loc);
         printf("%s", datestr);
         break;
      }
      case adiak_timeval: {
         struct timeval *tval = (struct timeval *) val->v_ptr;
         double duration = tval->tv_sec + (tval->tv_usec / 1000000.0);
         printf("%fs:timeval", duration);
         break;
      }
      case adiak_version: {
         char *s = (char *) val->v_ptr;
         printf("\"%s\":version", s);
         break;
      }
      case adiak_string: {
         char *s = (char *) val->v_ptr;
         printf("\"%s\":string", s);
         break;
      }
      case adiak_catstring: {
         char *s = (char *) val->v_ptr;
         printf("\"%s\":catstring", s);
         break;
      }
      case adiak_jsonstring: {
         char *s = (char *) val->v_ptr;
         printf("\"%s\":jsonstring", s);
         break;
      }
      case adiak_path: {
         char *s = (char *) val->v_ptr;
         printf("\"%s\":path", s);
         break;
      }
      case adiak_range: {
         adiak_value_t subvals[2];
         adiak_datatype_t* subtypes[2];

         adiak_get_subval(t, val, 0, subtypes+0, subvals+0);
         adiak_get_subval(t, val, 1, subtypes+1, subvals+1);

         adiak_print_value(subvals+0, *(subtypes+0));
         printf(" - ");
         adiak_print_value(subvals+1, *(subtypes+1));
         break;
      }
      case adiak_set: {
         printf("[");
         int num_elements = adiak_num_subvals(t);
         for (int i = 0; i < num_elements; i++) {
            adiak_value_t subval;
            adiak_datatype_t* subtype;
            adiak_get_subval(t, val, i, &subtype, &subval);
            adiak_print_value(&subval, subtype);
            if (i+1 != num_elements)
               printf(", ");
         }
         printf("]");
         break;
      }
      case adiak_list: {
         printf("{");
         int num_elements = adiak_num_subvals(t);
         for (int i = 0; i < num_elements; i++) {
            adiak_value_t subval;
            adiak_datatype_t* subtype;
            adiak_get_subval(t, val, i, &subtype, &subval);
            adiak_print_value(&subval, subtype);
            if (i+1 != num_elements)
               printf(", ");
         }
         printf("}");
         break;
      }
      case adiak_tuple: {
         printf("(");
         int num_elements = adiak_num_subvals(t);
         for (int i = 0; i < num_elements; i++) {
            adiak_value_t subval;
            adiak_datatype_t* subtype;
            adiak_get_subval(t, val, i, &subtype, &subval);
            adiak_print_value(&subval, subtype);
            if (i+1 != num_elements)
               printf(", ");
         }
         printf(")");
         break;
      }
   }
}
