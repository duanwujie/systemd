/* SPDX-License-Identifier: LGPL-2.1+ */
/***
  Copyright 2018 Jonathan Rudenberg

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include "fuzz.h"
#include "resolved-dns-packet.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
        _cleanup_(dns_packet_unrefp) DnsPacket *p = NULL;
        int r;

        if (size > DNS_PACKET_SIZE_MAX)
                return 0;

        r = dns_packet_new(&p, DNS_PROTOCOL_DNS, 0, DNS_PACKET_SIZE_MAX);
        if (r < 0)
                return 0;
        p->size = 0; /* by default append starts after the header, undo that */
        dns_packet_append_blob(p, data, size, NULL);
        if (size < DNS_PACKET_HEADER_SIZE) {
                /* make sure we pad the packet back up to the minimum header size */
                assert(p->allocated >= DNS_PACKET_HEADER_SIZE);
                memzero(DNS_PACKET_DATA(p) + size, DNS_PACKET_HEADER_SIZE - size);
                p->size = DNS_PACKET_HEADER_SIZE;
        }
        dns_packet_extract(p);

        return 0;
}
