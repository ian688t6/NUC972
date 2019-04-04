#ifndef __VMHD_BOX_H__
#define __VMHD_BOX_H__

typedef struct tag_vmhd_box {
	fullbox_header_t *boxheader;
	uint16_t graphicsmode; // copy, see below
	uint16_t opcolor[3];
	uint32_t struct_size;
} vmhd_box_t;

int read_vmhd_box(mp4_bits_t *bs, vmhd_box_t *vmhdbox);
int write_vmhd_box(mp4_bits_t *bs, vmhd_box_t *vmhdbox);
int alloc_struct_vmhd_box(vmhd_box_t **vmhdbox);
int free_struct_vmhd_box(vmhd_box_t *vmhdbox);

#endif
