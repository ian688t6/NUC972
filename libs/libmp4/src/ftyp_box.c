#include "mp4.h"

int read_ftyp_box(mp4_bits_t *bs, ftyp_box_t *ftypbox)
{
	int i;

	ftypbox->struct_size = ftypbox->boxheader->struct_size;

	ftypbox->major_brand = mp4_bs_read_u32(bs);
	ftypbox->minor_version = mp4_bs_read_u32(bs);
	ftypbox->struct_size += 8;

	ftypbox->brands = (ftypbox->boxheader->size - ftypbox->boxheader->struct_size - 8) / sizeof(int);

	ftypbox->compatible_brands = (uint32_t *)calloc(ftypbox->brands, sizeof(int));

	for (i = 0; i < ftypbox->brands; i++) {
		ftypbox->compatible_brands[i] = mp4_bs_read_u32(bs);
		ftypbox->struct_size += 4;
	}

	return ftypbox->struct_size;
}

int write_ftyp_box(mp4_bits_t *bs, ftyp_box_t *ftypbox)
{
	uint64_t pos, tail_pos;
	int i;

	pos = mp4_bs_get_position(bs);

	ftypbox->struct_size = write_box_header(bs, ftypbox->boxheader);

	mp4_bs_write_u32(bs, ftypbox->major_brand);
	mp4_bs_write_u32(bs, ftypbox->minor_version);
	ftypbox->struct_size += 8;

	for (i = 0; i < ftypbox->brands; i++) {
		mp4_bs_write_u32(bs, ftypbox->compatible_brands[i]);
		ftypbox->struct_size += 4;
	}

	ftypbox->boxheader->size = ftypbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, ftypbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return ftypbox->struct_size;

}

int alloc_struct_ftyp_box(ftyp_box_t ** ftypbox)
{
	ftyp_box_t * ftypbox_t = *ftypbox;
	if ((ftypbox_t = (ftyp_box_t *)calloc(1, sizeof(ftyp_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	if (alloc_struct_box_header(&(ftypbox_t->boxheader)) == 0) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	*ftypbox = ftypbox_t;
	return 1;
}

int free_struct_ftyp_box(ftyp_box_t * ftypbox)
{
	if (ftypbox) {
		if (free_struct_box_header(ftypbox->boxheader)) {
			if (ftypbox->compatible_brands) {
				free(ftypbox->compatible_brands);
				ftypbox->compatible_brands = NULL;
			}
		}
		free(ftypbox);
		ftypbox = NULL;
	}
	return 1;
}
