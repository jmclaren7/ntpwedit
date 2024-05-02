/*
 * ntreg.h - NT Registry Hive access library, constants & structures
 * 
 * NOTE: defines are not frozen. It can and will change every release.
 *
 * Copyright (c) 1997-2003 Petter Nordahl-Hagen.
 * Freely distributable in source or binary for noncommercial purposes,
 * but I allow some exceptions to this.
 * Please see the COPYING file for more details on
 * copyrights & credits.
 *  
 * THIS SOFTWARE IS PROVIDED BY PETTER NORDAHL-HAGEN `AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */ 


#ifndef _INCLUDE_NTREG_H
#define _INCLUDE_NTREG_H 1

#define SZ_MAX     4096       /* Max unicode strlen before we truncate */

#define KEY_ROOT   0x2c         /* Type ID of ROOT key node */
#define KEY_NORMAL 0x20       /* Normal nk key */

#define ABSPATHLEN 2048


/* Datatypes of the values in the registry */

#define REG_NONE                    0  /* No value type */
#define REG_SZ                      1  /* Unicode nul terminated string */
#define REG_EXPAND_SZ               2  /* Unicode nul terminated string + env */
#define REG_BINARY                  3  /* Free form binary */
#define REG_DWORD                   4  /* 32-bit number */
#define REG_DWORD_BIG_ENDIAN        5  /* 32-bit number */
#define REG_LINK                    6  /* Symbolic Link (unicode) */
#define REG_MULTI_SZ                7  /* Multiple Unicode strings */
#define REG_RESOURCE_LIST           8  /* Resource list in the resource map */
#define REG_FULL_RESOURCE_DESCRIPTOR 9 /* Resource list in the hardware description */
#define REG_RESOURCE_REQUIREMENTS_LIST 10

#define REG_MAX 10


/* The first page of the registry file is some kind of header, lot of
 * it's contents is unknown, and seems to be mostly NULLs anyway.
 * Note also, that this is the only place in the registry I've been
 * able to find _any_ kind of checksumming
 */

struct regf_header {

  long id;            /* 0x00000000	D-Word	ID: ASCII-"regf" = 0x66676572 */
  long unknown1;      /* 0x00000004	D-Word	???? */
  long unknown2;      /* 0x00000008	D-Word	???? Always the same value as at 0x00000004  */
  char timestamp[8];  /* 0x0000000C	Q-Word	last modify date in WinNT date-format */
  long unknown3;      /* 0x00000014	D-Word	1 */
  long unknown4;      /* 0x00000018	D-Word	3 - probably version #. 2 in NT3.51 */
  long unknown5;      /* 0x0000001C	D-Word	0 */
  long unknown6;      /* 0x00000020	D-Word	1 */
  long ofs_rootkey;   /* 0x00000024	D-Word	Offset of 1st key record */
  long filesize;      /* 0x00000028	D-Word	Size of the data-blocks (Filesize-4kb) */
  long unknown7;      /* 0x0000002C	D-Word	1 */
  char name[0x1fc-0x2c];   /* Seems like the hive's name is buried here, max len unknown */
  long checksum;      /* 0x000001FC	D-Word	Sum of all D-Words from 0x00000000 to 0x000001FB */
};

/* The page header, I don't know if the 14 "dummy" bytes has a meaning,
 * they seem to be mostly NULLS
 */

struct  hbin_page {

  long id;          /* 0x0000	D-Word	ID: ASCII-"hbin" = 0x6E696268  */
  long ofs_from1;   /* 0x0004	D-Word	Offset from the 1st hbin-Block */
  long ofs_next;    /* 0x0008	D-Word	Offset to the next hbin-Block (from THIS ONE)  */
  char dummy1[14];
  long len_page;    /* 0x001C	D-Word	Block-size??? Don't look like it,
                                        I only use the next-offset in this program  */
  char data[1];     /* 0x0020   First data block starts here           */

};

