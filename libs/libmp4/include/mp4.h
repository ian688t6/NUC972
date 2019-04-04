#ifndef __MP4_H__
#define __MP4_H__

#include "os.h"

#include "mp4_bits.h"

#undef assert
#define assert(a)

#define mp4_loge(fmt, arg...)		\
	do {	\
		printf("ERROR: %s %d ", __func__, __LINE__);	\
		printf(fmt"\n", ##arg);	\
	} while (0)

#define mp4_logi(fmt, arg...)		\
	do {	\
		printf("INFO: %s %d ", __func__, __LINE__);	\
		printf(fmt"\n", ##arg);	\
	} while (0)

#define mp4_logd(fmt, arg...)		\
	do {	\
		printf("DEBUG: %s %d ", __func__, __LINE__);	\
		printf(fmt"\n", ##arg);	\
	} while (0)

#include "box.h"
#include "stco_box.h"
#include "ftyp_box.h"
#include "free_box.h"
#include "mvhd_box.h"
#include "tkhd_box.h"

#include "mdhd_box.h"
#include "hdlr_box.h"

#include "vmhd_box.h"
#include "smhd_box.h"
#include "hmhd_box.h"
#include "nmhd_box.h"
#include "url_box.h"
#include "dref_box.h"
#include "dinf_box.h"

#include "stts_box.h"
#include "stss_box.h"
#include "ctts_box.h"
#include "stsc_box.h"
#include "stsz_box.h"
#include "stco_box.h"
#include "esds_box.h"
#include "mp4a_box.h"
#include "avcc_box.h"
#include "avc1_box.h"
#include "sowt_box.h"
#include "stsd_box.h"
#include "stbl_box.h"
#include "dinf_box.h"
#include "minf_box.h"

#include "mdia_box.h"

#include "trak_box.h"
#include "moov_box.h"
#include "mdat_box.h"

typedef enum {
	MP4_INVALID_AUDIO_TYPE,
	MP4_MPEG4_AUDIO_TYPE,
	MP4_PCM16_LITTLE_ENDIAN_AUDIO_TYPE,
} mp4_audio_type_e;

typedef struct tag_mp4 {
	mp4_audio_type_e audio_type;
	uint32_t audio_samplerate;
	uint32_t audio_channelcount;
	uint32_t bit_per_sample;

	ftyp_box_t *ftypbox;
	free_box_t *freebox;
	mdat_box_t *mdatbox;
	moov_box_t *moovbox;

	uint32_t struct_size;
	uint64_t position;

	mp4_bits_t *bs;

	uint32_t video_timescale;
	uint32_t audio_timescale;
	uint32_t video_frame_rate;
	uint32_t audio_sample_rate;
	uint32_t audio_sample_min_count;
	uint32_t video_sample_min_count;
	uint32_t video_key_sample_min_count;
	uint32_t audio_samples;
	uint32_t audio_sample_count;
	uint32_t video_sample_count;
	uint32_t video_key_sample_count;

	uint32_t *audio_stsz_entry_size;
	uint32_t *audio_stco_chunk_offset;
	uint32_t *audio_stsc_first_chunk;
	uint32_t *audio_stsc_samples_per_chunk;
	uint32_t *audio_stsc_sample_description_index;

	uint32_t *video_stsz_entry_size;
	uint32_t *video_stss_sample_number;
	uint32_t *video_stco_chunk_offset;
	uint32_t *video_stsc_first_chunk;
	uint32_t *video_stsc_samples_per_chunk;
	uint32_t *video_stsc_sample_description_index;
	struct timeval start_time;
	struct timeval end_time;
} mp4_t;

void mp4_destroy(mp4_t *mp4);
mp4_t *mp4_mux_create(uint8_t *filename);
int32_t mp4_mux_write_head(mp4_t *mp4);
int32_t mp4_mux_write_tail(mp4_t *mp4);
int32_t mp4_write_sample_video(mp4_t *mp4, uint8_t *data, uint32_t data_len, uint32_t iskey);
int32_t mp4_write_sample_audio(mp4_t *mp4, uint8_t *data, uint32_t data_len);
mp4_t *mp4_add_h264_video_track(mp4_t *mp4, uint32_t frameRate, uint32_t creatTime, uint32_t timeScale, uint16_t width, uint16_t height,
		uint8_t *h264buf, uint32_t h264len);

mp4_t *mp4_add_audio_track(mp4_t *mp4, uint32_t creatTime, uint16_t channel, uint32_t sample_rate, uint32_t samples, uint32_t bit_per_sample, mp4_audio_type_e audioType);

mp4_t *mp4_demux_create(uint8_t *filename);
int32_t mp4_get_track_h264_seq_pic_headers(mp4_t *mp4, uint8_t **pSeqHeader, uint16_t *pSeqHeaderSize, uint8_t **pPictHeader, uint16_t *pPictHeaderSize);

uint32_t mp4_get_key_sample_id(mp4_t *pst_mp4, uint32_t ui_sampleId);
int32_t mp4_is_key_sample(mp4_t *pst_mp4, uint32_t ui_sample_id);
int32_t mp4_read_video_sample(mp4_t *mp4, uint32_t sampleId, uint8_t **ppBytes, uint32_t *pNumBytes, int32_t *iskey, uint32_t *delta);
uint32_t mp4_get_video_time_scale(mp4_t *mp4);
uint32_t mp4_get_video_sample_total(mp4_t *mp4);

mp4_audio_type_e mp4_get_audio_type(mp4_t *mp4);
uint32_t mp4_get_audio_sample_rate(mp4_t *mp4);
uint32_t mp4_get_audio_channle_count(mp4_t *mp4);
uint32_t mp4_get_audio_bit_per_sample(mp4_t *mp4);
uint32_t mp4_get_audio_sample_total(mp4_t *mp4);
int32_t mp4_read_audio_sample(mp4_t *mp4, uint32_t sampleId, uint8_t **ppBytes, uint32_t *pNumBytes);

#endif
