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
	static void fuzzIds(MCP2515 &mcp2515);

	/**
	 * Start ID for fuzzing (inclusive).
	 */
	static const canid_t startId;

	/**
	 * End ID for fuzzing (inclusive). Use CAN_SFF_MASK to fuzz all IDs.
	 */
	static const canid_t endId;

	/**
	 * List of CAN IDs to skip during fuzzing. Must be sorted in ascending order.
	 */
	static const canid_t skipIds[];

	/**
	 * sizeof(skipIds).
	 */
	static const size_t skipIdsSize;

private:
	static void fuzzId(MCP2515 &mcp2515, canid_t id);

	static const size_t skipIdsCount;
};