/* Minimum block size utilized at end of block
 * seem to be either 8 or 16, less than this
 * is only filled with garbage. (usually 0xB2 0xB2 ..)
 */
#define HBIN_ENDFILL 0

/* Security descriptor. I know how it's linked, but don't know
   how the real security data is constructed, it may as well
   be like the higher level security structs defined by MS in its
   includes & NT docs. Currently, I have no use for it.
   Note that keys sharing the exact same security settings will
   most likely point to the same security descriptor, thus
   saving space and making it fast to make objects inherit settings
   (is inheritance supported? they speak of security inheritance as a "new"
    feature in the filesystem on NT5, even though I think it was
    also supported by the lower levels in the earlier versions)
*/
struct sk_key {

  short id;          /* 0x0000	Word	ID: ASCII-"sk" = 0x6B73        */
  short dummy1;      /* 0x0002	Word	Unused                         */
  long  ofs_prevsk;  /* 0x0004	D-Word	Offset of previous "sk"-Record */
  long  ofs_nextsk;  /* 0x0008	D-Word	Offset of next "sk"-Record     */
  long  no_usage;    /* 0x000C	D-Word	usage-counter                  */
  long  len_sk;      /* 0x0010	D-Word	Size of "sk"-record in bytes   */
  char  data[4];     /* Security data up to len_sk bytes               */

};

/* This is the subkeylist/hash structure. NT4.0+.
 * ID + count, then count number of offset/4byte "hash". (not true hash)
 * Probably changed from the 3.x version to make it faster to
 * traverse the registry if you're looking for a specific name
 * (saves lookups in 'nk's that have the first 4 name chars different)
 */

struct lf_key {

  short id;         /* 0x0000	Word	ID: ASCII-"lf" = 0x666C or "lh" = 0x686c */
  short no_keys;    /* 0x0002	Word	number of keys          */
                    /* 0x0004	????	Hash-Records            */
  
 union {

    struct lf_hash {
      long ofs_nk;    /* 0x0000	D-Word	Offset of corresponding "nk"-Record  */
      char name[4];   /* 0x0004	D-Word	ASCII: the first 4 characters of the key-name,  */
    } hash[1];

      /* WinXP uses a more real hash instead (base 37 of uppercase name chars)  */
      /* 		padded with 0's. Case sensitiv!                         */

    struct lh_hash {
      long ofs_nk;    /* 0x0000	D-Word	Offset of corresponding "nk"-Record  */
      long hash;      /* 0x0004	D-Word	ASCII: the first 4 characters of the key-name,  */
    } lh_hash[1];
  } u;

};

/* 3.x version of the above, contains only offset table, NOT
 * any start of names "hash". Thus needs 'nk' lookups for searches.
 */
struct li_key {

  short id;         /* 0x0000	Word	ID: ASCII-"li" = 0x696C */
  short no_keys;    /* 0x0002	Word	number of keys          */
                    /* 0x0004	????	Hash-Records            */
  struct li_hash {
    long ofs_nk;    /* 0x0000	D-Word	Offset of corresponding "nk"-Record  */
  } hash[1];
};


/* This is a list of pointers to struct li_key, ie
 * an extention record if many li's.
 * This happens in NT4&5 when the lf hashlist grows larger
 * than about 400-500 entries/subkeys??, then the nk_key->ofs_lf points to this
 * instead of directly to an lf.
 * The sub-indices this points to seems to be li (yes!) in NT4 and 2k.
 * In XP and newer they point to lh which is more efficient.
 * Likely to happen in HKLM\Software\classes (file extention list) and
 * in SAM when many users.
 */
struct ri_key {

  short id;         /* 0x0000	Word	ID: ASCII-"ri" = 0x6972 */
  short no_lis;    /* 0x0002	Word	number of pointers to li */
                    /* 0x0004	????	Hash-Records            */
  struct ri_hash {
      long ofs_li;    /* 0x0000	D-Word	Offset of corresponding "li"-Record  */
  } hash[1];
};


