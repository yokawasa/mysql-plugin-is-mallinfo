/*
Copyright 2009 Yoichi Kawasaki

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifdef DBUG_ON
#define SAFEMALLOC
#define PEDANTIC_SAFEMALLOC
#define SAFE_MUTEX
#endif

#define MYSQL_SERVER
#include <mysql_priv.h>
#include <mysql/plugin.h>
#ifdef OLD
#include <my_global.h>
#include <mysql_version.h>
#include <my_dir.h>
#endif

#if defined(HAVE_MALLINFO) && defined(HAVE_MALLOC_H)
#include <malloc.h>
#elif defined(HAVE_MALLINFO) && defined(HAVE_SYS_MALLOC_H)
#include <sys/malloc.h>
#endif


bool schema_table_store_record(THD *thd, TABLE *table);

ST_FIELD_INFO is_mallinfo_field_info[]=
{
    {"arena", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"ordblks", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"smblks", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"hblks", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"hblkhd", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"usmblks", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"fsmblks", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"uordblks", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"fordblks", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"keepcost", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"thread_count", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"thread_stack_size", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {"estimated_memory", MY_INT64_NUM_DECIMAL_DIGITS, MYSQL_TYPE_LONG, 0,0, NULL},
    {0, 0, MYSQL_TYPE_STRING, 0, 0, 0}
};

/********************************************************************
*  show_mallinfo()
*  code mostly from mysql_print_status() of sql/sql_test.cc
*********************************************************************/
void show_mallinfo() {
  fprintf(stderr, "Running threads: %d  Stack size: %ld\n",
        (int) thread_count, (long) my_thread_stack_size);

  struct mallinfo info= mallinfo();
  fprintf(stderr, "\nMemory status:\n\
Non-mmapped space allocated from system: %d\n\
Number of free chunks:           %d\n\
Number of fastbin blocks:        %d\n\
Number of mmapped regions:       %d\n\
Space in mmapped regions:        %d\n\
Maximum total allocated space:       %d\n\
Space available in freed fastbin blocks: %d\n\
Total allocated space:           %d\n\
Total free space:            %d\n\
Top-most, releasable space:      %d\n\
Estimated memory (with thread stack):    %ld\n",
     (int) info.arena   ,
     (int) info.ordblks,
     (int) info.smblks,
     (int) info.hblks,
     (int) info.hblkhd,
     (int) info.usmblks,
     (int) info.fsmblks,
     (int) info.uordblks,
     (int) info.fordblks,
     (int) info.keepcost,
     (long) (thread_count * my_thread_stack_size + info.hblkhd + info.arena));
}

static int fill_is_mallinfo_schema(THD *thd, TABLE_LIST *tables, COND *cond)
{
    DBUG_ENTER("fill_is_mallinfo_schema");
    CHARSET_INFO *scs= system_charset_info;
    TABLE *table= (TABLE *) tables->table;

    struct mallinfo info= mallinfo();

    table->field[0]->store(info.arena);
    table->field[1]->store(info.ordblks);
    table->field[2]->store(info.smblks);
    table->field[3]->store(info.hblks);
    table->field[4]->store(info.hblkhd);
    table->field[5]->store(info.usmblks);
    table->field[6]->store(info.fsmblks);
    table->field[7]->store(info.uordblks);
    table->field[8]->store(info.fordblks);
    table->field[9]->store(info.keepcost);
    table->field[10]->store(thread_count);
    table->field[11]->store(my_thread_stack_size);
    table->field[12]->store(thread_count * my_thread_stack_size + info.hblkhd + info.arena);

    schema_table_store_record(thd, table);
    DBUG_RETURN(0);
}

static int is_mallinfo_plugin_init(void *p)
{
    DBUG_ENTER("is_mallinfo_plugin_init");
    ST_SCHEMA_TABLE *schema= (ST_SCHEMA_TABLE *)p;
    schema->fields_info= is_mallinfo_field_info;
    schema->fill_table= fill_is_mallinfo_schema;
    DBUG_RETURN(0);
}


static int is_mallinfo_plugin_deinit(void *p)
{
    DBUG_ENTER("is_mallinfo_plugin_deinit");
    DBUG_RETURN(0);
}

struct st_mysql_information_schema is_mallinfo_plugin=
{ MYSQL_INFORMATION_SCHEMA_INTERFACE_VERSION };

/*
  Plugin library descriptor
*/

mysql_declare_plugin(is_mallinfo)
{
    MYSQL_INFORMATION_SCHEMA_PLUGIN,
    &is_mallinfo_plugin,
    "MALLINFO",
    "Yoichi Kawasaki",
    "malloc's mallinfo information schema interface.",
    PLUGIN_LICENSE_GPL,
    is_mallinfo_plugin_init, /* Plugin Init */
    is_mallinfo_plugin_deinit, /* Plugin Deinit */
    0x0010 /* 1.0 */,
    NULL,                       /* status variables                */
    NULL,                       /* system variables                */
    NULL                        /* config options                  */
}
mysql_declare_plugin_end;

