
#include <errno.h>
#include <fcntl.h>
#include "mp4.h"
#include "h264.h"

#define MP4_DEFAULT_TIME	3 /* minute */

static const int32_t matrix[9] = {0x00010000, 0, 0, 0, 0x00010000, 0, 0, 0, 0x40000000};

static int alloc_struct_mp4(mp4_t **mp4)
{
	mp4_t *mp4_tmp = *mp4;

	if ((mp4_tmp = (mp4_t *)calloc(1, sizeof(mp4_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}

	*mp4 = mp4_tmp;
	return 1;
}

static int free_struct_mp4(mp4_t *mp4)
{
	if (mp4) {
		free(mp4);
		mp4 = NULL;
	}
	return 1;
}

int32_t mp4_write_sample_audio(mp4_t *mp4, uint8_t *data, uint32_t data_len)
{
	uint64_t pos;

	if (mp4->audio_sample_count >= mp4->audio_sample_min_count) {
		mp4->audio_sample_min_count *= 2;
		mp4->audio_stco_chunk_offset = realloc(mp4->audio_stco_chunk_offset, mp4->audio_sample_min_count * sizeof(uint32_t));
		mp4->audio_stsc_first_chunk = realloc(mp4->audio_stsc_first_chunk, mp4->audio_sample_min_count * sizeof(uint32_t));
		mp4->audio_stsc_samples_per_chunk = realloc(mp4->audio_stsc_samples_per_chunk, mp4->audio_sample_min_count * sizeof(uint32_t));
		mp4->audio_stsc_sample_description_index = realloc(mp4->audio_stsc_sample_description_index, mp4->audio_sample_min_count * sizeof(uint32_t));
	}

	pos = mp4->position;

	mp4->audio_stco_chunk_offset[mp4->audio_sample_count] = pos;
	mp4->audio_stsc_first_chunk[mp4->audio_sample_count] = mp4->audio_sample_count + 1;
	mp4->audio_stsc_samples_per_chunk[mp4->audio_sample_count] = data_len / (mp4->bit_per_sample >> 3);
	mp4->audio_stsc_sample_description_index[mp4->audio_sample_count] = 0x01;
	mp4->audio_sample_count++;

	write_mdat_box_data(mp4->bs, mp4->mdatbox, data, data_len);
	mp4->position += data_len;

	return 0;
}

int32_t mp4_write_sample_video(mp4_t *mp4, uint8_t *data, uint32_t data_len, uint32_t iskey)
{
	int ret;
	uint64_t pos;
	uint8_t *framebuf;
	uint32_t framelen;
	uint32_t slice = 0;

	ret = h264_get_frame(data, data_len, &framebuf, &framelen, &slice, false);
	if (ret) {
		mp4_loge("h264_get_frame failed\n");
		return -1;
	}
	framebuf[0] = (uint8_t)((framelen - 4) >> 24);
	framebuf[1] = (uint8_t)((framelen - 4) >> 16);
	framebuf[2] = (uint8_t)((framelen - 4) >> 8);
	framebuf[3] = (uint8_t)(framelen - 4);

	if (mp4->video_sample_count >= mp4->video_sample_min_count) {
		mp4->video_sample_min_count *= 2;
		mp4->video_stsz_entry_size = realloc(mp4->video_stsz_entry_size, mp4->video_sample_min_count * sizeof(uint32_t));
		mp4->video_stco_chunk_offset = realloc(mp4->video_stco_chunk_offset, mp4->video_sample_min_count * sizeof(uint32_t));
		mp4->video_stsc_first_chunk = realloc(mp4->video_stsc_first_chunk, mp4->video_sample_min_count * sizeof(uint32_t));
		mp4->video_stsc_samples_per_chunk = realloc(mp4->video_stsc_samples_per_chunk, mp4->video_sample_min_count * sizeof(uint32_t));
		mp4->video_stsc_sample_description_index = realloc(mp4->video_stsc_sample_description_index, mp4->video_sample_min_count * sizeof(uint32_t));
	}
	if (mp4->video_key_sample_count >= mp4->video_key_sample_min_count) {
		mp4->video_key_sample_min_count *= 2;
		mp4->video_stss_sample_number = realloc(mp4->video_stss_sample_number, mp4->video_key_sample_min_count * sizeof(uint32_t));
	}

	pos = mp4->position;

	mp4->video_stsz_entry_size[mp4->video_sample_count] = framelen;
	mp4->video_stco_chunk_offset[mp4->video_sample_count] = pos;
	mp4->video_stsc_first_chunk[mp4->video_sample_count] = mp4->video_sample_count + 1;
	mp4->video_stsc_samples_per_chunk[mp4->video_sample_count] = 0x01;
	mp4->video_stsc_sample_description_index[mp4->video_sample_count] = 0x01;

	if (iskey) {
		mp4->video_stss_sample_number[mp4->video_key_sample_count] = mp4->video_sample_count + 1;
		mp4->video_key_sample_count++;
	}
	mp4->video_sample_count++;

	write_mdat_box_data(mp4->bs, mp4->mdatbox, framebuf, framelen);
	mp4->position += framelen;

	return 0;
}

static void mp4_get_audio_info(mp4_t *mp4)
{
	mp4->audio_type = MP4_INVALID_AUDIO_TYPE;
	if (mp4->moovbox) {
		if (mp4->moovbox->soun_trakbox) {
			if (mp4->moovbox->soun_trakbox->mdiabox) {
				if (mp4->moovbox->soun_trakbox->mdiabox->minfbox) {
					if (mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox) {
						if (mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox) {
							if (mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox) {
								mp4->audio_type = MP4_MPEG4_AUDIO_TYPE;
								mp4->bit_per_sample = mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->samplesize;
								mp4->audio_samplerate = mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->samplerate;
								mp4->audio_channelcount = mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->chanlecount;
							} else if (mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox) {
								mp4->audio_type = MP4_PCM16_LITTLE_ENDIAN_AUDIO_TYPE;
								mp4->bit_per_sample = mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->sample_size;
								mp4->audio_samplerate = mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->sample_rate;
								mp4->audio_channelcount = mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->channel_num;
							}
						}
					}
				}
			}
		}
	}
}

mp4_t *mp4_demux_create(uint8_t *filename)
{
	int ret;
	box_header_t *boxhead;
#ifdef USE_FILE_IO_INTERFACE
	int fd;
#else
	FILE *fp;
#endif
	mp4_t *mp4;

#ifdef USE_FILE_IO_INTERFACE
	fd = open((const char *)filename, O_RDONLY);
	if (fd < 0) {
		mp4_loge("open %s failed %d\n", filename, errno);
		return NULL;
	}
#else
	fp = fopen((const char *)filename, "rb");
	if (!fp) {
		mp4_loge("fopen %s failed %d\n", filename, errno);
		return NULL;
	}
#endif

	alloc_struct_mp4(&mp4);

#ifdef USE_FILE_IO_INTERFACE
	mp4->bs = mp4_bs_create_from_file(fd, MP4_BITS_READ);
#else
	mp4->bs = mp4_bs_create_from_file(fp, MP4_BITS_READ);
#endif
	if (!mp4->bs || mp4->bs->size == 0) {
		mp4_loge("mp4_bs_create_from_file failed");
		free_struct_mp4(mp4);
#ifdef USE_FILE_IO_INTERFACE
		close(fd);
#else
		fclose(fp);
#endif
		return NULL;
	}

	alloc_struct_box_header(&boxhead);

	mp4->struct_size = 0;

	while (mp4->struct_size < mp4->bs->size) {
		ret = read_box_header(mp4->bs, boxhead);
		if (ret == 0) {
			free_struct_mp4(mp4);
#ifdef USE_FILE_IO_INTERFACE
			close(fd);
#else
			fclose(fp);
#endif
			return NULL;
		}

		if (boxhead->type == MP4_BOX_TYPE_ftyp) {
			ftyp_box_t *ftypbox;

			alloc_struct_ftyp_box(&ftypbox);
			memcpy(ftypbox->boxheader, boxhead, sizeof(box_header_t));
			mp4->struct_size += read_ftyp_box(mp4->bs, ftypbox);
			mp4->ftypbox = ftypbox;
		} else if (boxhead->type == MP4_BOX_TYPE_free) {
			free_box_t *freebox;
			alloc_struct_free_box(&freebox);
			memcpy(freebox->boxheader, boxhead, sizeof(box_header_t));
			mp4->struct_size += read_free_box(mp4->bs, freebox);
			mp4->freebox = freebox;
		} else if (boxhead->type == MP4_BOX_TYPE_moov) {
			moov_box_t *moovbox;
			alloc_struct_moov_box(&moovbox);
			memcpy(moovbox->boxheader, boxhead, sizeof(box_header_t));
			mp4->struct_size += read_moov_box(mp4->bs, moovbox);
			mp4->moovbox = moovbox;
		} else {
			mp4_bs_skip_bytes(mp4->bs, boxhead->size - boxhead->struct_size);
			mp4->struct_size += boxhead->size;
		}
	}

	free_struct_box_header(boxhead);

	mp4_get_audio_info(mp4);

	return mp4;
}

mp4_t *mp4_add_audio_track(mp4_t *mp4, uint32_t creatTime, uint16_t channel, uint32_t sample_rate, uint32_t samples, uint32_t bit_per_sample, mp4_audio_type_e audioType)
{
	char audio_name[] = " SoundHandler";
	audio_name[0] = 0xc;

	mp4->audio_timescale = sample_rate;
	mp4->audio_samples = samples;
	mp4->bit_per_sample = bit_per_sample;

	/* set default time count */
	mp4->audio_sample_min_count = sample_rate * (MP4_DEFAULT_TIME * 60) / samples;
	mp4->audio_stco_chunk_offset = malloc(mp4->audio_sample_min_count * sizeof(uint32_t));
	mp4->audio_stsc_first_chunk = malloc(mp4->audio_sample_min_count * sizeof(uint32_t));
	mp4->audio_stsc_samples_per_chunk = malloc(mp4->audio_sample_min_count * sizeof(uint32_t));
	mp4->audio_stsc_sample_description_index = malloc(mp4->audio_sample_min_count * sizeof(uint32_t));
	if (!mp4->audio_stco_chunk_offset || !mp4->audio_stsc_first_chunk ||
			!mp4->audio_stsc_samples_per_chunk || !mp4->audio_stsc_sample_description_index) {
		mp4_loge("malloc Failed ");
		return mp4;
	}

	if (mp4->moovbox->soun_trakbox == NULL) {
		alloc_struct_trak_box(&mp4->moovbox->soun_trakbox);
		mp4->moovbox->soun_trakbox->boxheader->type = MP4_BOX_TYPE_trak;

		alloc_struct_tkhd_box(&mp4->moovbox->soun_trakbox->tkhdbox);
		mp4->moovbox->soun_trakbox->tkhdbox->boxheader->type = MP4_BOX_TYPE_tkhd;
		mp4->moovbox->soun_trakbox->tkhdbox->boxheader->flags = 0x1;
		mp4->moovbox->soun_trakbox->tkhdbox->creation_time = creatTime;
		mp4->moovbox->soun_trakbox->tkhdbox->modification_time = creatTime;
		mp4->moovbox->soun_trakbox->tkhdbox->track_ID = 0x2;
//		mp4->moovbox->soun_trakbox->tkhdbox->duration = sampleDuration; // ?????
		mp4->moovbox->soun_trakbox->tkhdbox->volume = 0x0100;
		memcpy(mp4->moovbox->soun_trakbox->tkhdbox->matrix, matrix, 36);

		alloc_struct_mdia_box(&mp4->moovbox->soun_trakbox->mdiabox);
		mp4->moovbox->soun_trakbox->mdiabox->boxheader->type = MP4_BOX_TYPE_mdia;

		alloc_struct_mdhd_box(&mp4->moovbox->soun_trakbox->mdiabox->mdhdbox);
		mp4->moovbox->soun_trakbox->mdiabox->mdhdbox->boxheader->type = MP4_BOX_TYPE_mdhd;
		mp4->moovbox->soun_trakbox->mdiabox->mdhdbox->creation_time = creatTime;
		mp4->moovbox->soun_trakbox->mdiabox->mdhdbox->modification_time = creatTime;
		mp4->moovbox->soun_trakbox->mdiabox->mdhdbox->timescale = mp4->audio_timescale;
//		mp4->moovbox->soun_trakbox->mdiabox->mdhdbox->duration = sampleDuration;  // ?????
		mp4->moovbox->soun_trakbox->mdiabox->mdhdbox->language = 0x15c7;

		alloc_struct_hdlr_box(&mp4->moovbox->soun_trakbox->mdiabox->hdlrbox);
		mp4->moovbox->soun_trakbox->mdiabox->hdlrbox->boxheader->type = MP4_BOX_TYPE_hdlr;
		mp4->moovbox->soun_trakbox->mdiabox->hdlrbox->pre_defined = MP4_FOURCC('m', 'h', 'l', 'r');
		mp4->moovbox->soun_trakbox->mdiabox->hdlrbox->handler_type = MP4_FOURCC('s', 'o', 'u', 'n');
		mp4->moovbox->soun_trakbox->mdiabox->hdlrbox->name = malloc(sizeof(audio_name));
		memcpy(mp4->moovbox->soun_trakbox->mdiabox->hdlrbox->name, audio_name, sizeof(audio_name));
		mp4->moovbox->soun_trakbox->mdiabox->hdlrbox->name_len = strlen(audio_name);

		alloc_struct_minf_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->boxheader->type = MP4_BOX_TYPE_minf;

		alloc_struct_smhd_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->smhdbox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->smhdbox->boxheader->type = MP4_BOX_TYPE_smhd;

		alloc_struct_dinf_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->dinfbox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->dinfbox->boxheader->type = MP4_BOX_TYPE_dinf;

		alloc_struct_dref_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->dinfbox->drefbox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->dinfbox->drefbox->boxheader->type = MP4_BOX_TYPE_dref;
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->dinfbox->drefbox->entry_count = 1;

		alloc_struct_url_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->dinfbox->drefbox->urlbox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->dinfbox->drefbox->urlbox->boxheader->type = MP4_BOX_TYPE_url;
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->dinfbox->drefbox->urlbox->boxheader->flags = 1;

		alloc_struct_stbl_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->boxheader->type = MP4_BOX_TYPE_stbl;

		alloc_struct_stsd_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->boxheader->type = MP4_BOX_TYPE_stsd;
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->entry_count = 1;

		if (audioType == MP4_MPEG4_AUDIO_TYPE) {
			alloc_struct_mp4a_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox);
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->boxheader->type = MP4_BOX_TYPE_mp4a;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->data_reference_index = 0x01;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->chanlecount = channel;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->samplesize = bit_per_sample;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->samplerate = mp4->audio_timescale;

			alloc_struct_esds_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox);
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->boxheader->type = MP4_BOX_TYPE_esds;

			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->tag = 0x03;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->Length_Field = 0x19;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->DecoderConfigDescriptor_tag = 0x04;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->Length_Field_1 = 0x11;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->objectTypeIndication = 0x40;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->streamType = 5;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->reserved = 1;
//			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->bufferSizeDB = ????; //0x000600
//			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->maxBitrate = ????
//			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->avgBitrate = ????
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->DecSpecificInfotag = 0x05;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->Length_Field_2 = 0x02;
//			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->audioObjectType_2_GASpecificConfig = ????;
//			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->samplingFrequencyIndex = ????;
//			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->channelConfiguration  = ?????
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->SLConfigDescrTag = 0x06;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->Length_Field_3 = 0x01;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox->predefined = 0x02;
		} else if (audioType == MP4_PCM16_LITTLE_ENDIAN_AUDIO_TYPE) {
			alloc_struct_sowt_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox);
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->boxheader->type = MP4_BOX_TYPE_sowt;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->data_reference_index = 0x01;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->version = 0;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->revision_level = 0;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->vendor = 0;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->channel_num = channel;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->sample_size = bit_per_sample;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->compression_id = 0;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->packet_size = 0;
			mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox->sample_rate = sample_rate;
		}

		alloc_struct_stts_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->sttsbox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->sttsbox->boxheader->type = MP4_BOX_TYPE_stts;
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->sttsbox->entry_count = 1;
//		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_count = ????
//		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_delta =  ????

		alloc_struct_stsc_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->boxheader->type = MP4_BOX_TYPE_stsc;
//		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->entry_count = ?????
//		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->first_chunk = ?????
//		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->samples_per_chunk = ????
//		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->sample_description_index = ????

		alloc_struct_stsz_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stszbox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stszbox->boxheader->type = MP4_BOX_TYPE_stsz;
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stszbox->sample_size = 0;
//		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stszbox->sample_count = ????;		//sample taltal
//		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stszbox->entry_size = ???;

		alloc_struct_stco_box(&mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stcobox);
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stcobox->boxheader->type = MP4_BOX_TYPE_stco;
//		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stcobox->entry_count = ????
//		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stcobox->chunk_offset = ???
	}

	return mp4;
}