/* This is the value descriptor.
 * If the sign bit (31st bit) in the length field is set, the value is
 * stored inline this struct, and not in a seperate data chunk -
 * the data then seems to be in the type field, and maybe also
 * in the flag and dummy1 field if -len > 4 bytes
 * If the name size == 0, then the struct is probably cut short right
 * after the val_type or flag.
 * The flag meaning is rather unknown.
 */
struct vk_key {

                    /* Offset	Size	Contents                 */
  short id;         /* 0x0000	Word	ID: ASCII-"vk" = 0x6B76  */
  short len_name;   /* 0x0002	Word	name length              */
  long  len_data;   /* 0x0004	D-Word	length of the data       */
  long  ofs_data;   /* 0x0008	D-Word	Offset of Data           */
  long  val_type;   /* 0x000C	D-Word	Type of value            */
  short flag;       /* 0x0010	Word	Flag                     */
  short dummy1;     /* 0x0012	Word	Unused (data-trash)      */
  char  keyname[1]; /* 0x0014	????	Name                     */

};

/* This is the key node (ie directory) descriptor, can contain subkeys and/or values.
 * Note that for values, the count is stored here, but for subkeys
 * there's a count both here and in the offset-table (lf or li struct).
 * What happens if these mismatch is not known.
 * What's the classname thingy? Can't remember seeing that used in
 * anything I've looked at.
 */
struct nk_key {

                        /* Offset	Size	Contents */
  short id;             /*  0x0000	Word	ID: ASCII-"nk" = 0x6B6E                */
  short type;           /*  0x0002	Word	for the root-key: 0x2C, otherwise 0x20 */
  char  timestamp[12];  /*  0x0004	Q-Word	write-date/time in windows nt notation */
  long  ofs_parent;     /*  0x0010	D-Word	Offset of Owner/Parent key             */
  long  no_subkeys;     /*  0x0014	D-Word	number of sub-Keys                     */
  char  dummy1[4];
  long  ofs_lf;         /*  0x001C	D-Word	Offset of the sub-key lf-Records       */
  char  dummy2[4];
  long  no_values;      /*  0x0024	D-Word	number of values                       */
  long  ofs_vallist;    /*  0x0028	D-Word	Offset of the Value-List               */
  long  ofs_sk;         /*  0x002C	D-Word	Offset of the sk-Record                */
  long  ofs_classnam;   /*  0x0030	D-Word	Offset of the Class-Name               */
  char  dummy3[16];
  long  dummy4;         /*  0x0044	D-Word	Unused (data-trash)                    */
  short len_name;       /*  0x0048	Word	name-length                            */
  short len_classnam;   /*  0x004A	Word	class-name length                      */
  char  keyname[1];     /*  0x004C	????	key-name                               */
};

/*********************************************************************************/

/* Structure defines for my routines */

struct ex_data {
  int nkoffs;
  struct nk_key *nk;
  char *name;
};

struct vex_data {
  int vkoffs;
  struct vk_key *vk;
  int type;       /* Value type REG_??? */
  int size;       /* Values size (normalized, inline accounted for) */
  int val;        /* Actual value itself if type==REG_DWORD */
  char *name;
};

struct keyval {
  int len;      /* Length of databuffer */
  int data;    /* Data. Goes on for length of value */
};

struct keyvala {
  int len;      /* Length of databuffer */
  int data[1];    /* Data. Goes on for length of value */
};

#define HMODE_RW        0
#define HMODE_RO        0x1
#define HMODE_OPEN      0x2
#define HMODE_DIRTY     0x4
#define HMODE_NOALLOC   0x8
#define HMODE_VERBOSE 0x1000

/* Suggested type of hive loaded, guessed by library, but not used by it */
#define HTYPE_UNKNOWN   0
#define HTYPE_SAM       1
#define HTYPE_SYSTEM    2
#define HTYPE_SECURITY  3
#define HTYPE_SOFTWARE  4

