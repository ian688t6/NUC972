#include "mp4.h"

int read_stsz_box(mp4_bits_t *bs, stsz_box_t *stszbox)
{
	int i;

	stszbox->struct_size = stszbox->boxheader->struct_size;

	stszbox->sample_size = mp4_bs_read_u32(bs);
	stszbox->sample_count = mp4_bs_read_u32(bs);
	stszbox->struct_size += 8;

	if (stszbox->sample_size == 0) {
		stszbox->entry_size = (uint32_t *)malloc(sizeof(uint32_t) * stszbox->sample_count);
		if (!stszbox->entry_size) {
			mp4_loge("malloc Failed ");
			return 0;
		}

		for (i = 0; i < stszbox->sample_count; i++) {
			stszbox->entry_size[i] = mp4_bs_read_u32(bs);
		}
		stszbox->struct_size += stszbox->sample_count * 4;
	}

	return stszbox->struct_size;
}

int write_stsz_box(mp4_bits_t *bs, stsz_box_t *stszbox)
{
	uint64_t pos, tail_pos;
	int i;

	pos = mp4_bs_get_position(bs);

	stszbox->struct_size = write_fullbox_header(bs, stszbox->boxheader);

	mp4_bs_write_u32(bs, stszbox->sample_size);
	mp4_bs_write_u32(bs, stszbox->sample_count);
	stszbox->struct_size += 8;

	if (stszbox->sample_size == 0) {
		for (i = 0; i < stszbox->sample_count; i++) {
			mp4_bs_write_u32(bs, stszbox->entry_size[i]);
		}
		stszbox->struct_size += stszbox->sample_count * 4;
	}

	stszbox->boxheader->size = stszbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, stszbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return stszbox->struct_size;
}

int alloc_struct_stsz_box(stsz_box_t **stszbox)
{
	stsz_box_t *stszbox_t = *stszbox;
	if ((stszbox_t = (stsz_box_t *)calloc(1, sizeof(stsz_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_fullbox_header(&(stszbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}

	*stszbox = stszbox_t;
	return 1;
}

int free_struct_stsz_box(stsz_box_t * stszbox)
{
	if (stszbox) {
		if (stszbox->boxheader) {
			free_struct_fullbox_header(stszbox->boxheader);
		}
		if (stszbox->entry_size) {
			free(stszbox->entry_size);
			stszbox->entry_size = NULL;
		}

		free(stszbox);
		stszbox = NULL;
	}
	return 1;
}