mp4_t *mp4_add_h264_video_track(mp4_t *mp4, uint32_t frameRate, uint32_t creatTime, uint32_t timeScale, uint16_t width, uint16_t height,
		uint8_t *h264buf, uint32_t h264len)
{
	int ret;
	uint8_t *p;
	uint8_t *spsbuf = NULL;
	uint8_t *ppsbuf = NULL;
	uint32_t len, off;
	uint32_t spslen = 0;
	uint32_t ppslen = 0;
	char vide_name[] = " VideoHandler";
	char avc1_compressorname[32] = " AVC Coding";
	vide_name[0] = 0xc;
	avc1_compressorname[0] = 10;

	mp4->video_timescale = timeScale;
	//mp4->video_frame_rate = frameRate;
	mp4->moovbox->mvhdbox->creation_time = creatTime;
	mp4->moovbox->mvhdbox->modification_time = creatTime;

	/* set default time + 2s, h264 gop size 16 */
	mp4->video_sample_min_count = frameRate * (MP4_DEFAULT_TIME * 60 + 2);
	mp4->video_key_sample_min_count = mp4->video_sample_min_count / 16;
	mp4->video_stss_sample_number = malloc(mp4->video_key_sample_min_count * sizeof(uint32_t));
	mp4->video_stsz_entry_size = malloc(mp4->video_sample_min_count * sizeof(uint32_t));
	mp4->video_stco_chunk_offset = malloc(mp4->video_sample_min_count * sizeof(uint32_t));
	mp4->video_stsc_first_chunk = malloc(mp4->video_sample_min_count * sizeof(uint32_t));
	mp4->video_stsc_samples_per_chunk = malloc(mp4->video_sample_min_count * sizeof(uint32_t));
	mp4->video_stsc_sample_description_index = malloc(mp4->video_sample_min_count * sizeof(uint32_t));
	if (!mp4->video_stss_sample_number || !mp4->video_stsz_entry_size ||
			!mp4->video_stco_chunk_offset || !mp4->video_stsc_first_chunk ||
			!mp4->video_stsc_samples_per_chunk || !mp4->video_stsc_sample_description_index) {
		mp4_loge("malloc Failed ");
		return mp4;
	}

	if (mp4->moovbox->vide_trakbox == NULL) {
		alloc_struct_trak_box(&mp4->moovbox->vide_trakbox);
		mp4->moovbox->vide_trakbox->boxheader->type = MP4_BOX_TYPE_trak;

		alloc_struct_tkhd_box(&mp4->moovbox->vide_trakbox->tkhdbox);
		mp4->moovbox->vide_trakbox->tkhdbox->boxheader->type = MP4_BOX_TYPE_tkhd;
		mp4->moovbox->vide_trakbox->tkhdbox->boxheader->flags = 0x1;

		mp4->moovbox->vide_trakbox->tkhdbox->creation_time = creatTime;
		mp4->moovbox->vide_trakbox->tkhdbox->modification_time = creatTime;
		mp4->moovbox->vide_trakbox->tkhdbox->track_ID = 0x01;
//		mp4->moovbox->vide_trakbox->tkhdbox->duration = sampleDuration;
//		mp4->moovbox->vide_trakbox->tkhdbox->layer = 0;

		memcpy(mp4->moovbox->vide_trakbox->tkhdbox->matrix, matrix, 36);
		mp4->moovbox->vide_trakbox->tkhdbox->width = width;
		mp4->moovbox->vide_trakbox->tkhdbox->height = height;

		alloc_struct_mdia_box(&mp4->moovbox->vide_trakbox->mdiabox);
		mp4->moovbox->vide_trakbox->mdiabox->boxheader->type = MP4_BOX_TYPE_mdia;

		alloc_struct_mdhd_box(&mp4->moovbox->vide_trakbox->mdiabox->mdhdbox);
		mp4->moovbox->vide_trakbox->mdiabox->mdhdbox->boxheader->type = MP4_BOX_TYPE_mdhd;
		mp4->moovbox->vide_trakbox->mdiabox->mdhdbox->creation_time = creatTime;
		mp4->moovbox->vide_trakbox->mdiabox->mdhdbox->modification_time = creatTime;
		mp4->moovbox->vide_trakbox->mdiabox->mdhdbox->timescale = timeScale;		// ???????
//		mp4->moovbox->vide_trakbox->mdiabox->mdhdbox->duration = sampleDuration;   // ????????
		mp4->moovbox->vide_trakbox->mdiabox->mdhdbox->language = 0x15c7;

		alloc_struct_hdlr_box(&mp4->moovbox->vide_trakbox->mdiabox->hdlrbox);
		mp4->moovbox->vide_trakbox->mdiabox->hdlrbox->boxheader->type = MP4_BOX_TYPE_hdlr;
		mp4->moovbox->vide_trakbox->mdiabox->hdlrbox->pre_defined = MP4_FOURCC('m', 'h', 'l', 'r');
		mp4->moovbox->vide_trakbox->mdiabox->hdlrbox->handler_type = MP4_FOURCC('v', 'i', 'd', 'e');
		mp4->moovbox->vide_trakbox->mdiabox->hdlrbox->name = malloc(sizeof(vide_name));
		memcpy(mp4->moovbox->vide_trakbox->mdiabox->hdlrbox->name, vide_name, sizeof(vide_name));
		mp4->moovbox->vide_trakbox->mdiabox->hdlrbox->name_len = strlen(vide_name);

		alloc_struct_minf_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->boxheader->type = MP4_BOX_TYPE_minf;

		alloc_struct_vmhd_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->vmhdbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->vmhdbox->boxheader->type = MP4_BOX_TYPE_vmhd;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->vmhdbox->boxheader->flags = 1;

		alloc_struct_dinf_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->dinfbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->dinfbox->boxheader->type = MP4_BOX_TYPE_dinf;

		alloc_struct_dref_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->dinfbox->drefbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->dinfbox->drefbox->boxheader->type = MP4_BOX_TYPE_dref;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->dinfbox->drefbox->entry_count = 1;

		alloc_struct_url_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->dinfbox->drefbox->urlbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->dinfbox->drefbox->urlbox->boxheader->type = MP4_BOX_TYPE_url;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->dinfbox->drefbox->urlbox->boxheader->flags = 1;

		alloc_struct_stbl_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->boxheader->type = MP4_BOX_TYPE_stbl;

		alloc_struct_stsd_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->boxheader->type = MP4_BOX_TYPE_stsd;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->entry_count = 1;

		alloc_struct_avc1_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->boxheader->type = MP4_BOX_TYPE_avc1;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->data_reference_index = 0x01;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->width = width;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->height = height;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->horizresolution = 0x00480000;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->vertresolution = 0x00480000;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->frame_count = 1;
		memcpy(mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->compressorname, avc1_compressorname, 32);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->depth = 0x0018;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->pre_defined2 = 0xffff;

		p = h264buf;
		len = h264len;
		ret = h264_get_sps(p, len, &spsbuf, &spslen);
		if (ret) {
			mp4_loge("h264_get_sps failed\n");
			return mp4;
		}
		off = spsbuf - p + spslen;
		p += off;
		len -= off;
		ret = h264_get_pps(p, len, &ppsbuf, &ppslen);
		if (ret) {
			mp4_loge("h264_get_pps failed\n");
			return mp4;
		}

		spsbuf += 4;
		ppsbuf += 4;
		spslen -= 4;
		ppslen -= 4;

		if (!spslen || !ppslen) {
			mp4_loge("spslen = %d, ppslen = %d\n", spslen, ppslen);
			return mp4;
		}

		alloc_struct_avcc_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->boxheader->type = MP4_BOX_TYPE_avcC;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->configurationVersion = 1;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->AVCProfileIndication = spsbuf[1];
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->profile_compatibility = spsbuf[2];
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->AVCLevelIndication = spsbuf[3];
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->lengthSizeMinusOne = 0xff;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->numOfSequenceParameterSets = 1;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->sequenceParameterSetLength = spslen;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->sequenceParameterSetNALUnit = malloc(spslen);
		memcpy(mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->sequenceParameterSetNALUnit, spsbuf, spslen);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->numOfPictureParameterSets = 1;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->pictureParameterSetLength = ppslen;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->pictureParameterSetNALUnit = malloc(ppslen);
		memcpy(mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->pictureParameterSetNALUnit, ppsbuf, ppslen);

		alloc_struct_stts_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->boxheader->type = MP4_BOX_TYPE_stts;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->entry_count = 1;
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_count = ????
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_delta =  ????

		alloc_struct_ctts_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->cttsbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->cttsbox->boxheader->type = MP4_BOX_TYPE_ctts;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->cttsbox->entry_count = 1;
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->cttsbox->sample_count = ?????
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->cttsbox->sample_offset = ????
		alloc_struct_stsc_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stscbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stscbox->boxheader->type = MP4_BOX_TYPE_stsc;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stscbox->entry_count = 1;
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stscbox->first_chunk = ?????
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stscbox->samples_per_chunk = ????
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stscbox->sample_description_index = ????

		alloc_struct_stsz_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->boxheader->type = MP4_BOX_TYPE_stsz;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->sample_size = 0;
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->sample_count = ????;		//sample taltal
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->entry_size = ???;

		alloc_struct_stco_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stcobox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stcobox->boxheader->type = MP4_BOX_TYPE_stco;
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stcobox->entry_count = ????
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stcobox->chunk_offset = ???
		alloc_struct_stss_box(&mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stssbox);
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stssbox->boxheader->type = MP4_BOX_TYPE_stss;
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stssbox->entry_count = ????
//		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stssbox->sample_number = ????
	}

	return mp4;
}

