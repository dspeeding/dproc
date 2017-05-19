#include "basetype.h"
#include "Lhash.h"

#undef MIN_NODES
#define MIN_NODES       16
#define UP_LOAD         (2*LH_LOAD_MULT) /* load times 256 (default 2) */
#define DOWN_LOAD       (LH_LOAD_MULT) /* load times 256 (default 1) */

static void expand(_LHASH *lh);
static void contract(_LHASH *lh);
static LHASH_NODE **getrn(IN _LHASH *lh, IN const void *data, IN unsigned long *rhash);

/*************************************************************************
@Purpose :  ����Hash��
@Param   :  h		--	��ϣ����ָ��
@Param   :	c		--	�ȽϺ���ָ��
@Return  :  �������ɵ�HASHָ�룬���ʧ�ܷ���NULL
@Modify  :
@Remark  :	���cΪ�գ���Ĭ��ʹ��strcmp ����
			���hΪ�գ���Ĭ��ʹ��lh_strhash ����
 *************************************************************************/
_LHASH *lh_new(IN LHASH_HASH_FN_TYPE h, IN LHASH_COMP_FN_TYPE c)
{
    _LHASH *ret;
    int i;

    if ((ret = (_LHASH*)malloc(sizeof(_LHASH))) == NULL)
	{
        goto err0;
	}
    if ((ret->b = (LHASH_NODE **)malloc(sizeof(LHASH_NODE *) * MIN_NODES)) == NULL)
	{
        goto err1;
	}
    for (i = 0; i < MIN_NODES; i++)
	{
        ret->b[i] = NULL;
	}
    ret->comp = ((c == NULL) ? (LHASH_COMP_FN_TYPE)strcmp : c);
    ret->hash = ((h == NULL) ? (LHASH_HASH_FN_TYPE)lh_strhash : h);
    ret->num_nodes = MIN_NODES / 2;
    ret->num_alloc_nodes = MIN_NODES;
    ret->p = 0;
    ret->pmax = MIN_NODES / 2;
    ret->up_load = UP_LOAD;
    ret->down_load = DOWN_LOAD;
    ret->num_items = 0;

    ret->num_expands = 0;
    ret->num_expand_reallocs = 0;
    ret->num_contracts = 0;
    ret->num_contract_reallocs = 0;
    ret->num_hash_calls = 0;
    ret->num_comp_calls = 0;
    ret->num_insert = 0;
    ret->num_replace = 0;
    ret->num_delete = 0;
    ret->num_no_delete = 0;
    ret->num_retrieve = 0;
    ret->num_retrieve_miss = 0;
    ret->num_hash_comps = 0;

    ret->error = 0;
    return (ret);
 err1:
    free(ret);
 err0:
    return (NULL);
}

/*************************************************************************
@Purpose :  �ͷŹ�ϣ��
@Param   :  lh		--	��ϣ��ָ��
@Return  :  None
@Modify  :
@Remark  :
 *************************************************************************/
void lh_free(IN _LHASH *lh)
{
    unsigned int i;
    LHASH_NODE *n, *nn;

    if (lh == NULL)
        return;

    for (i = 0; i < lh->num_nodes; i++) 
	{
        n = lh->b[i];
        while (n != NULL) 
		{
            nn = n->next;
            free(n);
            n = nn;
        }
    }
    free(lh->b);
    free(lh);
}

/*************************************************************************
@Purpose :  ��������
@Param   :  lh		--	��ϣ��ָ��
@Param   :	data	--	�����������
@Return  :  None
@Modify  :
@Remark  :
 *************************************************************************/
void *lh_insert(IN _LHASH *lh, IN void *data)
{
    unsigned long hash;
    LHASH_NODE *nn, **rn;
    void *ret;

    lh->error = 0;

	//���װ�����ӳ������ޣ�����չ��ϣ��
    if (lh->up_load <= (lh->num_items * LH_LOAD_MULT / lh->num_nodes))
	{
        expand(lh);
	}
    rn = getrn(lh, data, &hash);
    if (*rn == NULL) 
	{
		//������ĩβ׷������
        if ((nn = (LHASH_NODE *)malloc(sizeof(LHASH_NODE))) == NULL) 
		{
            lh->error++;
            return (NULL);
        }
        nn->data = data;
        nn->next = NULL;
#ifndef OPENSSL_NO_HASH_COMP
        nn->hash = hash;
#endif
        *rn = nn;
        ret = NULL;
        lh->num_insert++;
        lh->num_items++;
    } 
	else 
	{                    
		//�������ֵͬ�����滻Ϊ�µ�����
        ret = (*rn)->data;
        (*rn)->data = data;
        lh->num_replace++;
    }
    return (ret);
}

/*************************************************************************
@Purpose :  ɾ������
@Param   :  lh		--	��ϣ��ָ��
@Param   :	data	--	��ɾ��������
@Return  :  ���ص�����ָ�룬ʧ���򷵻�NULL
@Modify  :
@Remark  :
 *************************************************************************/
