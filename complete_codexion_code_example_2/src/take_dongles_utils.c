/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_dongles_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchahid <abchahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 00:20:39 by abchahid          #+#    #+#             */
/*   Updated: 2026/09/06 13:44:08 by abchahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	lock_physical_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	else
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	pthread_mutex_lock(&first->lock);
	print_action(coder, "has taken a dongle");
	pthread_mutex_lock(&second->lock);
	print_action(coder, "has taken a dongle");
}