static void free_trak_box_sons(trak_box_t *trakbox)
{
	if (trakbox->mdiabox) {
		if (trakbox->mdiabox->minfbox) {
			if (trakbox->mdiabox->minfbox->vmhdbox)
				free_struct_vmhd_box(trakbox->mdiabox->minfbox->vmhdbox);
			if (trakbox->mdiabox->minfbox->smhdbox)
				free_struct_smhd_box(trakbox->mdiabox->minfbox->smhdbox);
			if (trakbox->mdiabox->minfbox->nmhdbox)
				free_struct_nmhd_box(trakbox->mdiabox->minfbox->nmhdbox);
			if (trakbox->mdiabox->minfbox->hmhdbox)
				free_struct_hmhd_box(trakbox->mdiabox->minfbox->hmhdbox);
			if (trakbox->mdiabox->minfbox->dinfbox) {
				if (trakbox->mdiabox->minfbox->dinfbox->drefbox) {
					if (trakbox->mdiabox->minfbox->dinfbox->drefbox->urlbox)
						free_struct_url_box(trakbox->mdiabox->minfbox->dinfbox->drefbox->urlbox);
					free_struct_dref_box(trakbox->mdiabox->minfbox->dinfbox->drefbox);
				}
				free_struct_dinf_box(trakbox->mdiabox->minfbox->dinfbox);
			}
			if (trakbox->mdiabox->minfbox->stblbox) {
				if (trakbox->mdiabox->minfbox->stblbox->cttsbox)
					free_struct_ctts_box(trakbox->mdiabox->minfbox->stblbox->cttsbox);
				if (trakbox->mdiabox->minfbox->stblbox->sttsbox)
					free_struct_stts_box(trakbox->mdiabox->minfbox->stblbox->sttsbox);
				if (trakbox->mdiabox->minfbox->stblbox->stssbox)
					free_struct_stss_box(trakbox->mdiabox->minfbox->stblbox->stssbox);
				if (trakbox->mdiabox->minfbox->stblbox->stscbox)
					free_struct_stsc_box(trakbox->mdiabox->minfbox->stblbox->stscbox);
				if (trakbox->mdiabox->minfbox->stblbox->stszbox)
					free_struct_stsz_box(trakbox->mdiabox->minfbox->stblbox->stszbox);
				if (trakbox->mdiabox->minfbox->stblbox->stcobox)
					free_struct_stco_box(trakbox->mdiabox->minfbox->stblbox->stcobox);
				if (trakbox->mdiabox->minfbox->stblbox->stsdbox) {
					if (trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box) {
						if (trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox)
							free_struct_avcc_box(trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox);
						free_struct_avc1_box(trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box);
					}
					if (trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox) {
						if (trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox)
							free_struct_esds_box(trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox->esdsbox);
						free_struct_mp4a_box(trakbox->mdiabox->minfbox->stblbox->stsdbox->mp4abox);
					}
					if (trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox)
						free_struct_sowt_box(trakbox->mdiabox->minfbox->stblbox->stsdbox->sowtbox);
					free_struct_stsd_box(trakbox->mdiabox->minfbox->stblbox->stsdbox);
				}
				free_struct_stbl_box(trakbox->mdiabox->minfbox->stblbox);
			}
			free_struct_Minf_box(trakbox->mdiabox->minfbox);
		}
		if (trakbox->mdiabox->hdlrbox)
			free_struct_hdlr_box(trakbox->mdiabox->hdlrbox);
		if (trakbox->mdiabox->mdhdbox)
			free_struct_mdhd_box(trakbox->mdiabox->mdhdbox);
		free_struct_mdia_box(trakbox->mdiabox);
	}
	if (trakbox->tkhdbox)
		free_struct_tkhd_box(trakbox->tkhdbox);
}

