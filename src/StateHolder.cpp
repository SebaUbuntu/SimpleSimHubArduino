/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "StateHolder.h"

State StateHolder::state;

State &StateHolder::getState() {
	return state;
}
