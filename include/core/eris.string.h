#ifndef __ERIS_STRING_H__
#define __ERIS_STRING_H__

/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : This design to do string of the erishttp reference sds, apis eg:
 **        :    eris_string_alloc
 **        :    eris_string_create
 **        :    eris_string_create_n
 **        :    eris_string_assign
 **        :    eris_string_at
 **        :    eris_string_append
 **        :    eris_string_clear
 **        :    eris_string_copy
 **        :    eris_string_copy_n
 **        :    eris_string_complare
 **        :    eris_string_empty
 **        :    eris_string_erase
 **        :    eris_string_clone
 **        :    eris_string_find
 **        :    eris_string_rfind
 **        :    eris_string_replace
 **        :    eris_string_printf
 **        :    eris_string_vprintf
 **        :    eris_string_reverse
 **        :    eris_string_rotation
 **        :    eris_string_strip
 **        :    eris_string_free
 **        :    ...
 **        : and so on
 **
 ******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "eris.config.h"

#include "os/unix/eris.unix.h"

#include "core/eris.types.h"



/** npos of eris string global variable */
extern const eris_size_t ERIS_STRING_NPOS;

/** The type of eris string */
//typedef eris_char_t *eris_string_t;

/** The eris string array type */
//typedef eris_string_t *eris_string_array_t;


/** All type enum */
enum eris_string_type_e {
    ERIS_STRING_TYPE_5  = 0,
    ERIS_STRING_TYPE_8  = 1,
    ERIS_STRING_TYPE_16 = 2,
    ERIS_STRING_TYPE_32 = 3,
    ERIS_STRING_TYPE_64 = 4
};

/** The eris string type type */
typedef enum eris_string_type_e eris_string_type_t;

/** All eris_string types */
typedef struct eris_string_flags_s eris_string_flags_t;
typedef struct eris_string_5_s     eris_string_5_t;
typedef struct eris_string_8_s     eris_string_8_t;
typedef struct eris_string_16_s    eris_string_16_t;
typedef struct eris_string_32_s    eris_string_32_t;
typedef struct eris_string_64_s    eris_string_64_t;


#pragma pack( push)
#pragma pack( 1)


/** The eris string flags */
struct eris_string_flags_s {
    eris_uchar_t type:3;
    eris_uchar_t zero:5;
};


/** 5bits size eris string */
struct eris_string_5_s {
    /** flags */
    eris_string_flags_t flags;

    /** eris string */
    eris_char_t estring[];
};


/** 5bits size eris string */
struct eris_string_8_s {
    /** Size */
    eris_uint8_t size;

    /** Malloc size */
    eris_uint8_t alloc;

    /** flags */
    eris_string_flags_t flags;

    /** eris string */
    eris_char_t estring[];
};


/** 5bits size eris string */
struct eris_string_16_s {
    /** Size */
    eris_uint16_t size;

    /** Malloc size */
    eris_uint16_t alloc;

    /** flags */
    eris_string_flags_t flags;

    /** eris string */
    eris_char_t estring[];
};


/** 5bits size eris string */
struct eris_string_32_s {
    eris_uint32_t size;

    /** Malloc size */
    eris_uint32_t alloc;

    /** flags */
    eris_string_flags_t flags;

    /** eris string */
    eris_char_t estring[];
};


/** 5bits size eris string */
struct eris_string_64_s {
    eris_uint64_t size;

    /** Malloc size */
    eris_uint64_t alloc;

    /** flags */
    eris_string_flags_t flags;

    /** eris string */
    eris_char_t estring[];
};


/** The eris string array structure */
struct eris_string_array_s {
    /** Array size */
    eris_uint32_t size;

    /** The array context handler */
    eris_string_t array[];
};


#pragma pack( pop)


#define ERIS_STRING_TYPE(s)    (((eris_string_flags_t *)&((s)[-1]))->type)
#define ERIS_STRING_5SIZE(s)   (((eris_string_flags_t *)&((s)[-1]))->zero)
#define ERIS_STRING_VARP(s,T)  eris_string_##T##_t *eris_string_varp = (eris_void_t *)((eris_char_t *)(s) - (sizeof( eris_string_##T##_t)));
#define ERIS_STRING_BASE(s,T)  ((eris_string_##T##_t *)((eris_char_t *)(s) - (sizeof( eris_string_##T##_t))))


