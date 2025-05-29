/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "types.h"

/**
 * Cluster.
 */
class Cluster {
public:
	/**
	 * Initialize the cluster.
	 */
	static void setup();

	/**
	 * Push the current state to the cluster.
	 * @param state The current state to be pushed.
	 */
	static void updateState(State &state);
};