void mp4_destroy(mp4_t *mp4)
{
	if (!mp4)
		return;

#ifdef USE_FILE_IO_INTERFACE
	close(mp4->bs->fd);
#else
	fflush(mp4->bs->stream);
	fclose(mp4->bs->stream);
#endif

	if (mp4->moovbox) {
		if (mp4->moovbox->mvhdbox)
			free_struct_mvhd_box(mp4->moovbox->mvhdbox);
		if (mp4->moovbox->vide_trakbox) {
			free_trak_box_sons(mp4->moovbox->vide_trakbox);
			free_struct_trak_box(mp4->moovbox->vide_trakbox);
		}
		if (mp4->moovbox->soun_trakbox) {
			free_trak_box_sons(mp4->moovbox->soun_trakbox);
			free_struct_trak_box(mp4->moovbox->soun_trakbox);
		}
		free_struct_moov_box(mp4->moovbox);
	}

	free_struct_mdat_box(mp4->mdatbox);
	free_struct_ftyp_box(mp4->ftypbox);
	free_struct_free_box(mp4->freebox);
	mp4_bs_destroy(mp4->bs);

	free_struct_mp4(mp4);
}

mp4_t *mp4_mux_create(uint8_t *filename)
{
	mp4_t *mp4;
#ifdef USE_FILE_IO_INTERFACE
	int fd;
#else
	FILE *fp;
#endif
	uint32_t *compatible_brands;

#ifdef USE_FILE_IO_INTERFACE
	fd = open((const char *)filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		mp4_loge("open %s failed %d\n", filename, errno);
		return NULL;
	}
#else
	fp = fopen((const char *)filename, "wb+");
	if (!fp) {
		mp4_loge("fopen %s failed %d\n", filename, errno);
		return NULL;
	}
#endif

	if (!alloc_struct_mp4(&mp4)) {
#ifdef USE_FILE_IO_INTERFACE
		close(fd);
#else
		fclose(fp);
#endif
		return NULL;
	}

	gettimeofday(&mp4->start_time, NULL);

#ifdef USE_FILE_IO_INTERFACE
	mp4->bs = mp4_bs_create(NULL, 0, MP4_BITS_WRITE);
	mp4->bs->fd = fd;
	//mp4->bs = mp4_bs_create_from_file(fd, MP4_BITS_WRITE);
#else
	mp4->bs = mp4_bs_create(NULL, 0, MP4_BITS_WRITE);
	mp4->bs->stream = fp;
	mp4->bs->fd = fileno(fp);
	//mp4->bs = mp4_bs_create_from_file(fp, MP4_BITS_WRITE);
#endif
	if (!mp4->bs) {
		mp4_loge("mp4_bs_create_from_file failed");
		free_struct_mp4(mp4);
#ifdef USE_FILE_IO_INTERFACE
		close(fd);
#else
		fclose(fp);
#endif
		return NULL;
	}

	alloc_struct_free_box(&mp4->freebox);
	mp4->freebox->boxheader->type = MP4_BOX_TYPE_free;

	alloc_struct_ftyp_box(&mp4->ftypbox);
	mp4->ftypbox->boxheader->type = MP4_BOX_TYPE_ftyp;
	mp4->ftypbox->major_brand = MP4_FOURCC('m', 'p', '4', '2');
	mp4->ftypbox->brands = 2;
	mp4->ftypbox->compatible_brands = (uint32_t *)calloc(mp4->ftypbox->brands, sizeof(uint32_t));
	compatible_brands = mp4->ftypbox->compatible_brands;
	compatible_brands[0] = MP4_FOURCC('m', 'p', '4', '2');
	compatible_brands[1] = MP4_FOURCC('i', 's', 'o', 'm');

	alloc_struct_mdat_box(&mp4->mdatbox);
	mp4->mdatbox->boxheader->type = MP4_BOX_TYPE_mdat;

	alloc_struct_moov_box(&mp4->moovbox);
	mp4->moovbox->boxheader->type = MP4_BOX_TYPE_moov;

	alloc_struct_mvhd_box(&mp4->moovbox->mvhdbox);
	mp4->moovbox->mvhdbox->boxheader->type = MP4_BOX_TYPE_mvhd;
//	mp4->moovbox->mvhdbox->creation_time = creattime;
//	mp4->moovbox->mvhdbox->modification_time = creattime;
//	mp4->moovbox->mvhdbox->timescale = timescale;		// 90000
//	mp4->moovbox->mvhdbox->duration = duration; 	???????????	// sample * (timescale / framerate)
	mp4->moovbox->mvhdbox->rate = 0x00010000;
	mp4->moovbox->mvhdbox->volume = 0x0100;
	memcpy(mp4->moovbox->mvhdbox->matrix, matrix, 36);
	mp4->moovbox->mvhdbox->next_track_ID = 0x03;

	return mp4;
}