/** Eris string init */
#define eris_string_init(s)    ((s) = NULL)


/**
 * @Brief: Set string to eris string object.
 *
 * @Param: __esp, Eris string objects.
 * @Param: __s  , Value string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_set( eris_string_t *__esp, const eris_char_t *__s);


/**
 * @Brief: Set string to eris string object.
 *
 * @Param: __esp, Eris string objects.
 * @Param: __es , Value eris string.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_set_eris( eris_string_t *__esp, const eris_string_t __es);


/**
 * @Brief: Set a charactor at eris string index.
 *
 * @Param: __es, Eris string context.
 * @Param: __at, At index, number.
 * @Param: __c , A charactor.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_set_at_c( eris_string_t __es, eris_size_t __at, const eris_char_t __c);


/**
 * @Brief: Alloc empty eris string object.
 *
 * @Param: _n, The alloc of size n bits.
 *
 * @Return: Successed return eris string object, other, is NULL.
 **/
extern eris_string_t eris_string_alloc( eris_size_t __n);


/**
 * @Brief: Realloc eris string object, old->size + __n.
 *
 * @Param: __es, The old source eris string.
 * @Param: __n,  The alloc of size n bits.
 *
 * @Return: Successed return new eris string object, other, is NULL.
 **/
extern eris_string_t eris_string_realloc( eris_string_t __es, eris_size_t __n);


/**
 * @Brief: From eris string to interger.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of interger value.
 **/
extern eris_int_t eris_string_atoi( const eris_string_t __es);


/**
 * @Brief: From eris string to unsinged interger.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of unsigned interger value.
 **/
extern eris_uint_t eris_string_atou( const eris_string_t __es);


/**
 * @Brief: From eris string to long value.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of long value.
 **/
extern eris_long_t eris_string_atol( const eris_string_t __es);


/**
 * @Brief: From eris string to unsigned long value.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of unsigned long value.
 **/
extern eris_ulong_t eris_string_atoul( const eris_string_t __es);


/**
 * @Brief: From eris string to long long value.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of long long value.
 **/
extern eris_llong_t eris_string_atoll( const eris_string_t __es);


/**
 * @Brief: From eris string to unsigned long long value.
 *
 * @Param: __es, The eris string.
 *
 * @Return: A result of unsigned long long value.
 **/
extern eris_ullong_t eris_string_atoull( const eris_string_t __es);


/**
 * @Brief: From eris_int_t to eris string.
 *
 * @Param: __esp, The output eris string.
 * @Param: __v,   The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_itoa( eris_string_t *__esp, eris_int_t __v);


/**
 * @Brief: From unsigned eris_int_t to eris string.
 *
 * @Param: __esp, The output eris string.
 * @Param: __v,   The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_utoa( eris_string_t *__esp, eris_uint_t __v);


/**
 * @Brief: From long to eris string.
 *
 * @Param: __esp, The output eris string.
 * @Param: __v,   The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_ltoa( eris_string_t *__esp, eris_long_t __v);


/**
 * @Brief: From unsigned long to eris string.
 *
 * @Param: __esp, The output eris string.
 * @Param: __v,   The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_ultoa( eris_string_t *__esp, eris_ulong_t __v);


/**
 * @Brief: From long long to eris string.
 *
 * @Param: __esp, The output eris string.
 * @Param: __v,   The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_lltoa( eris_string_t *__esp, eris_llong_t __v);


/**
 * @Brief: From unsigned long long to eris string.
 *
 * @Param: __esp, The output eris string.
 * @Param: __v,   The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_ulltoa( eris_string_t *__esp, eris_ullong_t __v);


/**
 * @Brief: Hex string to interger.
 *
 * @Param: __es, The output eris string.
 *
 * @Return: Convert result interger.
 **/
extern eris_size_t eris_string_hextov( const eris_string_t __es);


/**
 * @Brief: From type size value to eris string.
 *
 * @Param: __esp, The output eris string.
 * @Param: __v,   The value number.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_vtohex( eris_string_t *__esp, eris_size_t __v);


/**
 * @Brief: Create the eris string.
 *
 * @Param: __s, The source string.
 *
 * @Return: Successed return eris string object, other, is NULL.
 **/
