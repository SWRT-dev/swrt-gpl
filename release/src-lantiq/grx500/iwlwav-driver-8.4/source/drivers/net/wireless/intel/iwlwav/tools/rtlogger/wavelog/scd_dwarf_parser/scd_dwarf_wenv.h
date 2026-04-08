#ifndef _SCD_DWARF_WENV_H_
#define _SCD_DWARF_WENV_H_

typedef enum ftenum {
  FT_NONE,        /* used for text labels with no value */
  FT_PROTOCOL,
  FT_BOOLEAN,     /* TRUE and FALSE come from <glib.h> */
  FT_UINT8,
  FT_UINT16,
  FT_UINT24,      /* really a UINT32, but displayed as 3 hex-digits if FD_HEX*/
  FT_UINT32,
  FT_UINT64,
  FT_INT8,
  FT_INT16,
  FT_INT24,       /* same as for UINT24 */
  FT_INT32,
  FT_INT64,
  FT_FLOAT,
  FT_DOUBLE,
  FT_ABSOLUTE_TIME,
  FT_RELATIVE_TIME,
  FT_STRING,
  FT_STRINGZ,     /* for use with proto_tree_add_item() */
  FT_EBCDIC,      /* for use with proto_tree_add_item() */
  FT_UINT_STRING, /* for use with proto_tree_add_item() */
  /*FT_UCS2_LE, */    /* Unicode, 2 byte, Little Endian     */
  FT_ETHER,
  FT_BYTES,
  FT_UINT_BYTES,
  FT_IPv4,
  FT_IPv6,
  FT_IPXNET,
  FT_FRAMENUM,    /* a UINT32, but if selected lets you go to frame with that number */
  FT_PCRE,        /* a compiled Perl-Compatible Regular Expression object */
  FT_GUID,        /* GUID, UUID */
  FT_OID,         /* OBJECT IDENTIFIER */
  FT_EUI64,
  FT_NUM_TYPES /* last item number plus one */
} ftenum_t;

typedef enum {
  BASE_NONE,      /**< none */
  BASE_DEC,       /**< decimal */
  BASE_HEX,       /**< hexadecimal */
  BASE_OCT,       /**< octal */
  BASE_DEC_HEX,   /**< decimal (hexadecimal) */
  BASE_HEX_DEC,   /**< hexadecimal (decimal) */
  BASE_CUSTOM     /**< call custom routine (in ->strings) to format */
} base_display_e;

typedef struct {
  int value;
  char* string;
} value_string;

#endif  /* _SCD_DWARF_WENV_H_ */
