/******************************************************************************
**
** FILE NAME  : ppacmd.h
** PROJECT    : PPA Configuration Utility
** MODULES    : Packet Acceleration
**
** DATE     : 28 Nov 2011
** AUTHOR     : Shao Guohua
** DESCRIPTION  : PPA (Routing Acceleration) User Configuration Utility
** COPYRIGHT  : Copyright (c) 2020, MaxLinear, Inc.
**              Copyright (c) 2009, Lantiq Deutschland GmbH
**              Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
**
**
*******************************************************************************/
#ifndef PPACMD_H_
#define PPACMD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <ctype.h>

/*safec v3 standard compatibility*/
#include <libsafec/safe_str_lib.h>
#include <libsafec/safe_mem_lib.h>

/* Note: The below macro is used for the new safec v3 standard update.
** This will help in future updates in standard to be incorporated.
*/
#define ppa_memset(buff, sz, val) \
  memset_s(buff, sz, val, sz);


#define PPACMD_VERION_MAJOR   2
#define PPACMD_VERION_MID     9
#define PPACMD_VERION_MINOR   0

#define PPA_TEST_AUTOMATION_ENABLE 0


/* note, we canot include this header file, otherwise uint32_t will conflicts with system defined.
** So we have to define some macro here: */
#define AF_INET     2
#define AF_INET6    10
extern int inet_aton(const char *cp, void *inp);
extern int inet_pton(int af, const char *src, void *dst);

typedef signed char	     int8_t;
typedef short		     int16_t;
typedef int		     int32_t;

#undef CONFIG_IFX_PMCU  //for some macro is not defined in PPA, but PMCU, so here disabled it
#include <net/ppa_api.h>


#define PPA_DEVICE   "/dev/ifx_ppa"
#define PPA_CMD_NAME "ppacmd"

#define PPACMD_DEBUG             1

#if PPACMD_DEBUG
extern int enable_debug ;
#define IFX_PPACMD_DBG(args...) if( enable_debug) printf(args)
#else
#define IFX_PPACMD_DBG(args...)
#endif

#define IFX_PPACMD_PRINT(args...)   printf(args)

#define PPA_CMD_OK          0
#define PPA_CMD_ERR         UINT_MAX
#define PPA_CMD_HELP        2
#define PPA_CMD_DONE        3
#define PPA_CMD_NOT_AVAIL       4


#define OPT_ENABLE_LAN        1
#define OPT_DISABLE_LAN       2
#define OPT_ENABLE_WAN        3
#define OPT_DISABLE_WAN       4

#define OPT_VLAN_AWARE        1
#define OPT_OUTER_VLAN_AWARE    2

#define PPA_MAX_CMD_OPTS      20

#define NIPQUAD(addr) \
  ((unsigned char *)&addr)[0], \
  ((unsigned char *)&addr)[1], \
  ((unsigned char *)&addr)[2], \
  ((unsigned char *)&addr)[3]

//#define ntohs
#undef NIP6
extern uint16_t ntohs(uint16_t netshort);
#define NIP6(addr) \
     ntohs(((unsigned short *)addr)[0]), \
     ntohs(((unsigned short *)addr)[1]), \
     ntohs(((unsigned short *)addr)[2]), \
     ntohs(((unsigned short *)addr)[3]), \
     ntohs(((unsigned short *)addr)[4]), \
     ntohs(((unsigned short *)addr)[5]), \
     ntohs(((unsigned short *)addr)[6]), \
     ntohs(((unsigned short *)addr)[7])
#define NUM_ENTITY(x)               (sizeof(x) / sizeof(*(x)))
//#define ENABLE_IPV6       1


typedef struct ppa_cmd_opts_t
{
    int   opt;
    char *optarg;
} PPA_CMD_OPTS;

typedef struct ppa_cmd_t
{
    const char   *name;                       // command string
    int       ioctl_cmd;                    // ioctl command value
    void     (*print_help)(int);                // function to print help for the command: 0 means first level, 1 mean 2nd level
    int      (*parse_options)(PPA_CMD_OPTS *,PPA_CMD_DATA *);   //
    int      (*do_command)(struct ppa_cmd_t *,PPA_CMD_DATA *);  // function to do error checking on options and fill out PPA_CMD_DATA
    void     (*print_data)(PPA_CMD_DATA *);           // print any returned values from PPA driver
    const struct option *long_opts;                 // long options for command
    const char  *short_opts;                    // short options for command
} PPA_COMMAND;

#define MAX_CMD_OPTS        8

#define PPACMD_MAX_FILENAME     32
#define STRING_TYPE_INTEGER 0
#define STRING_TYPE_IP 1
#define IP_NON_VALID 0
#define IP_VALID_V4 1
#define IP_VALID_V6 2


/* This file is from Linux Kernel (include/linux/list.h) 
 * and modified by simply removing hardware prefetching of list items. 
 * Here by copyright, credits attributed to wherever they belong.
 * Kulesh Shanmugasundaram (kulesh [squiggly] isis.poly.edu)
 */

struct list_head {
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *new,
                  struct list_head *prev,
                  struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is in an undefined state.
 */
static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = (void *) 0;
    entry->prev = (void *) 0;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry); 
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move(struct list_head *list, struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void list_move_tail(struct list_head *list,
                  struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add_tail(list, head);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}

static inline void __list_splice(struct list_head *list,
                 struct list_head *head)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void list_splice_init(struct list_head *list,
                    struct list_head *head)
{
    if (!list_empty(list)) {
        __list_splice(list, head);
        INIT_LIST_HEAD(list);
    }
}

/**
 * list_entry - get the struct for this entry
 * @ptr:    the &struct list_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * list_for_each    -   iterate over a list
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:   the head for your list.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); \
            pos = pos->next)
/**
 * list_for_each_prev   -   iterate over a list backwards
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:   the head for your list.
 */
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); \
            pos = pos->prev)
            
/**
 * list_for_each_safe   -   iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop counter.
 * @n:      another &struct list_head to use as temporary storage
 * @head:   the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * list_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop counter.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)              \
    for (pos = list_entry((head)->next, typeof(*pos), member);  \
         &pos->member != (head);                    \
         pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop counter.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)          \
    for (pos = list_entry((head)->next, typeof(*pos), member),  \
        n = list_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head);                    \
         pos = n, n = list_entry(n->member.next, typeof(*n), member))


#endif
