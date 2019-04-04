#include <unistd.h>
#include "mp4.h"

/* private */
struct mp4_bits *mp4_bs_create(char *buffer, uint64_t BufferSize, uint32_t mode)
{
	struct mp4_bits *tmp;
	if ((buffer && !BufferSize))
		return NULL;

	tmp = (struct mp4_bits *)malloc(sizeof(struct mp4_bits));
	if (!tmp)
		return NULL;
	memset(tmp, 0, sizeof(struct mp4_bits));

	tmp->original = (char *)buffer;
	tmp->size = BufferSize;

	tmp->position = 0;
	tmp->current = 0;
	tmp->bsmode = mode;
#ifndef USE_FILE_IO_INTERFACE
	tmp->stream = NULL;
#endif

	switch (tmp->bsmode) {
	case MP4_BITS_READ:
		tmp->nbBits = 8;
		tmp->current = 0;
		break;
	case MP4_BITS_WRITE:
		tmp->nbBits = 0;
		if (!buffer) {
			/*if BufferSize is specified, use it. This is typically used when AvgSize of
			 some buffers is known, but some exceed it. */
			if (BufferSize) {
				tmp->size = BufferSize;
			} else {
				tmp->size = BS_MEM_BLOCK_ALLOC_SIZE;
			}
			tmp->original = (char *)malloc(sizeof(char) * ((uint32_t)tmp->size));
			if (!tmp->original) {
				free(tmp);
				return NULL;
			}
			tmp->bsmode = MP4_BITS_WRITE_DYN;
		} else {
			tmp->original = (char *)buffer;
			tmp->size = BufferSize;
		}
		break;
	default:
		/*the stream constructor is not the same... */
		free(tmp);
		return NULL;
	}
	return tmp;
}

#ifdef USE_FILE_IO_INTERFACE
struct mp4_bits *mp4_bs_create_from_file(int fd, uint32_t mode)
#else
struct mp4_bits *mp4_bs_create_from_file(FILE *f, uint32_t mode)
#endif
{
	struct mp4_bits *tmp;

	tmp = (struct mp4_bits *)malloc(sizeof(struct mp4_bits));
	if (!tmp)
		return NULL;
	memset(tmp, 0, sizeof(struct mp4_bits));

	/*switch to internal mode */
	mode = (mode == MP4_BITS_READ) ? MP4_BITS_FILE_READ : MP4_BITS_FILE_WRITE;
	tmp->bsmode = mode;
	tmp->current = 0;
	tmp->nbBits = (mode == MP4_BITS_FILE_READ) ? 8 : 0;
	tmp->original = NULL;
	tmp->position = 0;
#ifdef USE_FILE_IO_INTERFACE
	tmp->fd = fd;
#else
	tmp->stream = f;
	tmp->fd = fileno(f);
#endif

	/*get the size of this file (for read streams) */
#ifdef USE_FILE_IO_INTERFACE
	lseek(fd, 0, SEEK_SET);
	tmp->size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
#else
	tmp->position = ftell(f);
	fseek(f, 0, SEEK_END);
	tmp->size = ftell(f);
	fseek(f, tmp->position, SEEK_SET);
#endif

	return tmp;
}

void mp4_bs_destroy(struct mp4_bits *bs)
{
	if (!bs)
		return;
	/*if we are in dynamic mode (alloc done by the bitstream), free the buffer if still present */
	if ((bs->bsmode == MP4_BITS_WRITE_DYN) && bs->original)
		free(bs->original);

	free(bs);
}

/*returns 1 if aligned wrt current mode, 0 otherwise*/
static int bs_is_align(struct mp4_bits *bs)
{
	switch (bs->bsmode) {
	case MP4_BITS_READ:
	case MP4_BITS_FILE_READ:
		return ((8 == bs->nbBits) ? 1 : 0);
	default:
		return !bs->nbBits;
	}
}

