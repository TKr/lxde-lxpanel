/**
 * Copyright (c) 2008 LxDE Developers, see the file AUTHORS for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <glib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <iwlib.h>
#include "fnetdaemon.h"
#include "lxnetdaemon.h"
/*
static gboolean
lxnetctl_read_channel(GIOChannel *gio, GIOCondition condition, gpointer data)
{
	GIOStatus ret;
	GError *err = NULL;
	gchar *msg;
	gsize len;

	if (condition & G_IO_HUP)
		g_error ("Read end of pipe died!\n");

	ret = g_io_channel_read_line (gio, &msg, &len, NULL, &err);
	if (ret == G_IO_STATUS_ERROR)
		g_error ("Error reading: %s\n", err->message);

	printf ("Read %u bytes: %s\n", len, msg);

	g_free (msg);

	return TRUE;
}
*/
GIOChannel *
lxnetdaemon_socket(void)
{
	GIOChannel *gio;
	int sockfd;
	struct sockaddr_un sa_un;

	/* create socket */
	sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (sockfd < 0) {
		return NULL;
	}

	/* Initiate socket */
	bzero(&sa_un, sizeof(sa_un));

	/* setting UNIX socket */
	sa_un.sun_family = AF_UNIX;
	snprintf(sa_un.sun_path, sizeof(sa_un.sun_path), LXNETDAEMON_SOCKET);

	if (connect(sockfd, (struct sockaddr *) &sa_un, sizeof (sa_un)) < 0) {
		return NULL;
	}

	gio = g_io_channel_unix_new(sockfd);
	g_io_channel_set_encoding(gio, NULL, NULL);
//	g_io_add_watch(gio, G_IO_IN | G_IO_HUP, lxnetctl_read_channel, NULL);

	return gio;
}

void lxnetdaemon_close(GIOChannel *gio)
{
	close(g_io_channel_unix_get_fd(gio));
}

void
lxnetdaemon_send_command(GIOChannel *gio, int command, const char* cmdargs)
{
	gsize len;

	g_io_channel_write_chars(gio, g_strdup_printf("%d %s", command, cmdargs), -1, &len, NULL);
	g_io_channel_flush(gio, NULL);
}