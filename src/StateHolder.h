/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "types.h"

class StateHolder {
public:
	static State &getState();

private:
	static State state;
};