/*fetch a new byte in the bitstream switch between packets*/
static uint8_t bs_read_byte(struct mp4_bits *bs)
{
#ifdef USE_FILE_IO_INTERFACE
	uint8_t ch;
#endif

	if (bs->bsmode == MP4_BITS_READ) {
		if (bs->position == bs->size) {
			if (bs->end_of_stream)
				bs->end_of_stream(bs->par);
			return 0;
		}
		return (uint8_t)bs->original[bs->position++];
	}
#ifdef USE_FILE_IO_INTERFACE
	bs->position++;
	read(bs->fd, &ch, 1);
	return ch;
#else
	/*we are in FILE mode, test for end of file */
	if (!feof(bs->stream)) {
		bs->position++;
		return fgetc(bs->stream);
	}
#endif
	if (bs->end_of_stream)
		bs->end_of_stream(bs->par);
	return 0;
}

/*#define NO_OPTS */

static uint8_t bit_mask[] = {
	0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1
};
static uint8_t bits_mask[] = {
	0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F
};

uint8_t mp4_bs_read_bit(struct mp4_bits *bs)
{
	if (bs->nbBits == 8) {
		bs->current = bs_read_byte(bs);
		bs->nbBits = 0;
	}
#ifdef NO_OPTS
	{
		s32 ret;
		bs->current <<= 1;
		bs->nbBits++;
		ret = (bs->current & 0x100) >> 8;
		return (uint8_t) ret;
	}
#else
	return (uint8_t)(bs->current & bit_mask[bs->nbBits++]) ? 1 : 0;
#endif

}

uint32_t mp4_bs_read_int(struct mp4_bits * bs, uint32_t nBits)
{
	uint32_t ret;

#ifndef NO_OPTS
	if (nBits + bs->nbBits <= 8) {
		bs->nbBits += nBits;
		ret = (bs->current >> (8 - bs->nbBits)) & bits_mask[nBits];
		return ret;
	}
#endif
	ret = 0;
	while (nBits-- > 0) {
		ret <<= 1;
		ret |= mp4_bs_read_bit(bs);
	}
	return ret;
}

uint32_t mp4_bs_read_u8(struct mp4_bits * bs)
{
	assert(bs->nbBits == 8);
	return (uint32_t)bs_read_byte(bs);
}

uint32_t mp4_bs_read_u16(struct mp4_bits * bs)
{
	uint32_t ret;
	assert(bs->nbBits == 8);
	ret = bs_read_byte(bs);
	ret <<= 8;
	ret |= bs_read_byte(bs);
	return ret;
}

uint32_t mp4_bs_read_u24(struct mp4_bits * bs)
{
	uint32_t ret;
	assert(bs->nbBits == 8);
	ret = bs_read_byte(bs);
	ret <<= 8;
	ret |= bs_read_byte(bs);
	ret <<= 8;
	ret |= bs_read_byte(bs);
	return ret;
}

uint32_t mp4_bs_read_u32(struct mp4_bits * bs)
{
	uint32_t ret;
	assert(bs->nbBits == 8);
	ret = bs_read_byte(bs);
	ret <<= 8;
	ret |= bs_read_byte(bs);
	ret <<= 8;
	ret |= bs_read_byte(bs);
	ret <<= 8;
	ret |= bs_read_byte(bs);
	return ret;
}

uint64_t mp4_bs_read_u64(struct mp4_bits * bs)
{
	uint64_t ret;
	ret = mp4_bs_read_u32(bs);
	ret <<= 32;
	ret |= mp4_bs_read_u32(bs);
	return ret;
}

uint64_t mp4_bs_read_long_int(struct mp4_bits * bs, uint32_t nBits)
{
	uint64_t ret = 0;
	while (nBits-- > 0) {
		ret <<= 1;
		ret |= mp4_bs_read_bit(bs);
	}
	return ret;
}