extern eris_string_t eris_string_create( const eris_char_t *__s);


/**
 * @Brief: Create the eris string of size.
 *
 * @Param: __s,  The source string.
 * @Param: __n, Make the size of the eris string object.
 *
 * @Return: Successed return eris string object. other is NULL.
 **/
extern eris_string_t eris_string_create_n( const eris_char_t *__s, eris_size_t __n);


/**
 * @Brief: Assign eris string and output sub eris string.
 *
 * @Param: __es, The eris string object.
 * @Param: __b, The begin index.
 * @Param: __e, The end index.
 *
 * @Return: Successed, return assign sub eris string. other is NULL.
 **/
extern eris_string_t eris_string_assign( const eris_string_t __es, eris_size_t __b, eris_size_t __e);


/**
 * @Brief: Get a eris_char_t of eris string.
 *
 * @Param: __es, The eris string object.
 * @Param: __at, Specify at of index.
 *
 * @Return: If not range out of eris string, return a char; other return '\0'.
 **/
extern eris_char_t eris_string_at( const eris_string_t __es, eris_size_t __at);


/**
 * @Brief: Get the eris string capacity.
 *
 * @Param: __es, The eris string.
 *
 * @Return: The eris string capacity.
 **/
extern eris_size_t eris_string_capacity( const eris_string_t __es);


/**
 * @Brief: The source eris string append tmp eris string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __s  ,  The tmp string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_append( eris_string_t *__esp, const eris_char_t *__s);


/**
 * @Brief: The source eris string append a char.
 *
 * @Param: __esp, The source eris string.
 * @Param: __c  , A tmp char.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_append_c( eris_string_t *__esp, eris_char_t __c);


/**
 * @Brief: The source eris string append tmp eris string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __s,   The tmp string.
 * @Param: __n ,  The size with append tmp eris string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_append_n( eris_string_t *__esp, const eris_char_t *__s, eris_size_t __n);


/**
 * @Brief: The source eris string append tmp eris string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __et , The tmp eris string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_append_eris( eris_string_t *__esp, const eris_string_t __et);


/**
 * @Brief: Cleanup the eris string.
 *
 * @Param: __es, The eris string source.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_cleanup( eris_string_t __es);


/**
 * @Brief: Cleanup as the eris string.
 *
 * @Param: __es, The eris string source.
 * @Param: __b,  The begin index.
 * @Param: __e,  The end index.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_cleanup_as( eris_string_t __es, eris_size_t __b, eris_size_t __e);


/**
 * @Brief: Copy from tmp eris string to source.
 *
 * @Param: __es, The source eris string.
 * @Param: __et, The tmp string.
 *
 * @Return: The result of copy finish source eris string.
 **/
extern eris_string_t eris_string_copy( eris_string_t __es, const eris_char_t *__s);


/**
 * @Brief: Copy from tmp eris string to source with size.
 *
 * @Param: __es, The source eris string.
 * @Param: __et, The tmp eris string.
 * @Param: __n , Copy size of the tmp eris string.
 *
 * @Return: The result of copy finish source eris string.
 **/
extern eris_string_t eris_string_copy_n( eris_string_t __es, const eris_char_t *__s, eris_size_t __n);


/**
 * @Brief: Copy from tmp eris string to source.
 *
 * @Param: __es, The source eris string.
 * @Param: __et, The tmp eris string.
 *
 * @Return: The result of copy finish source eris string.
 **/
extern eris_string_t eris_string_copy_eris( eris_string_t __es, const eris_string_t __et);


/**
 * @Brief: Copy n size from tmp eris string to source.
 *
 * @Param: __es, The source eris string.
 * @Param: __et, The tmp eris string.
 * @Param: __n , Copy size of the tmp eris string.
 *
 * @Return: The result of copy finish source eris string.
 **/
extern eris_string_t eris_string_copy_n_eris( eris_string_t __es, const eris_string_t __et, eris_size_t __n);


/**
 * @Brief: Compare eris string 1 vs othe string.
 *
 * @Param: __es,  The eris string.
 * @Param: __s,   The other string.
 * @Param: __case,If true ignore case, and vice versa.
 *
 * @Return: If equal is true, and vice versa is false.
 **/
