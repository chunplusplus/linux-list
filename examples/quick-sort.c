#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#include "common.h"

#define MAX_LEVELS 300

static uint16_t values[256];

static void list_qsort(struct list_head *head)
{
    struct list_head list_less, list_greater;
    struct listitem *pivot;
    struct listitem *item = NULL, *is = NULL;


    if (list_empty(head) || list_is_singular(head))
        return;

    void *beg[MAX_LEVELS], *end[MAX_LEVELS];
    beg[0] = head->next;
    end[0] = head->prev;
    int cur_index = 0;

    do {
        INIT_LIST_HEAD(&list_less);
        INIT_LIST_HEAD(&list_greater);

        // range for quick sort, before for keeping the nodes previous the range
        struct list_head before, range;

        INIT_LIST_HEAD(&before);
        INIT_LIST_HEAD(&range);

        list_cut_position(&before, head, beg[cur_index]);
        list_cut_position(&range, head, end[cur_index]);

        pivot = list_last_entry(&before, struct listitem, list);
        // printf("cur_index: %d, pivot value: %d\n", cur_index, pivot->i);

        list_del(&(pivot->list));

        list_for_each_entry_safe (item, is, &range, list) {
            if (cmpint(&item->i, &pivot->i) < 0)
                list_move_tail(&item->list, &list_less);
            else
                list_move(&item->list, &list_greater);
        }

        cur_index--;

        // record next move from list_less
        if (!(list_empty(&list_less) || list_is_singular(&list_less))) {
            cur_index++;
            beg[cur_index] = list_less.next;
            end[cur_index] = list_less.prev;
        }

        // record next move from list_greater
        if (!(list_empty(&list_greater) || list_is_singular(&list_greater))) {
            cur_index++;
            beg[cur_index] = list_greater.next;
            end[cur_index] = list_greater.prev;
        }

        list_splice(&list_greater, head);
        list_add(&pivot->list, head);
        list_splice(&list_less, head);
        list_splice(&before, head);

    } while (cur_index >= 0);
}

int main(void)
{
    struct list_head testlist;
    struct listitem *item, *is = NULL;
    size_t i;

    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    INIT_LIST_HEAD(&testlist);

    assert(list_empty(&testlist));

    for (i = 0; i < ARRAY_SIZE(values); i++) {
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);
        item->i = values[i];
        list_add_tail(&item->list, &testlist);
    }

    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
    list_qsort(&testlist);

    i = 0;
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->i == values[i]);
        list_del(&item->list);
        free(item);
        i++;
    }

    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));

    return 0;
}