float mp4_bs_read_float(struct mp4_bits *bs)
{
	char buf[4] = "\0\0\0";
	float fbuf;
#ifdef NO_OPTS
	s32 i;
	for (i = 0; i < 32; i++)
	buf[3 - i / 8] |= mp4_bs_read_bit(bs) << (7 - i % 8);
#else
	buf[3] = mp4_bs_read_int(bs, 8);
	buf[2] = mp4_bs_read_int(bs, 8);
	buf[1] = mp4_bs_read_int(bs, 8);
	buf[0] = mp4_bs_read_int(bs, 8);
#endif
	memcpy(&fbuf, buf, sizeof(buf));
	return fbuf;
}

double mp4_bs_read_double(struct mp4_bits *bs)
{
	char buf[8] = "\0\0\0\0\0\0\0";
	double dbuf;
	int32_t i;

	for (i = 0; i < 64; i++)
		buf[7 - i / 8] |= mp4_bs_read_bit(bs) << (7 - i % 8);
	memcpy(&dbuf, buf, sizeof(buf));
	return dbuf;
}

uint32_t mp4_bs_read_data(struct mp4_bits *bs, char *data, uint32_t nbBytes)
{
	uint64_t orig = bs->position;

	if (bs->position + nbBytes > bs->size)
		return 0;

	if (bs_is_align(bs)) {
		switch (bs->bsmode) {
		case MP4_BITS_READ:
			memcpy(data, bs->original + bs->position, nbBytes);
			bs->position += nbBytes;
			return nbBytes;
		case MP4_BITS_FILE_READ:
		case MP4_BITS_FILE_WRITE:
#ifdef USE_FILE_IO_INTERFACE
			read(bs->fd, data, nbBytes);
#else
			fread(data, nbBytes, 1, bs->stream);
#endif
			bs->position += nbBytes;
			return nbBytes;
		default:
			return 0;
		}
	}

	while (nbBytes-- > 0) {
		*data++ = mp4_bs_read_int(bs, 8);
	}
	return (uint32_t)(bs->position - orig);

}

static void bs_write_byte(struct mp4_bits *bs, uint8_t val)
{
	/*we don't allow write on READ buffers */
	if ((bs->bsmode == MP4_BITS_READ) || (bs->bsmode == MP4_BITS_FILE_READ))
		return;
#ifdef USE_FILE_IO_INTERFACE
	if (!bs->original && bs->fd < 0)
#else
	if (!bs->original && !bs->stream)
#endif
		return;

	/*we are in MEM mode */
	if ((bs->bsmode == MP4_BITS_WRITE) || (bs->bsmode == MP4_BITS_WRITE_DYN)) {
		if (bs->position == bs->size) {
			/*no more space... */
			if (bs->bsmode != MP4_BITS_WRITE_DYN)
				return;
			/*realloc if enough space... */
			if (bs->size > 0xFFFFFFFF)
				return;
			bs->original = (char *)realloc(bs->original, (uint32_t)(bs->size +
			BS_MEM_BLOCK_ALLOC_SIZE));
			if (!bs->original)
				return;
			bs->size += BS_MEM_BLOCK_ALLOC_SIZE;
		}
		bs->original[bs->position] = val;
		bs->position++;
		return;
	}
	/*we are in FILE mode, no pb for any realloc... */
#if defined(USE_FILE_IO_INTERFACE) || defined(ONLY_READ_WRITE_USE_FILE_IO)
	if (write(bs->fd, &val, 1) != 1) {
		mp4_loge("write failed\n");
	}
#else
	fputc(val, bs->stream);
#endif
	/*check we didn't rewind the stream */
	if (bs->size == bs->position)
		bs->size++;
	bs->position += 1;
}

static void bs_write_bit(struct mp4_bits *bs, uint32_t bit)
{
	bs->current <<= 1;
	bs->current |= bit;
	if (++bs->nbBits == 8) {
		bs->nbBits = 0;
		bs_write_byte(bs, (uint8_t)bs->current);
		bs->current = 0;
	}
}

