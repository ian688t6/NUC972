#include "mp4.h"

int read_nmhd_box(mp4_bits_t *bs, nmhd_box_t *nmhdbox)
{
	nmhdbox->struct_size = nmhdbox->boxheader->struct_size;

	return nmhdbox->struct_size;
}

int write_nmhd_box(mp4_bits_t *bs, nmhd_box_t *nmhdbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	nmhdbox->struct_size = write_fullbox_header(bs, nmhdbox->boxheader);

	nmhdbox->boxheader->size = nmhdbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, nmhdbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return nmhdbox->struct_size;
}

int alloc_struct_nmhd_box(nmhd_box_t **nmhdbox)
{
	nmhd_box_t * nmhdbox_t = *nmhdbox;
	if ((nmhdbox_t = (nmhd_box_t *)calloc(1, sizeof(mdhd_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(nmhdbox_t->boxheader));
	*nmhdbox = nmhdbox_t;
	return 1;
}

int free_struct_nmhd_box(nmhd_box_t *nmhdbox)
{
	if (nmhdbox) {
		free_struct_fullbox_header(nmhdbox->boxheader);
		free(nmhdbox);
		nmhdbox = NULL;
	}
	return 1;
}
