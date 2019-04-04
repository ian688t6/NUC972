#include "mp4.h"

int read_ctts_box(mp4_bits_t *bs, ctts_box_t *cttsbox)
{
	int i;

	cttsbox->struct_size = cttsbox->boxheader->struct_size;

	cttsbox->entry_count = mp4_bs_read_u32(bs);
	cttsbox->struct_size += 4;

	if (cttsbox->boxheader->version == 1) {
		cttsbox->sample_count = malloc(sizeof(uint64_t) * cttsbox->entry_count);
		if (cttsbox->sample_count == NULL) {
			mp4_loge("calloc Failed ");
			return 0;
		}
		cttsbox->sample_offset = malloc(sizeof(uint64_t) * cttsbox->entry_count);
		if (cttsbox->sample_offset == NULL) {
			mp4_loge("calloc Failed ");
			return 0;
		}

		uint64_t *sample_count;
		uint64_t *sample_offset;
		sample_count = (uint64_t *)cttsbox->sample_count;
		sample_offset = (uint64_t *)cttsbox->sample_offset;

		for (i = 0; i < cttsbox->entry_count; i++) {
			sample_count[i] = mp4_bs_read_u64(bs);
			sample_offset[i] = mp4_bs_read_u64(bs);
		}
		cttsbox->struct_size += cttsbox->entry_count * 16;
	} else {
		cttsbox->sample_count = malloc(sizeof(uint32_t) * cttsbox->entry_count);
		if (cttsbox->sample_count == NULL) {
			mp4_loge("calloc Failed ");
			return 0;
		}

		cttsbox->sample_offset = malloc(sizeof(uint32_t) * cttsbox->entry_count);
		if (cttsbox->sample_offset == NULL) {
			mp4_loge("calloc Failed ");
			return 0;
		}

		uint32_t *sample_count;
		uint32_t *sample_offset;
		sample_count = (uint32_t *)cttsbox->sample_count;
		sample_offset = (uint32_t *)cttsbox->sample_offset;
		for (i = 0; i < cttsbox->entry_count; i++) {
			sample_count[i] = mp4_bs_read_u32(bs);
			sample_offset[i] = mp4_bs_read_u32(bs);
		}
		cttsbox->struct_size += cttsbox->entry_count * 8;
	}

	return cttsbox->struct_size;
}

int write_ctts_box(mp4_bits_t *bs, ctts_box_t *cttsbox)
{

	uint64_t stcobox_pos, tail_pos;
	int i;

	stcobox_pos = mp4_bs_get_position(bs);

	cttsbox->struct_size = write_fullbox_header(bs, cttsbox->boxheader);

	mp4_bs_write_u32(bs, cttsbox->entry_count);
	cttsbox->struct_size += 4;

	if (cttsbox->boxheader->version == 1) {
		uint64_t *sample_count;
		uint64_t *sample_offset;
		sample_count = (uint64_t *)cttsbox->sample_count;
		sample_offset = (uint64_t *)cttsbox->sample_offset;

		for (i = 0; i < cttsbox->entry_count; i++) {
			mp4_bs_write_u64(bs, sample_count[i]);
			mp4_bs_write_u64(bs, sample_offset[i]);
		}
		cttsbox->struct_size += cttsbox->entry_count * 16;
	} else {
		uint32_t *sample_count;
		uint32_t *sample_offset;
		sample_count = (uint32_t *)cttsbox->sample_count;
		sample_offset = (uint32_t *)cttsbox->sample_offset;
		for (i = 0; i < cttsbox->entry_count; i++) {
			mp4_bs_write_u32(bs, sample_count[i]);
			mp4_bs_write_u32(bs, sample_offset[i]);
		}
		cttsbox->struct_size += cttsbox->entry_count * 8;
	}

	cttsbox->boxheader->size = cttsbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, stcobox_pos);
	mp4_bs_write_u32(bs, cttsbox->boxheader->size);

	mp4_bs_seek(bs, tail_pos);

	return cttsbox->struct_size;
}

int alloc_struct_ctts_box(ctts_box_t **cttsbox)
{
	ctts_box_t *cttsbox_t = *cttsbox;
	if ((cttsbox_t = (ctts_box_t *)calloc(1, sizeof(ctts_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(cttsbox_t->boxheader));
	*cttsbox = cttsbox_t;
	return 1;
}

int free_struct_ctts_box(ctts_box_t *cttsbox)
{
	if (cttsbox) {
		free_struct_fullbox_header(cttsbox->boxheader);
		if (cttsbox->sample_count) {
			free(cttsbox->sample_count);
			cttsbox->sample_count = NULL;
		}
		if (cttsbox->sample_offset) {
			free(cttsbox->sample_offset);
			cttsbox->sample_offset = NULL;
		}

		free(cttsbox);
		cttsbox = NULL;
	}
	return 1;
}
