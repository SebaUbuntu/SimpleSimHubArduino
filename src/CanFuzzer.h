/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <mcp2515.h>
#include <stdint.h>

/**
 * Utilities for finding CAN IDs.
 */
class CanFuzzer {
public:
	static void fuzzIds(MCP2515 &mcp2515, uint16_t start, uint16_t end);
	static void fuzzId(MCP2515 &mcp2515, uint16_t id);
};
