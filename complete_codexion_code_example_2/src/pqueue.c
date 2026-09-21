/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pqueue.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/18 10:28:53 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 17:10:06 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	pqueue_create(t_heapq *pqueue)
{
	pqueue->requests = (t_request *)malloc(sizeof(t_request) * 2);
	if (!pqueue->requests)
		return (false);
	pqueue->capacity = 2;
	pqueue->size = 0;
	return (true);
}

bool	pqueue_peek(t_heapq *pqueue, t_request *top)
{
	if (pqueue->size > 0)
	{
		*top = pqueue->requests[0];
		return (true);
	}
	return (false);
}

static void	swap_nodes(t_request *request1, t_request *request2)
{
	t_request	temp;

	temp = *request1;
	*request1 = *request2;
	*request2 = temp;
}

bool	pqueue_push(t_heapq *pqueue, t_request request)
{
	if (!pqueue || pqueue->size >= pqueue->capacity)
		return (false);
	pqueue->requests[pqueue->size] = request;
	if (pqueue->size == 1)
	{
		if (pqueue->requests[1].priority < pqueue->requests[0].priority)
			swap_nodes(&pqueue->requests[0], &pqueue->requests[1]);
		else if (pqueue->requests[1].priority == pqueue->requests[0].priority)
		{
			if (pqueue->requests[1].compiles_done
				< pqueue->requests[0].compiles_done)
			{
				swap_nodes(&pqueue->requests[0], &pqueue->requests[1]);
			}
			else if ((pqueue->requests[1].compiles_done
					== pqueue->requests[0].compiles_done)
				&& (pqueue->requests[1].coder_id
					< pqueue->requests[0].coder_id))
			{
				swap_nodes(&pqueue->requests[0], &pqueue->requests[1]);
			}
		}
	}
	pqueue->size++;
	return (true);
}

bool	pqueue_pop(t_heapq *pqueue, t_request *top)
{
	if (pqueue->size == 0)
		return (false);
	if (top != NULL)
		*top = pqueue->requests[0];
	if (pqueue->size == 2)
		pqueue->requests[0] = pqueue->requests[1];
	pqueue->size--;
	return (true);
}
