/*
 * SPDX-FileCopyrightText: Sebastiano Barezzi
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "CanFuzzer.h"

#include "MessageDebouncer.h"

const size_t CanFuzzer::skipIdsCount = CanFuzzer::skipIdsSize / sizeof(canid_t);

void CanFuzzer::fuzzIds(MCP2515 &mcp2515) {
	static MessageDebouncer messageDebouncer(100);
	static canid_t lastId = CanFuzzer::startId;
	static size_t skipIndex = 0;

	if (!messageDebouncer.shouldUpdate()) {
		return;
	}

	while (true) {
		lastId++;

		if (lastId > CanFuzzer::endId) {
			lastId = CanFuzzer::startId;
			skipIndex = 0;
		}

		while (skipIndex < CanFuzzer::skipIdsCount && lastId > CanFuzzer::skipIds[skipIndex]) {
			skipIndex++;
		}

		if (lastId != CanFuzzer::skipIds[skipIndex]) {
			break;
		} else {
			Serial.println("Skipping ID 0x" + String(lastId, 16));
		}
	}

	fuzzId(mcp2515, lastId);
}

void CanFuzzer::fuzzId(MCP2515 &mcp2515, canid_t id) {
	struct can_frame frame;

	frame.can_id = id;
	frame.can_dlc = 8;
	frame.data[0] = 0x00;
	frame.data[1] = 0x00;
	frame.data[2] = 0x00;
	frame.data[3] = 0x00;
	frame.data[4] = 0x00;
	frame.data[5] = 0x00;
	frame.data[6] = 0x00;
	frame.data[7] = 0x00;

	Serial.println("Fuzzing ID 0x" + String(id, 16));
	mcp2515.sendMessage(&frame);

	delay(10);
}
