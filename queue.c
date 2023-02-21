#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"


/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *merge_two_list(struct list_head *left,
                                 struct list_head *right);

struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    // Ensure that the head is not NULL.
    if (head != NULL) {
        INIT_LIST_HEAD(head);
    }

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l || list_empty(l)) {
        return;
    }
    element_t *item = NULL, *is = NULL;
    list_for_each_entry_safe (item, is, l, list) {
        /*list_del(&item->list);*/
        q_release_element(item);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }

    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node) {
        return false;
    }


    // Copy the input argument s to the member value in the object new_node
    new_node->value = malloc(strlen(s) + 1);
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    // memcpy(dest,source,size_t) from string.h
    memcpy(new_node->value, s, strlen(s));
    new_node->value[strlen(s)] = '\0';

    new_node->list.prev = head;
    new_node->list.next = head->next;
    head->next = &new_node->list;
    new_node->list.next->prev = &new_node->list;

    // cppcheck-suppress memleak
    return true;
}


/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node) {
        return false;
    }


    // Copy the input argument s to the member value in the object new_node
    new_node->value = malloc(strlen(s) + 1);
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    // memcpy(dest,source,size_t) from string.h
    memcpy(new_node->value, s, strlen(s));
    new_node->value[strlen(s)] = '\0';

    new_node->list.next = head;
    new_node->list.prev = head->prev;
    head->prev = &new_node->list;
    new_node->list.prev->next = &new_node->list;
    // cppcheck-suppress memleak
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *remove_node = list_first_entry(head, element_t, list);

    if (sp) {
        strncpy(sp, remove_node->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(head->next);

    return remove_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    // queue_contex_t *start_queue = list_entry(head,queue_contex_t,q);
    element_t *remove_node = list_last_entry(head, element_t, list);

    if (sp) {
        strncpy(sp, remove_node->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    list_del_init(head->prev);
    return remove_node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    int count = 0;
    element_t *item = NULL, *is = NULL;
    list_for_each_entry_safe (item, is, head, list) {
        count += 1;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (list_empty(head)) {
        return false;
    }

    struct list_head **indirect = &(head);
    /*Try to create the same terminal condition as the singular linked list */
    struct list_head *last = head->prev;
    last->next = NULL;
    /*Also,*/
    struct list_head *prev = NULL;

    for (struct list_head *fast = head; fast && fast->next;
         fast = fast->next->next) {
        prev = *indirect;
        indirect = &(*indirect)->next;
    }

    element_t *deleted_node = list_entry(*indirect, element_t, list);

    prev->next = (*indirect)->next;
    (*indirect)->next->prev = prev;

    head->prev->next = head;


    free(deleted_node);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    int count = 1;
    int swap_number = q_size(head) / 2;
    struct list_head *p1 = head->next, *p2 = head->next->next;
    while (count <= swap_number) {
        struct list_head *temp = p1->prev;
        p1->prev = p2;
        p1->next = p2->next;
        p2->prev = temp;
        p2->next = p1;
        p1->next->prev = p1;
        p2->prev->next = p2;

        p1 = p1->next;
        p2 = p1->next;

        count += 1;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head) {
        return;
    }
    int count = q_size(head);
    struct list_head *start = head;
    while (count >= 0) {
        struct list_head *temp = start->prev;
        start->prev = start->next;
        start->next = temp;
        start = start->next;
        count -= 1;
    }
}


static struct list_head *mergesort(struct list_head *head)
{
    if (!head || !head->next) {
        return head;
    }

    // Use the slow and fast pointer to find out the middle node of the list
    struct list_head *fast = head->next, *slow = head;
    for (; fast && fast->next; fast = fast->next->next) {
        slow = slow->next;
    }
    // Now,object slow is in the middle of the list.
    fast = slow->next;  // generate the type list_head* object right
    slow->next = NULL;  // generate the type list_head* object left = head

    return merge_two_list(mergesort(head), mergesort(fast));
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    head->prev->next = NULL;
    head->next = mergesort(head->next);
    struct list_head *ptr = head;
    for (; ptr->next; ptr = ptr->next)
        ptr->next->prev = ptr;
    ptr->next = head;
    head->prev = ptr;
}
/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}
struct list_head *merge_two_list(struct list_head *left,
                                 struct list_head *right)
{
    struct list_head *head = NULL, **indirect = &head, **cur = NULL;
    while (left && right) {
        if (strcmp(container_of(left, element_t, list)->value,
                   container_of(right, element_t, list)->value) >= 0) {
            cur = &right;
        } else {
            cur = &left;
        }
        *indirect = *cur;
        indirect = &(*indirect)->next;
        *cur = (*cur)->next;
    }

    *indirect = (struct list_head *) (void *) ((uintptr_t) (void *) left |
                                               (uintptr_t) (void *) right);

    return head;
}


/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