extern eris_int_t eris_string_compare( const eris_string_t __es, const eris_char_t *__s, eris_bool_t __case);


/**
 * @Brief: Compare eris string 1 vs other string with size.
 *
 * @Param: __es, The eris string.
 * @Param: __s , The other string.
 * @Param: __n , Compare size of eris string vs other string.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If is equal 0, other lt is EERIS_ERROR, gt is 1.
 **/
extern eris_int_t eris_string_compare_n( const eris_string_t __es, const eris_char_t *__s, eris_size_t __n, eris_bool_t __case);


/**
 * @Brief: Compare eris string 1 vs eris string 2.
 *
 * @Param: __es1,  The eris string one.
 * @Param: __es2,  The eris string two.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If is equal 0, other lt is EERIS_ERROR, gt is 1.
 **/
extern eris_int_t eris_string_compare_eris( const eris_string_t __es1, const eris_string_t __es2, eris_bool_t __case);


/**
 * @Brief: Clone the source string.
 *
 * @Param: __es, THe source string.
 *
 * @Return: The result eris string by clone.
 **/
extern eris_string_t eris_string_clone( const eris_char_t *__s); 


/**
 * @Brief: Clone the source string.
 *
 * @Param: __es, The source string.
 * @Param: __n , Clone size of the source eris string.
 *
 * @Return: The result eris string by clone.
 **/
extern eris_string_t eris_string_clone_n( const eris_char_t *__s, eris_size_t __n); 


/**
 * @Brief: Clone the source eris string.
 *
 * @Param: __es, THe source eris string.
 *
 * @Return: The result eris string by clone.
 **/
extern eris_string_t eris_string_clone_eris( const eris_string_t __es); 


/**
 * @Brief: Get the eris string alloc count.
 *
 * @Param: __es, The eris string.
 *
 * @Return: The eris string alloc count.
 **/
extern eris_size_t eris_string_count( const eris_string_t __es);


/**
 * @Brief: Check the eris string has empty?
 * 
 * @Param: __es, The eris string.
 *
 * @Return: If empty is true, and vice versa is false.
 **/
extern eris_bool_t eris_string_empty( const eris_string_t __es);


/**
 * @Brief: Erase substring from eris string.
 *
 * @Param: __es, The eris string.
 * @Param: __substr, The dest of string.
 *
 * @Param: Nothing.
 **/
extern eris_void_t eris_string_erase( eris_string_t __es, const eris_char_t *__substr);


/**
 * @Brief: Erase substring from eris string.
 *
 * @Param: __es, The eris string.
 * @Param: __at, Begin of this index.
 * @Param: __substr, The dest of string.
 *
 * @Param: Nothing.
 **/
extern eris_void_t eris_string_erase_at( eris_string_t __es, eris_size_t __at, const eris_char_t *__substr);


/**
 * @Brief: Erase substring from eris string.
 *
 * @Param: __es, The eris string.
 * @Param: __esubstr, The dest of eris substring.
 *
 * @Param: Nothing.
 **/
extern eris_void_t eris_string_erase_eris( eris_string_t __es, const eris_string_t __esubstr);


/**
 * @Brief: Erase substring from eris string.
 *
 * @Param: __es, The eris string.
 * @Param: __at, Begin of this index.
 * @Param: __esubstr, The dest of eris substring.
 *
 * @Param: Nothing.
 **/
extern eris_void_t eris_string_erase_at_eris( eris_string_t __es, eris_size_t __at, const eris_string_t __esubstr);


/**
 * @Brief: Find a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
extern eris_size_t eris_string_find( const eris_string_t __es, const eris_char_t *__substr);


/**
 * @Brief: Find a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __at, The index at the source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
extern eris_size_t eris_string_find_at( const eris_string_t __es, eris_size_t __at, const eris_char_t *__substr);


/**
 * @Brief: Find a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
extern eris_size_t eris_string_find_eris( const eris_string_t __es, const eris_string_t __esubstr);


/**
 * @Brief: Find a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __at, The index at the source eris string.
 * @Param: __esubstr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
extern eris_size_t eris_string_find_at_eris( const eris_string_t __es, eris_size_t __at, const eris_string_t __esubstr);


/**
 * @Brief: Find a sub string from the source eris string end.
 *
 * @Param: __es, The source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
extern eris_size_t eris_string_rfind( const eris_string_t __es, const eris_char_t *__substr);


/**
 * @Brief: From end to begin find at a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __at, The index at the source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
extern eris_size_t eris_string_rfind_at( const eris_string_t __es, eris_size_t __at, const eris_char_t *__substr);


/**
 * @Brief: From end to begin find a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
extern eris_size_t eris_string_rfind_eris( const eris_string_t __es, const eris_string_t __esubstr);


/**
 * @Brief: From end to begin find at a sub string of the source eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __at, The index at the source eris string.
 * @Param: __substr, The sub string of dest.
 *
 * @Return: If existed result index of the source eris string, Not found is ERIS_STRING_NPOS.
 **/