void mp4_bs_write_int(struct mp4_bits *bs, int32_t value, int32_t nBits)
{
	value <<= sizeof(int32_t) * 8 - nBits;
	while (--nBits >= 0) {
		bs_write_bit(bs, value < 0);
		value <<= 1;
	}
}

void mp4_bs_write_long_int(struct mp4_bits *bs, int64_t value, int32_t nBits)
{
	value <<= sizeof(int64_t) * 8 - nBits;
	while (--nBits >= 0) {
		bs_write_bit(bs, value < 0);
		value <<= 1;
	}
}

void mp4_bs_write_u8(struct mp4_bits *bs, uint32_t value)
{
	assert(!bs->nbBits);
	bs_write_byte(bs, (uint8_t)value);
}

void mp4_bs_write_u16(struct mp4_bits *bs, uint32_t value)
{
	assert(!bs->nbBits);
	bs_write_byte(bs, (uint8_t)((value >> 8) & 0xff));
	bs_write_byte(bs, (uint8_t)((value) & 0xff));
}

void mp4_bs_write_u24(struct mp4_bits *bs, uint32_t value)
{
	assert(!bs->nbBits);
	bs_write_byte(bs, (uint8_t)((value >> 16) & 0xff));
	bs_write_byte(bs, (uint8_t)((value >> 8) & 0xff));
	bs_write_byte(bs, (uint8_t)((value) & 0xff));
}

void mp4_bs_write_u32(struct mp4_bits *bs, uint32_t value)
{
	assert(!bs->nbBits);
	bs_write_byte(bs, (uint8_t)((value >> 24) & 0xff));
	bs_write_byte(bs, (uint8_t)((value >> 16) & 0xff));
	bs_write_byte(bs, (uint8_t)((value >> 8) & 0xff));
	bs_write_byte(bs, (uint8_t)((value) & 0xff));
}

void mp4_bs_write_u64(struct mp4_bits *bs, uint64_t value)
{
	assert(!bs->nbBits);
	mp4_bs_write_u32(bs, (uint32_t)((value >> 32) & 0xffffffff));
	mp4_bs_write_u32(bs, (uint32_t)(value & 0xffffffff));
}

void mp4_bs_write_float(struct mp4_bits *bs, float value)
{
	uint32_t i;
	union {
		float f;
		char sz[4];
	} float_value;
	float_value.f = value;

	for (i = 0; i < 32; i++)
		bs_write_bit(bs, (float_value.sz[3 - i / 8] & 1 << (7 - i % 8)) != 0);

}

void mp4_bs_write_double(struct mp4_bits *bs, double value)
{
	uint32_t i;
	union {
		double d;
		char sz[8];
	} double_value;
	double_value.d = value;
	for (i = 0; i < 64; i++) {
		bs_write_bit(bs, (double_value.sz[7 - i / 8] & 1 << (7 - i % 8)) != 0);
	}
}

uint32_t mp4_bs_write_data(struct mp4_bits *bs, char *data, uint32_t nbBytes)
{
	/*we need some feedback for this guy... */
	uint64_t begin = bs->position;
	if (!nbBytes) {
		mp4_loge("nbBytes = 0\n");
		return 0;
	}

	if (bs_is_align(bs)) {
		switch (bs->bsmode) {
		case MP4_BITS_WRITE:
			if (bs->position + nbBytes > bs->size)
				return 0;
			memcpy(bs->original + bs->position, data, nbBytes);
			bs->position += nbBytes;
			return nbBytes;
		case MP4_BITS_WRITE_DYN:
			/*need to realloc ... */
			if (bs->position + nbBytes > bs->size) {
				if (bs->size + nbBytes > 0xFFFFFFFF)
					return 0;
				bs->original = (char *)realloc(bs->original, sizeof(uint32_t) * ((uint32_t)bs->size + nbBytes));
				if (!bs->original)
					return 0;
				bs->size += nbBytes;
			}
			memcpy(bs->original + bs->position, data, nbBytes);
			bs->position += nbBytes;
			return nbBytes;
		case MP4_BITS_FILE_READ:
		case MP4_BITS_FILE_WRITE:
#if defined(USE_FILE_IO_INTERFACE) || defined(ONLY_READ_WRITE_USE_FILE_IO)
			if (write(bs->fd, data, nbBytes) != nbBytes)
#else
			if (fwrite(data, nbBytes, 1, bs->stream) != 1)
#endif
			{
				mp4_loge("write failed\n");
				return 0;
			}
			if (bs->size == bs->position)
				bs->size += nbBytes;
			bs->position += nbBytes;
			return nbBytes;
		default:
			mp4_loge("invalid mode = %d\n", bs->bsmode);
			return 0;
		}
	}

	while (nbBytes) {
		mp4_bs_write_int(bs, (int32_t)*data, 8);
		data++;
		nbBytes--;
	}
	return (uint32_t)(bs->position - begin);
}

