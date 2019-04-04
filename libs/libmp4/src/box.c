#include "mp4.h"

int read_box_header(struct mp4_bits *bs, box_header_t *boxheader)
{
	int pos;

	pos = mp4_bs_get_position(bs);
	boxheader->size = mp4_bs_read_u32(bs);
	boxheader->type = mp4_bs_read_u32(bs);
	boxheader->struct_size = 8;
	if (boxheader->size == 0) {
		mp4_loge(" fullbox size == 0");
		return 0;
	} else if (boxheader->size == 1) {
		boxheader->largesize = mp4_bs_read_u64(bs);
		boxheader->struct_size += 8;
	}
	if (boxheader->type == MP4_BOX_TYPE_uuid) {
		mp4_bs_read_data(bs, (char *)boxheader->usertype, 16);
		boxheader->struct_size += 16;
	}
	pos = mp4_bs_get_position(bs) - pos;

	return pos;
}

int read_fullbox_header(struct mp4_bits *bs, fullbox_header_t *boxheader)
{
	int pos;

	pos = mp4_bs_get_position(bs);

	boxheader->size = mp4_bs_read_u32(bs);
	boxheader->type = mp4_bs_read_u32(bs);
	boxheader->struct_size = 8;

	if (boxheader->size == 0) {
		mp4_loge(" fullbox size == 0");
		return 0;
	} else if (boxheader->size == 1) {
		boxheader->largesize = mp4_bs_read_u64(bs);
		boxheader->struct_size += 8;
	}
	if (boxheader->type == MP4_BOX_TYPE_uuid) {
		mp4_bs_read_data(bs, (char *)boxheader->usertype, 16);
		boxheader->struct_size += 16;
	}
	boxheader->version = mp4_bs_read_u8(bs);
	boxheader->flags = mp4_bs_read_u24(bs);
	boxheader->struct_size += 4;

	pos = mp4_bs_get_position(bs) - pos;

	return pos;
}

int write_fullbox_header(mp4_bits_t *bs, fullbox_header_t *boxheader)
{
	mp4_bs_write_u32(bs, boxheader->size);
	mp4_bs_write_u32(bs, boxheader->type);

	boxheader->struct_size = 8;
	if (boxheader->size == 1) {
		mp4_bs_write_u32(bs, boxheader->largesize);
		boxheader->struct_size += 8;
	}
	if (boxheader->type == MP4_BOX_TYPE_uuid) {
		mp4_bs_write_data(bs, (char *)boxheader->usertype, 16);
		boxheader->struct_size += 16;
	}

	mp4_bs_write_u8(bs, boxheader->version);
	mp4_bs_write_u24(bs, boxheader->flags);
	boxheader->struct_size += 4;

	return boxheader->struct_size;
}

int write_box_header(mp4_bits_t *bs, box_header_t *boxheader)
{
	mp4_bs_write_u32(bs, boxheader->size);
	mp4_bs_write_u32(bs, boxheader->type);

	boxheader->struct_size = 8;
	if (boxheader->size == 1) {
		mp4_bs_write_u32(bs, boxheader->largesize);
		boxheader->struct_size += 8;
	}
	if (boxheader->type == MP4_BOX_TYPE_uuid) {
		mp4_bs_write_data(bs, (char *)boxheader->usertype, 16);
		boxheader->struct_size += 16;
	}

	return boxheader->struct_size;
}

int alloc_struct_box_header(box_header_t ** boxheader)
{
	box_header_t *boxheader_t = *boxheader;
	if ((boxheader_t = (box_header_t *)calloc(1, sizeof(box_header_t))) == NULL) {
		mp4_loge("Malloc Failed ");
		return 0;
	}
	*boxheader = boxheader_t;
	return 1;
}

int alloc_struct_fullbox_header(fullbox_header_t ** fullboxheader)
{
	fullbox_header_t *boxheader_t = *fullboxheader;
	if ((boxheader_t = (fullbox_header_t *)calloc(1, sizeof(fullbox_header_t))) == NULL) {
		mp4_loge("Malloc Failed ");
		return 0;
	}
	*fullboxheader = boxheader_t;
	return 1;
}

int free_struct_box_header(box_header_t * boxheader)
{
	if (boxheader) {
		free(boxheader);
		boxheader = NULL;
	}
	return 1;
}

int free_struct_fullbox_header(fullbox_header_t * fullboxheader)
{
	if (fullboxheader) {
		free(fullboxheader);
		fullboxheader = NULL;
	}
	return 1;
}

int alloc_struct_box(box_t ** box)
{
	box_t *box_tmp = *box;
	if ((box_tmp = (box_t *)calloc(1, sizeof(box_t))) == NULL) {
		mp4_loge("Malloc Failed ");
		return 0;
	}

	*box = box_tmp;
	return 1;
}

int free_struct_box(box_t * box)
{
	if (box) {
		if (box->buf) {
			free(box->buf);
			box->buf = NULL;
		}
	}
	free(box);
	box = NULL;
	return 1;
}
