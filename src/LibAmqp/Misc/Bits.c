/*
   Copyright 2011 StormMQ Limited

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#include "Misc/Bits.h"

// Taken from http://graphics.stanford.edu/~seander/bithacks.html
const uint8_t amqp_mod_37_bit_position[] = // map a bit value mod 37 to its position
{
    32, 0, 1, 26, 2, 23, 27, 0, 3, 16, 24, 30, 28, 11, 0, 13, 4,
    7, 17, 0, 25, 22, 31, 15, 29, 10, 12, 6, 0, 21, 14, 9, 5,
    20, 8, 19, 18
};

const uint8_t amqp_mod_67_bit_position[] = // map a bit value mod 67 to its position
{
    0, 0, 1, 39, 2, 15, 40, 23, 3, 12, 16, 59, 41, 19, 24, 54,
    4, 0, 13, 10, 17, 62, 60, 28, 42, 30, 20, 51, 25, 44, 55, 47,
    5, 32, 0, 38, 14, 22, 11, 58, 18, 53, 63, 9, 61, 27, 29, 50,
    43, 46, 31, 37, 21, 57, 52, 8, 26, 49, 45, 36, 56, 7, 48, 35,
    6, 34, 33
};


uint32_t amqp_next_power_two_32(uint32_t n)
{
    n = n - 1;
    n = n | (n >> 1);
    n = n | (n >> 2);
    n = n | (n >> 4);
    n = n | (n >> 8);
    n = n | (n >> 16);
    return n + 1;
}

uint64_t amqp_next_power_two_64(uint64_t n)
{
    n = n - 1;
    n = n | (n >> 1);
    n = n | (n >> 2);
    n = n | (n >> 4);
    n = n | (n >> 8);
    n = n | (n >> 16);
    n = n | (n >> 32);
    return n + 1;
}

