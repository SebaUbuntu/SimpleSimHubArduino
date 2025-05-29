/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "CanFuzzer.h"

#include "Cluster.h"
#include "MessageDebouncer.h"
#include "StateHolder.h"

void CanFuzzer::fuzzIds(MCP2515 &mcp2515, uint16_t start, uint16_t end) {
	State &state = StateHolder::getState();

	for (uint16_t id = start; id < end; id++) {
		switch (id) {
			// Ignore known IDs
			case 0x036:
			case 0x0B6:
			case 0x0F6:
			case 0x128:
			case 0x161:
			case 0x168:
			case 0x1A1:
			case 0x1A8:
			case 0x3E7:
			case 0x928:
			case 0x961:
			case 0x968:
				break;
			default:
				fuzzId(mcp2515, id);
				Cluster::updateState(state);
				break;
		}
	}
}

void CanFuzzer::fuzzId(MCP2515 &mcp2515, uint16_t id) {
	struct can_frame frame;

	frame.can_id = id;
	frame.can_dlc = 8;
	frame.data[0] = 0xFF;
	frame.data[1] = 0xFF;
	frame.data[2] = 0xFF;
	frame.data[3] = 0xFF;
	frame.data[4] = 0xFF;
	frame.data[5] = 0xFF;
	frame.data[6] = 0xFF;
	frame.data[7] = 0xFF;

	Serial.println("Fuzzing ID " + String(id));
	mcp2515.sendMessage(&frame);

	delay(10);
}