int32_t mp4_mux_write_head(mp4_t *mp4)
{
	char *head_buf = NULL;
	uint32_t head_len = 0;
	mp4_bits_t *bs = mp4->bs;

	mp4->struct_size = 0;

	if (mp4->ftypbox) {
		mp4->struct_size += write_ftyp_box(bs, mp4->ftypbox);
	}

	if (mp4->freebox) {
		mp4->struct_size += write_free_box(bs, mp4->freebox);
	}

	if (mp4->mdatbox) {
		write_mdat_box_head(bs, mp4->mdatbox);
	}

	mp4_bs_obtain_content(bs, &head_buf, &head_len);
	if (head_buf && head_len)
		write(bs->fd, head_buf, head_len);
	mp4->position = (uint64_t)head_len;

	return 0;
}

#define MP4_CALC_FACTOR		1000000
int32_t mp4_mux_write_tail(mp4_t *mp4)
{
	uint32_t duration;
	uint32_t sample_delta;
	uint64_t us, tmp;
	char *tail_buf = NULL;
	uint32_t tail_len = 0;
	mp4_bits_t *bs = mp4->bs;

	gettimeofday(&mp4->end_time, NULL);
	us = (mp4->end_time.tv_sec * 1000000 + mp4->end_time.tv_usec) - (mp4->start_time.tv_sec * 1000000 + mp4->start_time.tv_usec);

	if (mp4->mdatbox) {
		mp4->struct_size += write_mdat_box_tail(bs, mp4->mdatbox);
	}

	if (mp4->moovbox->vide_trakbox) {
		tmp = (uint64_t)mp4->video_sample_count * MP4_CALC_FACTOR;
		mp4->video_frame_rate = (uint32_t)(tmp * 1000000 / us);
		mp4_logi("mp4->video_frame_rate * %d = %d\n", MP4_CALC_FACTOR, mp4->video_frame_rate);
		sample_delta = (uint32_t)(((uint64_t)mp4->video_timescale * MP4_CALC_FACTOR + mp4->video_frame_rate - 1) / mp4->video_frame_rate);
		duration = sample_delta * mp4->video_sample_count;

		mp4->moovbox->vide_trakbox->tkhdbox->duration = duration;

		mp4->moovbox->mvhdbox->timescale = mp4->video_timescale;
		mp4->moovbox->mvhdbox->duration = duration;

		mp4->moovbox->vide_trakbox->mdiabox->mdhdbox->timescale = mp4->video_timescale;
		mp4->moovbox->vide_trakbox->mdiabox->mdhdbox->duration = duration;

		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_count = malloc(sizeof(uint32_t));
		memcpy(mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_count, &mp4->video_sample_count, sizeof(uint32_t));
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_delta = malloc(sizeof(uint32_t));
		memcpy(mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_delta, &sample_delta, sizeof(uint32_t));

		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->cttsbox->sample_count = malloc(sizeof(uint32_t));
		memcpy(mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->cttsbox->sample_count, &mp4->video_sample_count, sizeof(uint32_t));
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->cttsbox->sample_offset = malloc(sizeof(uint32_t));
		memcpy(mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->cttsbox->sample_offset, &sample_delta, sizeof(uint32_t));

		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stscbox->first_chunk = mp4->video_stsc_first_chunk;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stscbox->samples_per_chunk = mp4->video_stsc_samples_per_chunk;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stscbox->sample_description_index = mp4->video_stsc_sample_description_index;

		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->sample_count = mp4->video_sample_count;		//sample taltal
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->entry_size = mp4->video_stsz_entry_size;

		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stcobox->entry_count = mp4->video_sample_count;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stcobox->chunk_offset = mp4->video_stco_chunk_offset;

		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stssbox->entry_count = mp4->video_key_sample_count;
		mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stssbox->sample_number = mp4->video_stss_sample_number;
	}

	if (mp4->moovbox->soun_trakbox) {
		sample_delta = mp4->audio_samples;
		duration = sample_delta * mp4->audio_sample_count;
		mp4->moovbox->soun_trakbox->tkhdbox->duration = duration;
		mp4->moovbox->soun_trakbox->mdiabox->mdhdbox->duration = duration;
		mp4->moovbox->soun_trakbox->mdiabox->mdhdbox->timescale = mp4->audio_timescale;

		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_count = malloc(sizeof(uint32_t));
		memcpy(mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_count, &duration, sizeof(uint32_t));
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_delta = malloc(sizeof(uint32_t));
		*mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_delta = 1;

		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->first_chunk = mp4->audio_stsc_first_chunk;
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->samples_per_chunk = mp4->audio_stsc_samples_per_chunk;
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->sample_description_index = mp4->audio_stsc_sample_description_index;
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->entry_count = mp4->audio_sample_count;

		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stszbox->sample_size = 1;
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stszbox->sample_count = mp4->audio_sample_count * mp4->audio_samples;

		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stcobox->entry_count = mp4->audio_sample_count;
		mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stcobox->chunk_offset = mp4->audio_stco_chunk_offset;
	}

	if (mp4->moovbox) {
		mp4->struct_size += write_moov_box(bs, mp4->moovbox);
	}

	mp4_bs_obtain_content(bs, &tail_buf, &tail_len);
	if (tail_buf && tail_len)
		write(bs->fd, tail_buf, tail_len);

	return 0;
}