void *lh_delete(IN _LHASH *lh, IN const void *data)
{
    unsigned long hash;
    LHASH_NODE *nn, **rn;
    void *ret;

    lh->error = 0;
    rn = getrn(lh, data, &hash);

    if (*rn == NULL) 
	{
        lh->num_no_delete++;
        return (NULL);
    } 
	else 
	{
        nn = *rn;
        *rn = nn->next;
        ret = nn->data;
        free(nn);
        lh->num_delete++;
    }

    lh->num_items--;
    if ((lh->num_nodes > MIN_NODES) &&
        (lh->down_load >= (lh->num_items * LH_LOAD_MULT / lh->num_nodes)))
        contract(lh);

    return (ret);
}

/*************************************************************************
@Purpose :  ��������
@Param   :  lh		--	��ϣ��ָ��
@Param   :	data	--	����ѯ������
@Return  :  ���ص�����ָ�룬ʧ���򷵻�NULL
@Modify  :
@Remark  :
 *************************************************************************/
void *lh_retrieve(IN _LHASH *lh, IN const void *data)
{
    unsigned long hash;
    LHASH_NODE **rn;
    void *ret;

    lh->error = 0;
    rn = getrn(lh, data, &hash);

    if (*rn == NULL) 
	{
        lh->num_retrieve_miss++;
        return (NULL);
    } 
	else 
	{
        ret = (*rn)->data;
        lh->num_retrieve++;
    }
    return (ret);
}

/*************************************************************************
@Purpose :  ����������ִ�в���
@Param   :  lh		--	��ϣ��ָ��
@Param   :	use_arg	--	0--��ʾ����Ҫ���� 1--��ʾ��Ҫ����
@Param   :	func	--	ִ�в����ĺ�����ַ
@Param   :	arg		--	ִ�к���ʱ��Ҫ�Ĳ���
@Return  :  None
@Modify  :
@Remark  :	
 *************************************************************************/
static void doall_util_fn(IN _LHASH *lh, IN int use_arg, IN LHASH_DOALL_FN_TYPE func,
                          IN LHASH_DOALL_ARG_FN_TYPE func_arg, IN void *arg)
{
    int i;
    LHASH_NODE *a, *n;

    if (lh == NULL)
        return;

    /*
     * reverse the order so we search from 'top to bottom' We were having
     * memory leaks otherwise
     */
    for (i = lh->num_nodes - 1; i >= 0; i--) 
	{
        a = lh->b[i];
        while (a != NULL) 
		{
            /*
             * 28/05/91 - eay - n added so items can be deleted via lh_doall
             */
            /*
             * 22/05/08 - ben - eh? since a is not passed, this should not be
             * needed
             */
            n = a->next;
            if (use_arg)
                func_arg(a->data, arg);
            else
                func(a->data);
            a = n;
        }
    }
}

/*************************************************************************
@Purpose :  ����������ִ�в���
@Param   :  lh		--	��ϣ��ָ��
@Param   :	func	--	ִ�в����ĺ�����ַ
@Return  :  None
@Modify  :
@Remark  :
 *************************************************************************/
void lh_doall(IN _LHASH *lh, IN LHASH_DOALL_FN_TYPE func)
{
    doall_util_fn(lh, 0, func, (LHASH_DOALL_ARG_FN_TYPE)0, NULL);
}

/*************************************************************************
@Purpose :  ����������ִ�в�������Ҫ������
@Param   :  lh		--	��ϣ��ָ��
@Param   :	func	--	ִ�в����ĺ�����ַ
@Param   :	arg		--	ִ�к���ʱ��Ҫ�Ĳ���
@Return  :  None
@Modify  :
@Remark  :
 *************************************************************************/
void lh_doall_arg(IN _LHASH *lh, IN LHASH_DOALL_ARG_FN_TYPE func, IN void *arg)
{
    doall_util_fn(lh, 1, (LHASH_DOALL_FN_TYPE)0, func, arg);
}

/*************************************************************************
@Purpose :  �Թ�ϣ�������չ
@Param   :  lh		--	[IN] ��ϣ��ָ��
@Return  :  None
@Modify  :
@Remark  :	֮ǰֻʹ����b[0]-b[7] ������չ��ÿ��һ��
 *************************************************************************/
