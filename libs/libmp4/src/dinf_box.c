#include "mp4.h"

int read_dinf_box(mp4_bits_t *bs, dinf_box_t *dinfbox)
{
	fullbox_header_t *boxhead;
	dref_box_t *drefbox;

	dinfbox->struct_size = dinfbox->boxheader->struct_size;

	alloc_struct_fullbox_header(&boxhead);
	read_fullbox_header(bs, boxhead);

	if (boxhead->type == MP4_BOX_TYPE_dref) {
		alloc_struct_dref_box(&drefbox);
		memcpy(drefbox->boxheader, boxhead, sizeof(fullbox_header_t));
		dinfbox->struct_size += read_dref_box(bs, drefbox);
		dinfbox->drefbox = drefbox;
	}

	free_struct_fullbox_header(boxhead);

	return dinfbox->struct_size;
}

int write_dinf_box(mp4_bits_t *bs, dinf_box_t *dinfbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	dinfbox->struct_size = write_box_header(bs, dinfbox->boxheader);

	if (dinfbox->drefbox) {
		dinfbox->struct_size += write_dref_box(bs, dinfbox->drefbox);
	}

	dinfbox->boxheader->size = dinfbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, dinfbox->boxheader->size);

	mp4_bs_seek(bs, tail_pos);

	return dinfbox->struct_size;
}

int alloc_struct_dinf_box(dinf_box_t **dinfbox)
{
	dinf_box_t *dinfbox_t = *dinfbox;
	if ((dinfbox_t = (dinf_box_t *)calloc(1, sizeof(dinf_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(dinfbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*dinfbox = dinfbox_t;
	return 1;
}

int free_struct_dinf_box(dinf_box_t *dinfbox)
{
	if (dinfbox) {
		if (dinfbox->boxheader) {
			free_struct_box_header(dinfbox->boxheader);
		}
		free(dinfbox);
		dinfbox = NULL;
	}
	return 1;
}
