/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   drop_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 09:12:03 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 13:38:35 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	drop_dongles(t_coder *coder)
{
	long long	time_now;

	pthread_mutex_unlock(&coder->left_dongle->lock);
	pthread_mutex_unlock(&coder->right_dongle->lock);
	pthread_mutex_lock(&coder->data->state_lock);
	time_now = get_current_time_ms();
	coder->left_dongle->is_available = true;
	coder->left_dongle->last_released_time = time_now;
	coder->right_dongle->is_available = true;
	coder->right_dongle->last_released_time = time_now;
	pthread_cond_broadcast(&coder->data->table_cond);
	pthread_mutex_unlock(&coder->data->state_lock);
}