static void expand(_LHASH *lh)
{
    LHASH_NODE **n, **n1, **n2, *np;
    unsigned int p, i, j;
    unsigned long hash, nni;

    lh->num_nodes++;
    lh->num_expands++;
    p = (int)lh->p++;
    n1 = &(lh->b[p]);
    n2 = &(lh->b[p + (int)lh->pmax]);
    *n2 = NULL;                 /* 27/07/92 - eay - undefined pointer bug */
    nni = lh->num_alloc_nodes;

	//���ڱ�����չ��������е���
    for (np = *n1; np != NULL;) 
	{
#ifndef OPENSSL_NO_HASH_COMP
        hash = np->hash;
#else
        hash = lh->hash(np->data);
        lh->num_hash_calls++;
#endif
		//����ϣֵ��չ�������ռ�
        if ((hash % nni) != p) 
		{ 
			//�����������Ĺ�ϣֵ��ԭ����һ��������õ�b[pmax+p]�����±�
            *n1 = (*n1)->next;
            np->next = *n2;
            *n2 = np;
        } 
		else
		{
            n1 = &((*n1)->next);
		}
        np = *n1;
    }

	//����ռ�̫С�����������
    if ((lh->p) >= lh->pmax) 
	{
        j = (int)lh->num_alloc_nodes * 2;
        n = (LHASH_NODE **)realloc(lh->b,
                                           (int)(sizeof(LHASH_NODE *) * j));
        if (n == NULL) 
		{
/*                      fputs("realloc error in lhash",stderr); */
            lh->error++;
            lh->p = 0;
            return;
        }
        /* else */
        for (i = (int)lh->num_alloc_nodes; i < j; i++) /* 26/02/92 eay */
            n[i] = NULL;        /* 02/03/92 eay */
        lh->pmax = lh->num_alloc_nodes;
        lh->num_alloc_nodes = j;
        lh->num_expand_reallocs++;
        lh->p = 0;
        lh->b = n;
    }
}

static void contract(_LHASH *lh)
{
    LHASH_NODE **n, *n1, *np;

    np = lh->b[lh->p + lh->pmax - 1];
    lh->b[lh->p + lh->pmax - 1] = NULL; /* 24/07-92 - eay - weird but :-( */
    if (lh->p == 0) 
	{
        n = (LHASH_NODE **)realloc(lh->b,
                                           (unsigned int)(sizeof(LHASH_NODE *)
                                                          * lh->pmax));
        if (n == NULL) 
		{
/*                      fputs("realloc error in lhash",stderr); */
            lh->error++;
            return;
        }
        lh->num_contract_reallocs++;
        lh->num_alloc_nodes /= 2;
        lh->pmax /= 2;
        lh->p = lh->pmax - 1;
        lh->b = n;
    }
	else
	{
        lh->p--;
	}

    lh->num_nodes--;
    lh->num_contracts++;

    n1 = lh->b[(int)lh->p];
    if (n1 == NULL)
	{
        lh->b[(int)lh->p] = np;
	}
    else
	{
		//��λ��b[p]�����ĩβ
        while (n1->next != NULL)
		{
            n1 = n1->next;
		}
		//ָ��b[p+pmax-1]
        n1->next = np;
    }
}

/*************************************************************************
@Purpose :  ��ȡ���ݵ�λ��ָ�뼰��ϣ���
@Param   :  lh		--	��ϣ��ָ��
@Param   :	data	--	����ָ��
@Param   :	rhash	--	������Ĺ�ϣֵ
@Return  :  ����λ�õ�ָ��
@Modify  :
@Remark  :	
 *************************************************************************/
static LHASH_NODE **getrn(IN _LHASH *lh, IN const void *data, IN unsigned long *rhash)
{
    LHASH_NODE **ret, *n1;
    unsigned long hash, nn;
    LHASH_COMP_FN_TYPE cf;

    hash = (*(lh->hash)) (data);
    lh->num_hash_calls++;
    *rhash = hash;

	//ֻʹ�õ���ϣ��ǰ�벿��
    nn = hash % lh->pmax;
    if (nn < lh->p)		//���С��p�������й���չ������Ҫ�õ���ϣ��ȫ������num_alloc_nodes
	{
        nn = hash % lh->num_alloc_nodes;
	}
    cf = lh->comp;
    ret = &(lh->b[(int)nn]);
    for (n1 = *ret; n1 != NULL; n1 = n1->next) 
	{
#ifndef OPENSSL_NO_HASH_COMP
        lh->num_hash_comps++;
        if (n1->hash != hash) 
		{
            ret = &(n1->next);
            continue;
        }
#endif
        lh->num_comp_calls++;
        if (cf(n1->data, data) == 0)
		{
			//���ֵһ���򷵻�
            break;
		}
        ret = &(n1->next);
    }
    return (ret);
}

/*
 * The following hash seems to work very well on normal text strings no
 * collisions on /usr/dict/words and it distributes on %2^n quite well, not
 * as good as MD5, but still good.
 */
unsigned long lh_strhash(const char *c)
{
    unsigned long ret = 0;
    long n;
    unsigned long v;
    int r;

    if ((c == NULL) || (*c == '\0'))
        return (ret);
/*-
    unsigned char b[16];
    MD5(c,strlen(c),b);
    return(b[0]|(b[1]<<8)|(b[2]<<16)|(b[3]<<24));
*/

    n = 0x100;
    while (*c) 
	{
        v = n | (*c);
        n += 0x100;
        r = (int)((v >> 2) ^ v) & 0x0f;
        ret = (ret << r) | (ret >> (32 - r));
        ret &= 0xFFFFFFFFL;
        ret ^= v * v;
        c++;
    }
    return ((ret >> 16) ^ ret);
}

unsigned long lh_num_items(const _LHASH *lh)
{
    return lh ? lh->num_items : 0;
}
