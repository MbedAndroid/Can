class DecodeOOK
{
	protected:
	int total_bits, bits, flip, state, pos;
	char data[32];

	virtual int decode (int width) =0;

	public:

	enum { UNKNOWN, T0, OK, DONE };

	DecodeOOK () {
		resetDecoder();
	}

	bool nextPulse (int width) {
		if (state != DONE) {
			switch (decode(width)) {
				case -1:
				resetDecoder();
				break;
				case 1:
				done();
				break;
			}
		}

		return isDone();
	}

	bool isDone () const {
		return state == DONE;
	}

	const char* getData (int& count) const {
		count = pos;
		return data;
	}

	void resetDecoder () {
		total_bits = bits = pos = flip = 0;
		state = UNKNOWN;
	}

	// add one bit to the packet data buffer

	virtual void gotBit (int value) {
		total_bits++;
		char *ptr = data + pos;
		*ptr = (*ptr >> 1) | (value << 7);

		if (++bits >= 8) {
			bits = 0;
			if (++pos >= sizeof data) {
				resetDecoder();
				return;
			}
		}
		state = OK;
	}

	// store a bit using Manchester encoding
	void manchester (char value) {
		flip ^= value; // manchester code, long pulse flips the bit
		gotBit(flip);
	}

	void done () {
		while (bits)
		gotBit(0); // padding
		state = DONE;
	}
};


// 433 MHz decoders


class OregonDecoderV2 : public DecodeOOK
{
	public:
	OregonDecoderV2() {}

	// add one bit to the packet data buffer
	virtual void gotBit (int value) {
		if (!(total_bits & 0x01)) {
			data[pos] = (data[pos] >> 1) | (value ? 0x80 : 00);
		}
		total_bits++;
		pos = total_bits >> 4;
		if (pos >= sizeof data) {
			resetDecoder();
			return;
		}
		state = OK;
	}

	virtual int decode (int width) {
		if (200 <= width && width < 1200) {
			bool w = width >= 700;
			switch (state) {
				case UNKNOWN:
				if (w) {
					// Long pulse
					++flip;
					} else if (32 <= flip) {
					// Short pulse, start bit
					flip = 0;
					state = T0;
					} else {
					// Reset decoder
					return -1;
				}
				break;
				case OK:
				if (!w) {
					// Short pulse
					state = T0;
					} else {
					// Long pulse
					manchester(1);
				}
				break;
				case T0:
				if (!w) {
					// Second short pulse
					manchester(0);
					} else {
					// Reset decoder
					return -1;
				}
				break;
			}
			} else {
			return -1;
		}
		return total_bits == 160 ? 1: 0;
	}
};