extern eris_size_t eris_string_rfind_at_eris( const eris_string_t __es, eris_size_t __at, const eris_string_t __esubstr);


/**
 * @Brief: Compare eris string 1 vs othe string.
 *
 * @Param: __es,  The eris string.
 * @Param: __s,    The other string.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If equal is true, and vice versa is false.
 **/
extern eris_bool_t eris_string_isequal( const eris_string_t __es, const eris_char_t *__s, eris_bool_t __case);


/**
 * @Brief: Compare eris string 1 vs other string with size.
 *
 * @Param: __es,  The eris string.
 * @Param: __s ,   The other string.
 * @Param: __n ,   Compare size of eris string vs other string.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If equal is true, and vice versa is false.
 **/
extern eris_bool_t eris_string_isequal_n( const eris_string_t __es, const eris_char_t *__s, eris_size_t __n, eris_bool_t __case);


/**
 * @Brief: Compare eris string 1 vs eris string 2.
 *
 * @Param: __es1,  The eris string one.
 * @Param: __es2,  The eris string two.
 * @Param: __case, If true ignore case, and vice versa.
 *
 * @Return: If equal is true, and vice versa is false.
 **/
extern eris_bool_t eris_string_isequal_eris( const eris_string_t __es1, const eris_string_t __es2, eris_bool_t __case);


/**
 * @Brief: Replace the source eris string sub string to the replace string.
 *
 * @Param: __es, The source eris string.
 * @Param: __substr, The sub string.
 * @Param: __repstr, The replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_replace( eris_string_t *__esp, const eris_char_t *__substr, const eris_char_t *__repstr);


/**
 * @Brief: Replace at begin of the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __at, At the begin index.
 * @Param: __substr, The sub string.
 * @Param: __repstr, The replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_replace_at( eris_string_t *__esp, eris_size_t __at, const eris_char_t *__substr, const eris_char_t *__repstr);


/**
 * @Brief: Replace all the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __substr, The sub string.
 * @Param: __repstr, The replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_replace_all( eris_string_t *__esp, const eris_char_t *__substr, const eris_char_t *__repstr);


/**
 * @Brief: Replace all the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __at, At the begin index.
 * @Param: __substr, The sub string.
 * @Param: __repstr, The replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_replace_all_at( eris_string_t *__esp, eris_size_t _at, const eris_char_t *__substr, const eris_char_t *__repstr);


/**
 * @Brief: Replace the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __substr, The eris sub string.
 * @Param: __repstr, The eris replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_replace_eris( eris_string_t *__esp, const eris_string_t __substr, const eris_string_t __repstr);


/**
 * @Brief: Replace the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __at, At the begin index.
 * @Param: __substr, The eris sub string.
 * @Param: __repstr, The eris replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_replace_at_eris( eris_string_t *__esp, eris_size_t __at, const eris_string_t __substr, const eris_string_t __repstr);


/**
 * @Brief: Replace all the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __substr, The eris sub string.
 * @Param: __repstr, The eris replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_replace_all_eris( eris_string_t *__esp, const eris_string_t __substr, const eris_string_t __repstr);


/**
 * @Brief: Replace all the source eris string sub string to the replace string.
 *
 * @Param: __esp, The source eris string.
 * @Param: __at, At the begin index.
 * @Param: __substr, The eris sub string.
 * @Param: __repstr, The eris replace string.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_replace_all_at_eris( eris_string_t *__esp, eris_size_t __at, const eris_string_t __substr, const eris_string_t __repstr);


/**
 * @Brief: Get the eris string size.
 *
 * @Param: __es, The eris string object.
 *
 * @Return: The eris string size.
 **/
