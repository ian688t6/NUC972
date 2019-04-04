#include "mp4.h"

int read_stsc_box(mp4_bits_t *bs, stsc_box_t *stscbox)
{
	int i;

	stscbox->struct_size = stscbox->boxheader->struct_size;

	stscbox->entry_count = mp4_bs_read_u32(bs);
	stscbox->struct_size += 4;

	stscbox->first_chunk = (uint32_t *)malloc(sizeof(uint32_t) * stscbox->entry_count);
	if (!stscbox->first_chunk) {
		mp4_loge("malloc Failed ");
		return 0;
	}
	stscbox->samples_per_chunk = (uint32_t *)malloc(sizeof(uint32_t) * stscbox->entry_count);
	if (!stscbox->samples_per_chunk) {
		mp4_loge("malloc Failed ");
		return 0;
	}
	stscbox->sample_description_index = (uint32_t *)malloc(sizeof(uint32_t) * stscbox->entry_count);
	if (!stscbox->sample_description_index) {
		mp4_loge("malloc Failed ");
		return 0;
	}

	for (i = 0; i < stscbox->entry_count; i++) {
		stscbox->first_chunk[i] = mp4_bs_read_u32(bs);
		stscbox->samples_per_chunk[i] = mp4_bs_read_u32(bs);
		stscbox->sample_description_index[i] = mp4_bs_read_u32(bs);
	}
	stscbox->struct_size += stscbox->entry_count * 12;

	return stscbox->struct_size;
}

int write_stsc_box(mp4_bits_t *bs, stsc_box_t *stscbox)
{
	uint64_t pos, tail_pos;
	int i;

	pos = mp4_bs_get_position(bs);

	stscbox->struct_size = write_fullbox_header(bs, stscbox->boxheader);

	mp4_bs_write_u32(bs, stscbox->entry_count);
	stscbox->struct_size += 4;

	for (i = 0; i < stscbox->entry_count; i++) {
		mp4_bs_write_u32(bs, stscbox->first_chunk[i]);
		mp4_bs_write_u32(bs, stscbox->samples_per_chunk[i]);
		mp4_bs_write_u32(bs, stscbox->sample_description_index[i]);
	}
	stscbox->struct_size += stscbox->entry_count * 12;

	stscbox->boxheader->size = stscbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, stscbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return stscbox->struct_size;
}

int alloc_struct_stsc_box(stsc_box_t **stscbox)
{
	stsc_box_t * stscbox_t = *stscbox;
	if ((stscbox_t = (stsc_box_t *)calloc(1, sizeof(stsc_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_fullbox_header(&(stscbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}

	*stscbox = stscbox_t;
	return 1;
}

int free_struct_stsc_box(stsc_box_t * stscbox)
{
	if (stscbox) {
		if (stscbox->boxheader) {
			free_struct_fullbox_header(stscbox->boxheader);
		}

		if (stscbox->first_chunk) {
			free(stscbox->first_chunk);
			stscbox->first_chunk = NULL;
		}
		if (stscbox->samples_per_chunk) {
			free(stscbox->samples_per_chunk);
			stscbox->samples_per_chunk = NULL;
		}
		if (stscbox->sample_description_index) {
			free(stscbox->sample_description_index);
			stscbox->sample_description_index = NULL;
		}

		free(stscbox);
		stscbox = NULL;
	}
	return 1;
}
