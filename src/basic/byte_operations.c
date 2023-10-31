
#include <ustd/common.h>


// -------------------------------------------------------------------------------------------------
void
bytewise_copy(void *dest, const void *source, size_t nb_bytes)
{
    u8 *byte_dest = (u8 *) dest;
    u8 *byte_source = (u8 *) source;
    u8 *byte_end = ((u8 *) source) + nb_bytes;

    while (byte_source != byte_end)
    {
        *(byte_dest++) = *(byte_source++);
    }
}

// -------------------------------------------------------------------------------------------------
u8
count_set_bits(u8 value)
{
    u8 counter = 0u;

    while (value)
    {
        counter += 1u;
        value &= (u8) (value - 1u);
    }

    return counter;
}

// -------------------------------------------------------------------------------------------------
u32
reverse_bit_representation(u32 x)
{
    x = (((x & 0xff00ff00) >> 8u) | ((x & 0x00ff00ff) << 8u));
    return ((x >> 16u) | (x << 16u));

}
