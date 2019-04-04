#include "mp4.h"

int read_sowt_box(mp4_bits_t *bs, sowt_box_t *sowtbox)
{
	sowtbox->struct_size = sowtbox->boxheader->struct_size;

	sowtbox->reseved[0] = mp4_bs_read_u8(bs);
	sowtbox->reseved[1] = mp4_bs_read_u8(bs);
	sowtbox->reseved[2] = mp4_bs_read_u8(bs);
	sowtbox->reseved[3] = mp4_bs_read_u8(bs);
	sowtbox->reseved[4] = mp4_bs_read_u8(bs);
	sowtbox->reseved[5] = mp4_bs_read_u8(bs);
	sowtbox->data_reference_index = mp4_bs_read_u16(bs);

	sowtbox->version = mp4_bs_read_u16(bs);
	sowtbox->revision_level = mp4_bs_read_u16(bs);
	sowtbox->vendor = mp4_bs_read_u32(bs);
	sowtbox->channel_num = mp4_bs_read_u16(bs);
	sowtbox->sample_size = mp4_bs_read_u16(bs);
	sowtbox->compression_id = mp4_bs_read_u16(bs);
	sowtbox->packet_size = mp4_bs_read_u16(bs);
	sowtbox->sample_rate = mp4_bs_read_u32(bs) >> 16;
	sowtbox->struct_size += 28;

	return sowtbox->struct_size;
}

int write_sowt_box(mp4_bits_t *bs, sowt_box_t *sowtbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	sowtbox->struct_size = write_box_header(bs, sowtbox->boxheader);

	mp4_bs_write_u8(bs, sowtbox->reseved[0]);
	mp4_bs_write_u8(bs, sowtbox->reseved[1]);
	mp4_bs_write_u8(bs, sowtbox->reseved[2]);
	mp4_bs_write_u8(bs, sowtbox->reseved[2]);
	mp4_bs_write_u8(bs, sowtbox->reseved[4]);
	mp4_bs_write_u8(bs, sowtbox->reseved[5]);
	mp4_bs_write_u16(bs, sowtbox->data_reference_index);

	mp4_bs_write_u16(bs, sowtbox->version);
	mp4_bs_write_u16(bs, sowtbox->revision_level);
	mp4_bs_write_u32(bs, sowtbox->vendor);
	mp4_bs_write_u16(bs, sowtbox->channel_num);
	mp4_bs_write_u16(bs, sowtbox->sample_size);
	mp4_bs_write_u16(bs, sowtbox->compression_id);
	mp4_bs_write_u16(bs, sowtbox->packet_size);
	mp4_bs_write_u32(bs, sowtbox->sample_rate << 16);

	sowtbox->struct_size += 28;

	sowtbox->boxheader->size = sowtbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, sowtbox->boxheader->size);

	mp4_bs_seek(bs, tail_pos);

	return sowtbox->struct_size;
}

int alloc_struct_sowt_box(sowt_box_t **sowtbox)
{
	sowt_box_t *sowtbox_t = *sowtbox;
	if ((sowtbox_t = (sowt_box_t *)calloc(1, sizeof(sowt_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(sowtbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*sowtbox = sowtbox_t;
	return 1;
}

int free_struct_sowt_box(sowt_box_t *sowtbox)
{
	if (sowtbox) {
		if (sowtbox->boxheader) {
			free_struct_box_header(sowtbox->boxheader);
		}
		free(sowtbox);
		sowtbox = NULL;
	}
	return 1;
}
