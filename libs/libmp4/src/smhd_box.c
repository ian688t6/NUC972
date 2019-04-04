#include "mp4.h"

int read_smhd_box(mp4_bits_t *bs, smhd_box_t *smhdbox)
{
	smhdbox->struct_size = smhdbox->boxheader->struct_size;

	smhdbox->balance = mp4_bs_read_u16(bs);
	smhdbox->reserved = mp4_bs_read_u16(bs);

	smhdbox->struct_size += 4;

	return smhdbox->struct_size;
}

int write_smhd_box(mp4_bits_t *bs, smhd_box_t *smhdbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	smhdbox->struct_size = write_fullbox_header(bs, smhdbox->boxheader);

	mp4_bs_write_u16(bs, smhdbox->balance);
	mp4_bs_write_u16(bs, smhdbox->reserved);

	smhdbox->struct_size += 4;

	smhdbox->boxheader->size = smhdbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, smhdbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return smhdbox->struct_size;
}

int alloc_struct_smhd_box(smhd_box_t **smhdbox)
{
	smhd_box_t *smhdbox_t = *smhdbox;
	if ((smhdbox_t = (smhd_box_t *)calloc(1, sizeof(smhd_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(smhdbox_t->boxheader));
	*smhdbox = smhdbox_t;
	return 1;
}

int free_struct_smhd_box(smhd_box_t * smhdbox)
{
	if (smhdbox) {
		free_struct_fullbox_header(smhdbox->boxheader);
		free(smhdbox);
		smhdbox = NULL;
	}
	return 1;
}
