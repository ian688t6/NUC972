#ifndef __H264_H__
#define __H264_H__

#include "os.h"

typedef enum {
	NALU_TYPE_UNKNOWN	= 0,
	NALU_TYPE_SLICE		= 1,
	NALU_TYPE_SLICE_DPA	= 2,
	NALU_TYPE_SLICE_DPB	= 3,
	NALU_TYPE_SLICE_DPC	= 4,
	NALU_TYPE_SLICE_IDR	= 5,    /* ref_idc != 0 */
	NALU_TYPE_SEI		= 6,    /* ref_idc == 0 */
	NALU_TYPE_SPS		= 7,
	NALU_TYPE_PPS		= 8,
	NALU_TYPE_PD		= 9,
	NALU_TYPE_EOSEQ		= 10,
	NALU_TYPE_EOSTREAM	= 11,
	NALU_TYPE_FILL		= 12,
	NALU_TYPE_SEQEXT	= 13,
	/* ref_idc == 0 for 6,9,10,11,12 */
} nalu_unit_type_e;

typedef enum {
	NALU_PRIORITY_DISPOSABLE	= 0,
	NALU_PRIRITY_LOW		= 1,
	NALU_PRIORITY_HIGH		= 2,
	NALU_PRIORITY_HIGHEST		= 3,
} nalu_priority_e;

typedef enum {
	FRAME_I  = 15,
	FRAME_P  = 16,
	FRAME_B  = 17
} Frametype_e;

int h264_get_sps(uint8_t *buf, uint32_t len, uint8_t **sps, uint32_t *sps_len);
int h264_get_pps(uint8_t *buf, uint32_t len, uint8_t **pps, uint32_t *pps_len);
int h264_get_frame(uint8_t *buf, uint32_t len, uint8_t **frame, uint32_t *frame_len, uint32_t *slice, bool to_end);

#endif