int32_t mp4_get_track_h264_seq_pic_headers(mp4_t *mp4, uint8_t **pSeqHeader, uint16_t *pSeqHeaderSize, uint8_t **pPictHeader, uint16_t *pPictHeaderSize)
{
	if (mp4) {
		if (mp4->moovbox) {
			if (mp4->moovbox->vide_trakbox) {
				if (mp4->moovbox->vide_trakbox->mdiabox) {
					if (mp4->moovbox->vide_trakbox->mdiabox->minfbox) {
						if (mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox) {
							if (mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox) {
								if (mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box) {
									if (mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox) {
										*pSeqHeader =
										                mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->sequenceParameterSetNALUnit;
										*pSeqHeaderSize =
										                mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->sequenceParameterSetLength;
										*pPictHeader =
										                mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->pictureParameterSetNALUnit;
										*pPictHeaderSize =
										                mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stsdbox->avc1box->avccbox->pictureParameterSetLength;
										return 0;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return 1;
}

uint32_t mp4_get_video_sample_total(mp4_t *mp4)
{
	if (mp4) {
		if (mp4->moovbox) {
			if (mp4->moovbox->vide_trakbox) {
				if (mp4->moovbox->vide_trakbox->mdiabox) {
					if (mp4->moovbox->vide_trakbox->mdiabox->minfbox) {
						if (mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox) {
							if (mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox) {
								return mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->sample_count;
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

uint32_t mp4_get_video_time_scale(mp4_t *mp4)
{
	if (mp4) {
		if (mp4->moovbox) {
			if (mp4->moovbox->vide_trakbox) {
				if (mp4->moovbox->vide_trakbox->mdiabox) {
					if (mp4->moovbox->vide_trakbox->mdiabox->mdhdbox) {
//						mp4->moovbox->vide_trakbox->mdiabox->mdhdbox->duration;
						return mp4->moovbox->vide_trakbox->mdiabox->mdhdbox->timescale;
					}
				}
			}
		}
	}
	return 0;
}

int32_t mp4_is_key_sample(mp4_t *pst_mp4, uint32_t ui_sample_id)
{
	uint32_t	ui_key_sample_id = 0;
	int32_t 	i_index, i_lindex, i_rindex;

	if (!pst_mp4->video_stss_sample_number || !pst_mp4->video_key_sample_count) {
		return 0;
	}

	i_lindex = 0;
	i_rindex = pst_mp4->video_key_sample_count - 1;

	while (i_lindex <= i_rindex) {
		i_index = (i_lindex + i_rindex) >> 1;
		ui_key_sample_id = pst_mp4->video_key_sample_count - 1;
		if (ui_sample_id == ui_key_sample_id) {
			return 1;
		}

		if (ui_sample_id > ui_key_sample_id) {
			i_lindex = i_index + 1;		
		} else {
			i_rindex = i_index - 1;
		}
	}

	return 0;
}

uint32_t mp4_get_key_sample_id(mp4_t *pst_mp4, uint32_t ui_sampleId)
{
	uint32_t ui_key_sampleId;
	int32_t index, lindex, rindex;

	if (!pst_mp4->video_stss_sample_number || !pst_mp4->video_key_sample_count)
		return ui_sampleId;

	lindex = 0;
	rindex = pst_mp4->video_key_sample_count - 1;

	while (lindex <= rindex) {
		index = (lindex + rindex) >> 1;
		ui_key_sampleId = pst_mp4->video_stss_sample_number[index] - 1;

		if (ui_sampleId == ui_key_sampleId)
			return ui_sampleId;

		if (ui_sampleId > ui_key_sampleId)
			lindex = index + 1;
		else
			rindex = index - 1;
	}

	if (ui_sampleId - pst_mp4->video_stss_sample_number[rindex] < pst_mp4->video_stss_sample_number[lindex] - ui_sampleId)
		return pst_mp4->video_stss_sample_number[rindex] - 1;
	return pst_mp4->video_stss_sample_number[lindex] - 1;
}

int32_t mp4_read_video_sample(mp4_t *mp4, uint32_t sampleId, uint8_t **ppBytes, uint32_t *pNumBytes, int32_t *iskey, uint32_t *delta)
{
#if !defined(USE_FILE_IO_INTERFACE) && !defined(ONLY_READ_WRITE_USE_FILE_IO)
	uint64_t pos;
#endif
	uint32_t offset, size;
	uint8_t *pbytes;

	if (!mp4->moovbox || !mp4->moovbox->vide_trakbox || !mp4->moovbox->vide_trakbox->mdiabox || !mp4->moovbox->vide_trakbox->mdiabox->minfbox
	                || !mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox || !mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stcobox
	                || !mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stcobox->chunk_offset
	                || !mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox
	                || !mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->entry_size
	                || !mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox
	                || !mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_delta) {
#ifdef USE_FILE_IO_INTERFACE
		close(mp4->bs->fd);
#else
		fclose(mp4->bs->stream);
#endif
		return 0;
	}

	offset = mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stcobox->chunk_offset[sampleId];
	if (mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->sample_size) {
		size = mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->sample_size;
	} else {
		size = mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->stszbox->entry_size[sampleId];
	}

	if (mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_delta) {
		*delta = *mp4->moovbox->vide_trakbox->mdiabox->minfbox->stblbox->sttsbox->sample_delta;
	}

	pbytes = malloc(size);
	if (!pbytes) {
		mp4_loge("malloc Failed size = %d", size);
		return 0;
	}

#if defined(USE_FILE_IO_INTERFACE) || defined(ONLY_READ_WRITE_USE_FILE_IO)
	lseek(mp4->bs->fd, offset, SEEK_SET);
	read(mp4->bs->fd, pbytes, size);
	lseek(mp4->bs->fd, 0, SEEK_SET);
#else
	pos = ftell(mp4->bs->stream);

	fseek(mp4->bs->stream, offset, SEEK_SET);
	fread(pbytes, size, 1, mp4->bs->stream);
	fseek(mp4->bs->stream, pos, SEEK_SET);
#endif

	*iskey = ((pbytes[4] == 0x65) ? 1 : 0);
	*ppBytes = pbytes;
	*pNumBytes = size;

	return 1;
}

mp4_audio_type_e mp4_get_audio_type(mp4_t *mp4)
{
	if (!mp4)
		return MP4_INVALID_AUDIO_TYPE;
	return mp4->audio_type;
}

uint32_t mp4_get_audio_sample_rate(mp4_t *mp4)
{
	if (!mp4)
		return 0;
	return mp4->audio_samplerate;
}

uint32_t mp4_get_audio_channle_count(mp4_t *mp4)
{
	if (!mp4)
		return 0;
	return mp4->audio_channelcount;
}

uint32_t mp4_get_audio_bit_per_sample(mp4_t *mp4)
{
	if (!mp4)
		return 0;
	return mp4->bit_per_sample;
}

uint32_t mp4_get_audio_sample_total(mp4_t *mp4)
{
	if (!mp4)
		return 0;

	if (mp4->moovbox) {
		if (mp4->moovbox->soun_trakbox) {
			if (mp4->moovbox->soun_trakbox->mdiabox) {
				if (mp4->moovbox->soun_trakbox->mdiabox->minfbox) {
					if (mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox) {
						if (mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox) {
							return mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->entry_count;
						}
					}
				}
			}
		}
	}

	return 0;
}

int32_t mp4_read_audio_sample(mp4_t *mp4, uint32_t sampleId, uint8_t **ppBytes, uint32_t *pNumBytes)
{
#if !defined(USE_FILE_IO_INTERFACE) && !defined(ONLY_READ_WRITE_USE_FILE_IO)
	uint64_t pos;
#endif
	uint32_t offset, size;
	uint8_t *pbytes;

	if (!mp4->moovbox || !mp4->moovbox->soun_trakbox || !mp4->moovbox->soun_trakbox->mdiabox || !mp4->moovbox->soun_trakbox->mdiabox->minfbox
	                || !mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox || !mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stcobox
	                || !mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stcobox->chunk_offset
	                || !mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox
	                || !mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->samples_per_chunk) {
#ifdef USE_FILE_IO_INTERFACE
		close(mp4->bs->fd);
#else
		fclose(mp4->bs->stream);
#endif
		return 0;
	}

	offset = mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stcobox->chunk_offset[sampleId];
	size = mp4->moovbox->soun_trakbox->mdiabox->minfbox->stblbox->stscbox->samples_per_chunk[sampleId] * (mp4->bit_per_sample >> 3);

	pbytes = malloc(size);
	if (!pbytes) {
		mp4_loge("malloc Failed size = %d", size);
		return 0;
	}

#if defined(USE_FILE_IO_INTERFACE) || defined(ONLY_READ_WRITE_USE_FILE_IO)
	lseek(mp4->bs->fd, offset, SEEK_SET);
	read(mp4->bs->fd, pbytes, size);
	lseek(mp4->bs->fd, 0, SEEK_SET);
#else
	pos = ftell(mp4->bs->stream);

	fseek(mp4->bs->stream, offset, SEEK_SET);
	fread(pbytes, size, 1, mp4->bs->stream);
	fseek(mp4->bs->stream, pos, SEEK_SET);
#endif

	*ppBytes = pbytes;
	*pNumBytes = size;

	return 1;
}
