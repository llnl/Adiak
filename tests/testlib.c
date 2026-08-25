#include "adiak_tool.h"

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>

#define XSTR(S) #S
#define STR(S) XSTR(S)

#ifdef __GNUC__
#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) UNUSED_ ## x
#endif


static void print_nameval(const char *name, adiak_value_t *value, adiak_datatype_t *t, adiak_record_info_t *info, void *UNUSED(opaque_value))
{
   double timestamp = ((double) info->timestamp.tv_sec) + (info->timestamp.tv_nsec * 1e-9);
   printf("%s - %f - %s: ", STR(TOOLNAME), timestamp, name);
   adiak_print_value(value, t);
   printf("\n");
}

static void print_on_flush(const char *name, int UNUSED(category), const char *UNUSED(subcategory), adiak_value_t *UNUSED(value), adiak_datatype_t *UNUSED(t), void *UNUSED(opaque_value))
{
   if (strcmp(name, "flush") != 0)
      return;
   adiak_list_namevals_with_info(1, adiak_category_all, print_nameval, NULL);
}

void print_all_adiak_vars()
{
   adiak_list_namevals_with_info(1, adiak_category_all, print_nameval, NULL);
}

static void onload() __attribute__((constructor));
static void onload()
{
   if (strcmp(STR(TOOLNAME), "TOOL3") == 0)
      adiak_register_cb(1, adiak_control, print_on_flush, 0, NULL);
   else
      adiak_register_cb_with_info(1, adiak_category_all, print_nameval, 0, NULL);
}
