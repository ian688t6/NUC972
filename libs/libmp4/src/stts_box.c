#include "mp4.h"

int read_stts_box(mp4_bits_t *bs, stts_box_t *sttsbox)
{
	int i;

	sttsbox->struct_size = sttsbox->boxheader->struct_size;

	sttsbox->entry_count = mp4_bs_read_u32(bs);
	sttsbox->struct_size += 4;

	sttsbox->sample_count = (uint32_t *)malloc(sizeof(uint32_t) * sttsbox->entry_count);
	if (sttsbox->sample_count == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	sttsbox->sample_delta = (uint32_t *)malloc(sizeof(uint32_t) * sttsbox->entry_count);
	if (sttsbox->sample_delta == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}

	for (i = 0; i < sttsbox->entry_count; i++) {
		sttsbox->sample_count[i] = mp4_bs_read_u32(bs);
		sttsbox->sample_delta[i] = mp4_bs_read_u32(bs);
	}
	sttsbox->struct_size += sttsbox->entry_count * 8;

	return sttsbox->struct_size;
}

int write_stts_box(mp4_bits_t *bs, stts_box_t *sttsbox)
{
	uint64_t pos, tail_pos;
	int i;

	pos = mp4_bs_get_position(bs);

	sttsbox->struct_size = write_fullbox_header(bs, sttsbox->boxheader);

	mp4_bs_write_u32(bs, sttsbox->entry_count);
	sttsbox->struct_size += 4;

	for (i = 0; i < sttsbox->entry_count; i++) {
		mp4_bs_write_u32(bs, sttsbox->sample_count[i]);
		mp4_bs_write_u32(bs, sttsbox->sample_delta[i]);
	}
	sttsbox->struct_size += sttsbox->entry_count * 8;

	sttsbox->boxheader->size = sttsbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, sttsbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return sttsbox->struct_size;
}

int alloc_struct_stts_box(stts_box_t **sttsbox)
{
	stts_box_t *sttsbox_t = *sttsbox;
	if ((sttsbox_t = (stts_box_t *)calloc(1, sizeof(stts_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(sttsbox_t->boxheader));
	*sttsbox = sttsbox_t;
	return 1;
}

int free_struct_stts_box(stts_box_t *sttsbox)
{
	if (sttsbox) {
		free_struct_fullbox_header(sttsbox->boxheader);
		if (sttsbox->sample_count) {
			free(sttsbox->sample_count);
			sttsbox->sample_count = NULL;
		}
		if (sttsbox->sample_delta) {
			free(sttsbox->sample_delta);
			sttsbox->sample_delta = NULL;
		}

		free(sttsbox);
		sttsbox = NULL;
	}
	return 1;
}
