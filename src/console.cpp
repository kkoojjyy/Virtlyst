/*
 * Copyright (C) 2018 Daniel Nicoletti <dantti12@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "console.h"

#include "virtlyst.h"
#include "lib/connection.h"
#include "lib/domain.h"

#include <libvirt/libvirt.h>

Console::Console(Virtlyst *parent) : Controller(parent)
  , m_virtlyst(parent)
{

}

void Console::index(Context *c, const QString &hostId, const QString &uuid)
{
    QStringList errors;
    c->setStash(QStringLiteral("template"), QStringLiteral("console-vnc.html"));
    c->setStash(QStringLiteral("host_id"), hostId);
    c->setStash(QStringLiteral("time_refresh"), 8000);

    Connection *conn = m_virtlyst->connection(hostId);
    if (conn == nullptr) {
        fprintf(stderr, "Failed to open connection to qemu:///system\n");
        return;
    }
    c->setStash(QStringLiteral("host"), QVariant::fromValue(conn));

    virDomainPtr domain = virDomainLookupByUUIDString(conn->raw(), uuid.toUtf8().constData());
    if (!domain) {
        errors.append(QStringLiteral("Domain not found: no domain with matching name '%1'").arg(uuid));
        c->setStash(QStringLiteral("errors"), errors);
        return;
    }
    auto dom = new Domain(domain, conn, c);
    dom->loadXml();
}