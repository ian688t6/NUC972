#include "mp4.h"

int read_vmhd_box(mp4_bits_t *bs, vmhd_box_t *vmhdbox)
{
	vmhdbox->struct_size = vmhdbox->boxheader->struct_size;

	vmhdbox->graphicsmode = mp4_bs_read_u16(bs);
	vmhdbox->opcolor[0] = mp4_bs_read_u16(bs);
	vmhdbox->opcolor[1] = mp4_bs_read_u16(bs);
	vmhdbox->opcolor[2] = mp4_bs_read_u16(bs);

	vmhdbox->struct_size += 8;

	return vmhdbox->struct_size;
}

int write_vmhd_box(mp4_bits_t *bs, vmhd_box_t *vmhdbox)
{
	uint64_t pos, tail_pos;

	pos = mp4_bs_get_position(bs);

	vmhdbox->struct_size = write_fullbox_header(bs, vmhdbox->boxheader);

	mp4_bs_write_u16(bs, vmhdbox->graphicsmode);
	mp4_bs_write_u16(bs, vmhdbox->opcolor[0]);
	mp4_bs_write_u16(bs, vmhdbox->opcolor[1]);
	mp4_bs_write_u16(bs, vmhdbox->opcolor[2]);

	vmhdbox->struct_size += 8;

	vmhdbox->boxheader->size = vmhdbox->struct_size;
	tail_pos = mp4_bs_get_position(bs);
	mp4_bs_seek(bs, pos);
	mp4_bs_write_u32(bs, vmhdbox->boxheader->size);
	mp4_bs_seek(bs, tail_pos);

	return vmhdbox->struct_size;
}

int alloc_struct_vmhd_box(vmhd_box_t **vmhdbox)
{
	vmhd_box_t * vmhdbox_t = *vmhdbox;
	if ((vmhdbox_t = (vmhd_box_t *)calloc(1, sizeof(vmhd_box_t))) == NULL) {
		mp4_loge("calloc Failed ");
		return 0;
	}
	alloc_struct_fullbox_header(&(vmhdbox_t->boxheader));
	*vmhdbox = vmhdbox_t;
	return 1;
}

int free_struct_vmhd_box(vmhd_box_t *vmhdbox)
{
	if (vmhdbox) {
		if (vmhdbox->boxheader) {
			free_struct_fullbox_header(vmhdbox->boxheader);
		}
		free(vmhdbox);
		vmhdbox = NULL;
	}
	return 1;
}
