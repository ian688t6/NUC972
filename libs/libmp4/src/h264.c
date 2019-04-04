#include "h264.h"

static bool h264_find_nalu(uint8_t *buf, uint32_t len, uint8_t **nalu, uint32_t *nalu_len, bool to_end)
{
	bool find = false;
	uint32_t l = len;

	if (l < 4)
		return 0;

	while (l > 3) {
		if ((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0) && (buf[3] == 1)) {
			if (!find) {
				*nalu = buf;
				find = true;
				if (!to_end)
					break;
			} else {
				break;
			}
		}
		buf++;
		l--;
	}

	if (find) {
		if ((!to_end) || (l < 4))
			*nalu_len = len;
		else
			*nalu_len = buf - *nalu;
	}

	return find;
}

/* if found return 0, else return -1 */
int h264_get_sps(uint8_t *buf, uint32_t len, uint8_t **sps, uint32_t *sps_len)
{
	bool find = false;
	uint8_t *nalubuf;
	uint32_t nalulen;

	while (1) {
		find = h264_find_nalu(buf, len, &nalubuf, &nalulen, true);
		if (find) {
			if ((nalubuf[4] & 0x1f) == NALU_TYPE_SPS) {
				*sps = nalubuf;
				*sps_len = nalulen;
				return 0;
			} else {
				buf += nalulen;
				len -= nalulen;
			}
		} else {
			return -1;
		}
	}

	return -1;
}

/* if found return 0, else return -1 */
int h264_get_pps(uint8_t *buf, uint32_t len, uint8_t **pps, uint32_t *pps_len)
{
	bool find = false;
	uint8_t *nalubuf;
	uint32_t nalulen;

	while (1) {
		find = h264_find_nalu(buf, len, &nalubuf, &nalulen, true);
		if (find) {
			if ((nalubuf[4] & 0x1f) == NALU_TYPE_PPS) {
				*pps = nalubuf;
				*pps_len = nalulen;
				return 0;
			} else {
				buf += nalulen;
				len -= nalulen;
			}
		} else {
			return -1;
		}
	}

	return -1;
}

/* if found return 0, else return -1 */
int h264_get_frame(uint8_t *buf, uint32_t len, uint8_t **frame, uint32_t *frame_len, uint32_t *slice, bool to_end)
{
	bool find = false;
	uint32_t l = len;

	if (l < 4)
		return -1;

	while (l > 3) {
		if (((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0) && (buf[3] == 1))
				&& (((buf[4] & 0x1f) == NALU_TYPE_SLICE) || ((buf[4] & 0x1f) == NALU_TYPE_SLICE_IDR))) {
			if (!find) {
				*frame = buf;
				*slice = buf[4] & 0x1f;
				find = true;
				if (!to_end)
					break;
			} else {
				break;
			}
		}
		buf++;
		l--;
	}

	if (find) {
		if ((!to_end) || (l < 4))
			*frame_len = len;
		else
			*frame_len = buf - *frame;

		return 0;
	}

	return -1;
}
