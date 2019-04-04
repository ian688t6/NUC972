#include "mp4.h"

int read_stco_box(mp4_bits_t *bs, stco_box_t *stcobox)
{
	int i;
	stcobox->struct_size = stcobox->boxheader->struct_size;

	stcobox->entry_count = mp4_bs_read_u32(bs);
	stcobox->struct_size += 4;

	stcobox->chunk_offset = (uint32_t *)malloc(sizeof(uint32_t) * stcobox->entry_count);

	for (i = 0; i < stcobox->entry_count; i++) {
		stcobox->chunk_offset[i] = mp4_bs_read_u32(bs);
	}
	stcobox->struct_size += stcobox->entry_count * 4;

	return stcobox->struct_size;
}

int write_stco_box(mp4_bits_t *bs, stco_box_t *stcobox)
{
	uint64_t stcobox_pos, tail_pos;
	int i;

	stcobox_pos = mp4_bs_get_position(bs);

	stcobox->struct_size = write_fullbox_header(bs, stcobox->boxheader);

	mp4_bs_write_u32(bs, stcobox->entry_count);
	stcobox->struct_size += 4;

	for (i = 0; i < stcobox->entry_count; i++) {
		mp4_bs_write_u32(bs, stcobox->chunk_offset[i]);
	}
	stcobox->struct_size += stcobox->entry_count * 4;

	stcobox->boxheader->size = stcobox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, stcobox_pos);
	mp4_bs_write_u32(bs, stcobox->boxheader->size);

	mp4_bs_seek(bs, tail_pos);

	return stcobox->struct_size;
}

int alloc_struct_stco_box(stco_box_t **stcobox)
{
	stco_box_t * stcobox_t = *stcobox;
	if ((stcobox_t = (stco_box_t *)calloc(1, sizeof(stco_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}

	alloc_struct_fullbox_header(&(stcobox_t->boxheader));

	*stcobox = stcobox_t;
	return 1;
}

int free_struct_stco_box(stco_box_t *stcobox)
{
	if (stcobox) {
		if (stcobox->boxheader) {
			free_struct_fullbox_header(stcobox->boxheader);
		}
		if (stcobox->chunk_offset) {
			free(stcobox->chunk_offset);
			stcobox->chunk_offset = NULL;
		}
		free(stcobox);
		stcobox = NULL;
	}
	return 1;
}