/*align return the num of bits read in READ mode, 0 in WRITE*/

uint8_t mp4_bs_align(struct mp4_bits * bs)
{
	uint8_t res = 8 - bs->nbBits;
	if ((bs->bsmode == MP4_BITS_READ) || (bs->bsmode == MP4_BITS_FILE_READ)) {
		if (res > 0) {
			mp4_bs_read_int(bs, res);
		}
		return res;
	}
	if (bs->nbBits > 0) {
		mp4_bs_write_int(bs, 0, res);
		return res;
	}
	return 0;
}

/*size available in the bitstream*/
#ifndef USE_FILE_IO_INTERFACE
uint64_t mp4_bs_available(struct mp4_bits * bs)
{
	int64_t cur, end;

	/*in WRITE mode only, this should not be called, but return something big in case ... */
	if ((bs->bsmode == MP4_BITS_WRITE) || (bs->bsmode == MP4_BITS_WRITE_DYN))
		return (uint64_t)-1;

	/*we are in MEM mode */
	if (bs->bsmode == MP4_BITS_READ) {
		return (bs->size - bs->position);
	}
	/*FILE READ: assume size hasn't changed, otherwise the user shall call mp4_bs_get_refreshed_size */
	if (bs->bsmode == MP4_BITS_FILE_READ)
		return (bs->size - bs->position);

	cur = ftell(bs->stream);
	fseek(bs->stream, 0, SEEK_END);
	end = ftell(bs->stream);
	fseek(bs->stream, cur, SEEK_SET);
	return (uint64_t)(end - cur);
}
#endif

/*call this funct to set the buffer size to the nb of bytes written 
 Used only in WRITE mode, as we don't know the real size during allocation...
 return -1 for bad param or malloc failed
 return nbBytes cut*/
static uint32_t bs_cut_buffer(struct mp4_bits *bs)
{
	uint32_t nbBytes;
	if ((bs->bsmode != MP4_BITS_WRITE_DYN) && (bs->bsmode != MP4_BITS_WRITE))
		return (uint32_t)-1;
	/*Align our buffer or we're dead! */
	mp4_bs_align(bs);

	nbBytes = (uint32_t)(bs->size - bs->position);
	if (!nbBytes || (nbBytes == 0xFFFFFFFF) || (bs->position >= 0xFFFFFFFF))
		return 0;
	bs->original = (char *)realloc(bs->original, (uint32_t)bs->position);
	if (!bs->original)
		return (uint32_t)-1;
	/*just in case, re-adjust.. */
	bs->size = bs->position;
	return nbBytes;
}

/*For DYN mode, this gets the content out*/

void mp4_bs_get_content(struct mp4_bits *bs, char **output, uint32_t * outSize)
{
	/*only in WRITE MEM mode */
	if (bs->bsmode != MP4_BITS_WRITE_DYN)
		return;
	if (!bs->position && !bs->nbBits) {
		*output = NULL;
		*outSize = 0;
		free(bs->original);
	} else {
		bs_cut_buffer(bs);
		*output = bs->original;
		*outSize = (uint32_t)bs->size;
	}
	bs->original = NULL;
	bs->size = 0;
	bs->position = 0;
}