extern eris_size_t eris_string_size( const eris_string_t __es);


/**
 * @Brief: Do format of eris string. However this function only handles an incompatible subset of 
 *         printf alike format sepcifiers:
 *         %c - C character.
 *         %s - C string.
 *         %S - The eris string.
 *         %i - Signed int.
 *         %u - Unsigned int.
 *         %l - Signed long int.
 *         %L - Unsigned long int.
 *         %I - Signed long long.
 *         %U - Unsigned long long.
 *         %% - Verbatim "%" character.
 *
 * @Param: __es,  The source eris string.
 * @Param: __fmt, The preris_int_t format string.
 * @Param: ...,   The other input arguments.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_printf( eris_string_t *__esp, const eris_char_t *__fmt, ...);


/**
 * @Brief: Append format of eris string. However this function only handles an incompatible subset of 
 *         printf alike format sepcifiers:
 *         %c - C character.
 *         %s - C string.
 *         %S - The eris string.
 *         %i - Signed int.
 *         %u - Unsigned int.
 *         %l - Signed long int.
 *         %L - Unsigned long int.
 *         %I - Signed long long.
 *         %U - Unsigned long long.
 *         %% - Verbatim "%" character.
 *
 * @Param: __es,  The source eris string.
 * @Param: __fmt, The preris_int_t format string.
 * @Param: ...,   The other input arguments.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_printf_append( eris_string_t *__esp, const eris_char_t *__fmt, ...);


/**
 * @Brief: Do format var list of eris string. However this function only handles an incompatible subset of 
 *         printf alike format sepcifiers:
 *         %c - C character.
 *         %s - C string.
 *         %S - The eris string.
 *         %i - Signed int.
 *         %u - Unsigned int.
 *         %l - Signed long int.
 *         %L - Unsigned long int.
 *         %I - Signed long long.
 *         %U - Unsigned long long.
 *         %% - Verbatim "%" character.
 *
 * @Param: __es,  The source eris string.
 * @Param: __fmt, The preris_int_t format string.
 * @Param: __vap  The other input arguments list.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_vprintf( eris_string_t *__esp, const eris_char_t *__fmt, va_list __vap);


/**
 * @Brief: Append format var list of eris string. However this function only handles an incompatible subset of 
 *         printf alike format sepcifiers:
 *         %c - C character.
 *         %s - C string.
 *         %S - The eris string.
 *         %i - Signed int.
 *         %u - Unsigned int.
 *         %l - Signed long int.
 *         %L - Unsigned long int.
 *         %I - Signed long long.
 *         %U - Unsigned long long.
 *         %% - Verbatim "%" character.
 *
 * @Param: __es,  The source eris string.
 * @Param: __fmt, The preris_int_t format string.
 * @Param: __vap  The other input arguments list.
 *
 * @Return: If successed is 0, other is EERIS_ERROR.
 **/
extern eris_int_t eris_string_vprintf_append( eris_string_t *__esp, const eris_char_t *__fmt, va_list __vap);


/**
 * @Brief: Reverse the eris string.
 *
 * @Param: __es, The source eris string.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_reverse( eris_string_t __es);


/**
 * @Brief: Rotation the eris string.
 *
 * @Param: __es, The source eris string.
 * @Param: __pos,The pos of rotation.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_rotation( eris_string_t __es, eris_size_t __pos);


/**
 * @Brief: Strip the right and left blank character.
 *
 * @Param: __es, The eris string object.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_strip( eris_string_t __es);


/**
 * @Brief: Strip the left blank character.
 *
 * @Param: __es, The eris string object.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_lstrip( eris_string_t __es);


/**
 * @Brief: Strip the right blank character.
 *
 * @Param: __es, The eris string object.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_rstrip( eris_string_t __es);


/**
 * @Brief: Split the eris string.
 *
 * @Param: __s,  The source string.
 * @Param: __sp, The splitators string.
 *
 * @Return: If soucessed reuslt of eris string array, other is NULL.
 **/
extern eris_string_array_t eris_string_split( const eris_char_t *__s, const eris_char_t *__sp);