/* Hive definition, allocated by openHive(), dealloc by closeHive()
 * contains state data, must be passed in all functions
 */
struct hive {
  char *filename;        /* Hives filename */
  int  filedesc;         /* File descriptor (only valid if state == OPEN) */
  int  state;            /* Current state of hive */
  int  type;             /* Suggested type of hive. NOTE: Library will guess when
			    it loads it, but application may change it if needed */
  int  pages;            /* Number of pages, total */
  int  useblk;           /* Total # of used blocks */
  int  unuseblk;         /* Total # of unused blocks */
  int  usetot;           /* total # of bytes in useblk */
  int  unusetot;         /* total # of bytes in unuseblk */
  int  size;             /* Hives size (filesise) in bytes */
  int  rootofs;          /* Offset of root-node */
  short nkindextype;     /* Subkey-indextype the root key uses */
  char *buffer;          /* Files raw contents */
};

/***************************************************/

/* Various nice macros */

#define CREATE(result, type, number)\
    { \
        if (!((result) = (type *) calloc ((number), sizeof(type)))) { \
            perror("malloc failure"); \
            abort() ; \
       } \
    }
#define ALLOC(result, size, number)\
    { \
        if (!((result) = (void *) calloc ((number), (size)))) { \
            perror("malloc failure"); \
            abort() ; \
       } \
    }
#define FREE(p) { if (p) { free(p); (p) = 0; } }


#endif

/******* Function prototypes **********/

char *str_dup( const char *str );
int fmyinput(char *prmpt, char *ibuf, int maxlen);
void hexprnt(char *s, unsigned char *bytes, int len);
void hexdump(char *hbuf, int start, int stop, int ascii);
int find_in_buf(char *buf, char *what, int sz, int len, int start);
int get_int( char *array );
void cheap_uni2ascii(char *src, char *dest, int l);
void cheap_ascii2uni(char *src, char *dest, int l);
void skipspace(char **c);
int gethex(char **c);
int gethexorstr(char **c, char *wb);
int debugit(char *buf, int sz);
int parse_block(struct hive *hdesc, int vofs,int verbose);
int ex_next_n(struct hive *hdesc, int nkofs, int *count, int *countri, struct ex_data *sptr);
int ex_next_v(struct hive *hdesc, int nkofs, int *count, struct vex_data *sptr);
int get_abs_path(struct hive *hdesc, int nkofs, char *path, int maxlen);
int trav_path(struct hive *hdesc, int vofs, char *path, int type);
int get_val_type(struct hive *hdesc, int vofs, char *path);
int get_val_len(struct hive *hdesc, int vofs, char *path);
void *get_val_data(struct hive *hdesc, int vofs, char *path, int val_type);
struct keyval *get_val2buf(struct hive *hdesc, struct keyval *kv,
			   int vofs, char *path, int type );
int get_dword(struct hive *hdesc, int vofs, char *path);
int put_buf2val(struct hive *hdesc, struct keyval *kv,
		int vofs, char *path, int type );
int put_dword(struct hive *hdesc, int vofs, char *path, int dword);
void closeHive(struct hive *hdesc);
int writeHive(struct hive *hdesc);
struct hive *openHive(char *filename, int mode);

void nk_ls(struct hive *hdesc, char *path, int vofs, int type);

struct vk_key *add_value(struct hive *hdesc, int nkofs, char *name, int type);
void del_allvalues(struct hive *hdesc, int nkofs);
int del_value(struct hive *hdesc, int nkofs, char *name);
struct nk_key *add_key(struct hive *hdesc, int nkofs, char *name);
int del_key(struct hive *hdesc, int nkofs, char *name);
void rdel_keys(struct hive *hdesc, char *path, int nkofs);
struct keyval *get_class(struct hive *hdesc, int curnk, char *path);
