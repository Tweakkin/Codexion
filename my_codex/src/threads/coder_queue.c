/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_queue.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user <user@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 20:42:00 by user             #+#    #+#             */
/*   Updated: 2026/10/07 20:42:00 by user            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_requests(t_queue *queue)
{
	t_request	temp;

	temp = queue->requests[0];
	queue->requests[0] = queue->requests[1];
	queue->requests[1] = temp;
}

static int	should_swap(t_queue *queue)
{
	t_request	*r0;
	t_request	*r1;

	r0 = &queue->requests[0];
	r1 = &queue->requests[1];
	if (r1->priority_value < r0->priority_value)
		return (1);
	if (r1->priority_value == r0->priority_value
		&& r1->compiles_done < r0->compiles_done)
		return (1);
	if (r1->priority_value == r0->priority_value
		&& r1->compiles_done == r0->compiles_done
		&& r1->coder_id < r0->coder_id)
		return (1);
	return (0);
}

void	push_request(t_queue *queue, t_request req)
{
	queue->requests[queue->size] = req;
	if (queue->size == 1 && should_swap(queue))
		swap_requests(queue);
	queue->size++;
}

int	can_take_dongles(t_coder *coder)
{
	if (!coder->left_dongle->is_available)
		return (0);
	if (!coder->right_dongle->is_available)
		return (0);
	if (coder->left_dongle->wait_queue.requests[0].coder_id
		!= coder->id)
		return (0);
	if (coder->right_dongle->wait_queue.requests[0].coder_id
		!= coder->id)
		return (0);
	return (1);
}

void	pop_request(t_queue *queue)
{
	if (queue->size == 2)
		queue->requests[0] = queue->requests[1];
	if (queue->size > 0)
		queue->size--;
}