/**
 * @Brief: Split the eris string.
 *
 * @Param: __es, The eris string.
 * @Param: __sp, The splitators string.
 *
 * @Return: If soucessed reuslt of eris string array, other is NULL.
 **/
extern eris_string_array_t eris_string_split_eris( const eris_string_t __es, const eris_char_t *__sp);


/**
 * @Brief: Get at the array element.
 *
 * @Param: __esa, The eris string array.
 * @Param: __at,  At the array index.
 *
 * @Return: The eris string at the array.
 **/
extern const eris_string_t eris_string_array_at( const eris_string_array_t __esa, eris_size_t __at);


/**
 * @Brief: Get the eris string array size.
 *
 * @Param: __esa, The eris string array.
 *
 * @Return: The eris string array size.
 **/
extern eris_size_t eris_string_array_size( const eris_string_array_t __esa);


/**
 * @Brief: Release the eris string array.
 *
 * @Param: __es, The eris string objects.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_array_free( eris_string_array_t __esa);


/**
 * @Brief: To lower with eris string.
 *
 * @Param: __es, The eris string.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_tolower( eris_string_t __es);


/**
 * @Brief: To lower with at begin of the eris string.
 *
 * @Param: __es, The eris string.
 * @param: __at, At the begin of eris string.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_tolower_at( eris_string_t __es, eris_size_t __at);


/**
 * @Brief: To lower with eris string.
 *
 * @Param: __es, The eris string.
 * @param: __n,  To lower n charactors.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_tolower_n( eris_string_t __es, eris_size_t __n);


/**
 * @Brief: To lower with at begin of the eris string n size.
 *
 * @Param: __es, The eris string.
 * @param: __at, At the begin of eris string.
 * @param: __n,  To lower n charactors.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_tolower_at_n( eris_string_t __es, eris_size_t __at, eris_size_t __n);


/**
 * @Brief: To upper with eris string.
 *
 * @Param: __es, The eris string.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_toupper( eris_string_t __es);


/**
 * @Brief: To upper with at begin of the eris string.
 *
 * @Param: __es, The eris string.
 * @param: __at, At the begin of eris string.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_toupper_at( eris_string_t __es, eris_size_t __at);


/**
 * @Brief: To upper with eris string.
 *
 * @Param: __es, The eris string.
 * @param: __n,  To upper n charactors.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_toupper_n( eris_string_t __es, eris_size_t __n);


/**
 * @Brief: To upper with at begin of the eris string n size.
 *
 * @Param: __es, The eris string.
 * @param: __at, At the begin of eris string.
 * @param: __n,  To upper n charactors.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_toupper_at_n( eris_string_t __es, eris_size_t __at, eris_size_t __n);


/**
 * @Brief: Check the eris string start with the dest string.
 * 
 * @Param: __es, The eris string.
 * @Param: __s,  The dest string.
 *
 * @Return: If start with is true, other is false.
 **/
extern eris_bool_t eris_string_start_with( const eris_string_t __es, const eris_char_t *__s);


/**
 * @Brief: Check the eris string start with the dest string.
 * 
 * @Param: __es, The eris string.
 * @Param: __et, The dest eris string.
 *
 * @Return: If start with is true, other is false.
 **/
extern eris_bool_t eris_string_start_with_eris( const eris_string_t __es, const eris_string_t __et);


/**
 * @Brief: Check the eris string end with the dest string.
 * 
 * @Param: __es, The eris string.
 * @Param: __s,  The dest string.
 *
 * @Return: If end with is true, other is false.
 **/
extern eris_bool_t eris_string_end_with( const eris_string_t __es, const eris_char_t *__s);


/**
 * @Brief: Check the eris string end with the dest string.
 * 
 * @Param: __es, The eris string.
 * @Param: __et, The dest eris string.
 *
 * @Return: If end with is true, other is false.
 **/
extern eris_bool_t eris_string_end_with_eris( const eris_string_t __es, const eris_string_t __et);


/**
 * @Brief: Release the eris string.
 *
 * @Param: __es, The eris string objects.
 *
 * @Return: Nothing.
 **/
extern eris_void_t eris_string_free( eris_string_t __es);


#ifdef __cplusplus
}
#endif

#endif /** __ERIS_STRING_H__ */

