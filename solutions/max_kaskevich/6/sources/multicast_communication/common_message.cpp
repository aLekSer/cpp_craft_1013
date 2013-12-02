#include "common_message.h"
void multicast_communication::init_time_table()
{
    const char begin_char = 48;
    const char end_char = 107;
    for( char c = begin_char; c <= end_char; ++c )
    {
        time_table[ c ] = c - begin_char;
    }
}

uint32_t multicast_communication::get_seconds( const char data[3] )
{
    std::call_once( time_table_init_flag, init_time_table );
    return time_table[ data[ 0 ] ] * 3600 + 
            time_table[ data[ 1 ] ] * 60 + 
            time_table[ data[ 2 ] ];
}
