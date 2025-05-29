/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <stdint.h>

/**
 * Message debouncer.
 */
class MessageDebouncer {
public:
	/**
	 * Constructor.
	 *
	 * @param delayMs The minimum debounce delay in milliseconds to wait before sending the next
	 *                message
	 */
	MessageDebouncer(uint16_t delayMs);

	/**
	 * Check if the message should be sent. If true, it updates the last poll time.
	 * 
	 * @return true if the message should be sent, false otherwise
	 */
	bool shouldUpdate();

private:
	uint16_t debounceDelayMs;
	bool sentAnyMessage = false;
	uint32_t lastPoll = 0;
};