/*For DYN mode, this obtain the content out*/
void mp4_bs_obtain_content(struct mp4_bits *bs, char **output, uint32_t *outSize)
{
	/*only in WRITE MEM mode */
	if (bs->bsmode != MP4_BITS_WRITE_DYN || !bs->position) {
		*output = NULL;
		*outSize = 0;
		return;
	}

	*output = bs->original;
	*outSize = (uint32_t)bs->position;
	bs->position = 0;
}

/*	Skip nbytes. 
 Align
 If READ (MEM or FILE) mode, just read n times 8 bit
 If WRITE (MEM or FILE) mode, write n times 0 on 8 bit
 */

void mp4_bs_skip_bytes(struct mp4_bits *bs, uint64_t nbBytes)
{
	if (!bs || !nbBytes)
		return;

	mp4_bs_align(bs);

	/*special case for file skipping... */
	if ((bs->bsmode == MP4_BITS_FILE_WRITE) || (bs->bsmode == MP4_BITS_FILE_READ)) {
#ifdef USE_FILE_IO_INTERFACE
		lseek(bs->fd, nbBytes, SEEK_CUR);
#else
		fseek(bs->stream, nbBytes, SEEK_CUR);
#endif
		bs->position += nbBytes;
		return;
	}

	/*special case for reading */
	if (bs->bsmode == MP4_BITS_READ) {
		bs->position += nbBytes;
		return;
	}
	/*for writing we must do it this way, otherwise pb in dynamic buffers */
	while (nbBytes) {
		mp4_bs_write_int(bs, 0, 8);
		nbBytes--;
	}
}

/*Only valid for READ MEMORY*/

void mp4_bs_rewind_bits(struct mp4_bits *bs, uint64_t nbBits)
{
	uint64_t nbBytes;
	if (bs->bsmode != MP4_BITS_READ)
		return;

	nbBits -= (bs->nbBits);
	nbBytes = (nbBits + 8) >> 3;
	nbBits = nbBytes * 8 - nbBits;
	mp4_bs_align(bs);
	assert(bs->position >= nbBytes);
	bs->position -= nbBytes + 1;
	mp4_bs_read_int(bs, (uint32_t)nbBits);
	return;
}

/*seek from begining of stream: use internally even when non aligned!*/
static int bs_seek_intern(struct mp4_bits *bs, uint64_t offset)
{
	uint32_t i;
	/*if mem, do it */
	if ((bs->bsmode == MP4_BITS_READ) || (bs->bsmode == MP4_BITS_WRITE) || (bs->bsmode == MP4_BITS_WRITE_DYN)) {
		if (offset > 0xFFFFFFFF)
			return -1;
		/*0 for write, read will be done automatically */
		if (offset >= bs->size) {
			if ((bs->bsmode == MP4_BITS_READ) || (bs->bsmode == MP4_BITS_WRITE))
				return -1;
			/*in DYN, realloc ... */
			bs->original = (char *)realloc(bs->original, (uint32_t)(offset + 1));
			for (i = 0; i < (uint32_t)(offset + 1 - bs->size); i++) {
				bs->original[bs->size + i] = 0;
			}
			bs->size = offset + 1;
		}
		bs->current = bs->original[offset];
		bs->position = offset;
		bs->nbBits = (bs->bsmode == MP4_BITS_READ) ? 8 : 0;
		return 0;
	}

#ifdef USE_FILE_IO_INTERFACE
	lseek(bs->fd, offset, SEEK_SET);
#else
	fseek(bs->stream, offset, SEEK_SET);
#endif

	bs->position = offset;
	bs->current = 0;
	/*setup NbBits so that next acccess to the buffer will trigger read/write */
	bs->nbBits = (bs->bsmode == MP4_BITS_FILE_READ) ? 8 : 0;
	return 0;
}

/*seek from begining of stream: align before anything else*/

