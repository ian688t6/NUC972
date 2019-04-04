#include "mp4.h"

int read_hmhd_box(mp4_bits_t *bs, hmhd_box_t *hmhdbox)
{
	hmhdbox->struct_size = hmhdbox->boxheader->struct_size;

	hmhdbox->maxPDUsize = mp4_bs_read_u16(bs);
	hmhdbox->avgPDUsize = mp4_bs_read_u16(bs);
	hmhdbox->maxbitrate = mp4_bs_read_u32(bs);
	hmhdbox->avgbitrate = mp4_bs_read_u32(bs);
	hmhdbox->reserved = mp4_bs_read_u32(bs);

	hmhdbox->struct_size += 16;

	return hmhdbox->struct_size;
}

int write_hmhd_box(mp4_bits_t *bs, hmhd_box_t *hmhdbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	hmhdbox->struct_size = write_fullbox_header(bs, hmhdbox->boxheader);

	mp4_bs_write_u16(bs, hmhdbox->maxPDUsize);
	mp4_bs_write_u16(bs, hmhdbox->avgPDUsize);
	mp4_bs_write_u32(bs, hmhdbox->maxbitrate);
	mp4_bs_write_u32(bs, hmhdbox->avgbitrate);
	mp4_bs_write_u32(bs, hmhdbox->reserved);

	hmhdbox->struct_size += 16;

	hmhdbox->boxheader->size = hmhdbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, hmhdbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return hmhdbox->struct_size;
}

int alloc_struct_hmhd_box(hmhd_box_t **hmhdbox)
{
	hmhd_box_t * hmhdbox_t = *hmhdbox;
	if ((hmhdbox_t = (hmhd_box_t *)calloc(1, sizeof(hmhd_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(hmhdbox_t->boxheader));
	*hmhdbox = hmhdbox_t;
	return 1;
}

int free_struct_hmhd_box(hmhd_box_t * hmhdbox)
{
	if (hmhdbox) {
		if (hmhdbox->boxheader) {
			free_struct_fullbox_header(hmhdbox->boxheader);
		}
		free(hmhdbox);
		hmhdbox = NULL;
	}
	return 1;
}
