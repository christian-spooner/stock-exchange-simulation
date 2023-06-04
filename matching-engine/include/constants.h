#pragma once

/**
 * Maximum price of a limit order, in dollars.
 * This is one less than the maximum size of an unsigned short.
 */
#define MAX_PRICE 65534

/**
 * Minimum price of a limit order, in dollars.
 */
#define MIN_PRICE 00001

/**
 * Maximum number of uncrossed orders that may be
 * sitting on a given book at once.
 */
#define MAX_LIVE_ENTRIES 65535

#define BUY 0