int mp4_bs_seek(struct mp4_bits *bs, uint64_t offset)
{
	/*warning: we allow offset = bs->size for WRITE buffers */
	if (offset > bs->size)
		return -1;

	mp4_bs_align(bs);
	return bs_seek_intern(bs, offset);
}

/*peek bits (as int!!) from orig position (ON BYTE BOUNDARIES, from 0) - only for read ...*/

uint32_t mp4_bs_peek_bits(struct mp4_bits * bs, uint32_t numBits, uint32_t byte_offset)
{
	uint64_t curPos;
	uint32_t curBits, ret, current;

	if ((bs->bsmode != MP4_BITS_READ) && (bs->bsmode != MP4_BITS_FILE_READ))
		return 0;
	if (!numBits || (bs->size < bs->position + byte_offset))
		return 0;

	/*store our state */
	curPos = bs->position;
	curBits = bs->nbBits;
	current = bs->current;

	if (byte_offset)
		mp4_bs_seek(bs, bs->position + byte_offset);
	ret = mp4_bs_read_int(bs, numBits);

	/*restore our cache - position */
	mp4_bs_seek(bs, curPos);
	/*to avoid re-reading our bits ... */
	bs->nbBits = curBits;
	bs->current = current;
	return ret;
}

#ifndef USE_FILE_IO_INTERFACE
uint64_t mp4_bs_get_refreshed_size(struct mp4_bits * bs)
{
	int64_t offset;

	switch (bs->bsmode) {
	case MP4_BITS_READ:
	case MP4_BITS_WRITE:
		return bs->size;

	default:
		offset = ftell(bs->stream);
		fseek(bs->stream, 0, SEEK_END);
		bs->size = ftell(bs->stream);
		fseek(bs->stream, offset, SEEK_SET);
		return bs->size;
	}
}
#endif

uint64_t mp4_bs_get_size(struct mp4_bits * bs)
{
	return bs->size;
}

uint64_t mp4_bs_get_position(struct mp4_bits * bs)
{
	return bs->position;
}

uint8_t mp4_bs_bits_available(struct mp4_bits * bs)
{
	if (bs->size > bs->position)
		return 8;
	if (bs->nbBits < 8)
		return (8 - bs->nbBits);
	return 0;
}

void mp4_bs_set_eos_callback(struct mp4_bits *bs, void (*end_of_stream)(void *par), void *par)
{
	bs->end_of_stream = end_of_stream;
	bs->par = par;
}

uint32_t mp4_bs_read_u32_le(struct mp4_bits *bs)
{
	uint32_t ret, v;
	ret = mp4_bs_read_int(bs, 8);
	v = mp4_bs_read_int(bs, 8);
	v <<= 8;
	ret |= v;
	v = mp4_bs_read_int(bs, 8);
	v <<= 16;
	ret |= v;
	v = mp4_bs_read_int(bs, 8);
	v <<= 24;
	ret |= v;
	return ret;
}

uint16_t mp4_bs_read_u16_le(struct mp4_bits * bs)
{
	uint32_t ret, v;
	ret = mp4_bs_read_int(bs, 8);
	v = mp4_bs_read_int(bs, 8);
	v <<= 8;
	ret |= v;
	return ret;
}

void mp4_bs_write_u32_le(struct mp4_bits *bs, uint32_t val)
{
	mp4_bs_write_int(bs, val & 0xFF, 8);
	mp4_bs_write_int(bs, val >> 8, 8);
	mp4_bs_write_int(bs, val >> 16, 8);
	mp4_bs_write_int(bs, val >> 24, 8);
}

void mp4_bs_write_u16_le(struct mp4_bits *bs, uint32_t val)
{
	mp4_bs_write_int(bs, val & 0xFF, 8);
	mp4_bs_write_int(bs, val >> 8, 8);
}

uint32_t mp4_bs_get_bit_offset(struct mp4_bits *bs)
{
	return (uint32_t)((bs->position - 1) * 8 + bs->nbBits);
}
