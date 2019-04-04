#include "mp4.h"

int read_stss_box(mp4_bits_t *bs, stss_box_t *stssbox)
{
	int i;

	stssbox->struct_size = stssbox->boxheader->struct_size;

	stssbox->entry_count = mp4_bs_read_u32(bs);
	stssbox->struct_size += 4;

	if (stssbox->entry_count == 0)
		return stssbox->struct_size;

	stssbox->sample_number = (uint32_t *)malloc(sizeof(uint32_t) * stssbox->entry_count);
	if (stssbox->sample_number == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}

	for (i = 0; i < stssbox->entry_count; i++) {
		stssbox->sample_number[i] = mp4_bs_read_u32(bs);
	}
	stssbox->struct_size += stssbox->entry_count * 4;

	return stssbox->struct_size;
}

int write_stss_box(mp4_bits_t *bs, stss_box_t *stssbox)
{
	uint64_t pos, tail_pos;
	int i;

	pos = mp4_bs_get_position(bs);

	stssbox->struct_size = write_fullbox_header(bs, stssbox->boxheader);

	mp4_bs_write_u32(bs, stssbox->entry_count);
	stssbox->struct_size += 4;

	for (i = 0; i < stssbox->entry_count; i++) {
		mp4_bs_write_u32(bs, stssbox->sample_number[i]);
	}
	stssbox->struct_size += stssbox->entry_count * 4;

	stssbox->boxheader->size = stssbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, stssbox->boxheader->size);

	mp4_bs_seek(bs, tail_pos);

	return stssbox->struct_size;
}

int alloc_struct_stss_box(stss_box_t **stssbox)
{
	stss_box_t *stssbox_t = *stssbox;
	if ((stssbox_t = (stss_box_t *)calloc(1, sizeof(stss_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(stssbox_t->boxheader));
	*stssbox = stssbox_t;
	return 1;
}

int free_struct_stss_box(stss_box_t *stssbox)
{
	if (stssbox) {
		free_struct_fullbox_header(stssbox->boxheader);
		if (stssbox->sample_number) {
			free(stssbox->sample_number);
			stssbox->sample_number = NULL;
		}

		free(stssbox);
		stssbox = NULL;
	}
	return 1;
}
