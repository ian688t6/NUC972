#include "mp4.h"

int read_free_box(mp4_bits_t *bs, free_box_t *freebox)
{
	freebox->struct_size = freebox->boxheader->struct_size;
	freebox->data_size = freebox->boxheader->size - freebox->boxheader->struct_size;
	if (freebox->data_size > 0) {
		if ((freebox->data = (unsigned char *)calloc(freebox->data_size, sizeof(char))) == NULL) {
			mp4_loge("calloc Failed ");
			return 0;
		}
		mp4_bs_read_data(bs, (char *)freebox->data, freebox->data_size);
		freebox->struct_size += freebox->data_size;
	}

	return freebox->struct_size;
}

int write_free_box(mp4_bits_t *bs, free_box_t *freebox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	freebox->struct_size = write_box_header(bs, freebox->boxheader);
	if (freebox->data_size) {
		mp4_bs_write_data(bs, (char *)freebox->data, freebox->data_size);
	}
	freebox->struct_size += freebox->data_size;

	freebox->boxheader->size = freebox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, freebox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return freebox->struct_size;
}

int alloc_struct_free_box(free_box_t **freebox)
{
	free_box_t *freebox_t = *freebox;

	if ((freebox_t = (free_box_t *)calloc(1, sizeof(free_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(freebox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*freebox = freebox_t;
	return 1;
}

int free_struct_free_box(free_box_t *freebox)
{
	if (freebox) {
		if (free_struct_box_header(freebox->boxheader)) {
			if (freebox->data) {
				free(freebox->data);
				freebox->data = NULL;
			}
		}
		free(freebox);
		freebox = NULL;
	}

	return 1;
}
